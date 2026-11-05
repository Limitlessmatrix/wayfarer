#include "terminal.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "pico/sync.h"
#include "pico/time.h"
#include "pico/stdio_usb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "config.h"
#include "pico/unique_id.h"
#include "common.h"
#include "base64.h"
#include "bsp_komihash.h"
#include "bsp_gost28147.h"
#include "eeprom.h"

#define CONSOLE_BUFFER_SIZE 128
#define UART_BUFFER_SIZE 512
#define DOCKING_COMMAND_QUEUE_SIZE 8
#define UART_ID uart0

static char console_buffer[CONSOLE_BUFFER_SIZE];
static size_t console_index = 0;
static char pending_console_command[CONSOLE_BUFFER_SIZE];
static volatile bool pending_console_command_ready = false;
bool usb_banner_printed = false;
    
static char uart_buffer[UART_BUFFER_SIZE];
static size_t uart_index = 0;
static char uart_input_buffer[UART_BUFFER_SIZE];
static volatile bool uart_input_ready = false;
static struct repeating_timer uart_timer;

static char docking_command_queue[DOCKING_COMMAND_QUEUE_SIZE][UART_BUFFER_SIZE];
static size_t docking_command_head = 0;
static size_t docking_command_tail = 0;
static size_t docking_command_count = 0;

static void uart_poll(void);
static bool terminal_enqueue_docking_command(const char *command);
static void console_handle_command(char *command);

extern uint16_t pending_score;

char gost_key[32] = {
    0x4e, 0x8b, 0x67, 0x8d, 0xc1, 0xac, 0xbf, 0x60, 0x7a, 0x83, 0xa8, 0xbb, 0x04, 0xd7, 0x96, 0x10,
    0xfd, 0xdd, 0xa2, 0x30, 0xcb, 0xd7, 0x74, 0xf7, 0x16, 0xbc, 0x1d, 0x0c, 0x84, 0xab, 0x17, 0xaf};

const char gost_enc[] = {
    0x22, 0x2f, 0x55, 0x91, 0xde, 0x66, 0x43, 0x11,
    0x0f, 0xd9, 0xa3, 0x3d, 0xb4, 0xb6, 0x74, 0x15,
    0x13, 0x41, 0xd4, 0x47, 0x10, 0xf1, 0x0d, 0x58
};

void uart_poll() {
    uint32_t irq_state = save_and_disable_interrupts();
    bool ready = uart_input_ready;
    if (ready) {
        uart_input_ready = false;
    }
    restore_interrupts(irq_state);

    if (!ready) {
        return;
    }

    char input_buffer[UART_BUFFER_SIZE];
    memset(input_buffer, 0, sizeof(input_buffer));
    size_t input_len = strnlen(uart_input_buffer, sizeof(input_buffer) - 1);
    if (input_len == 0) {
        uart_input_buffer[0] = '\0';
        return;
    }
    memcpy(input_buffer, uart_input_buffer, input_len);
    input_buffer[input_len] = '\0';
    uart_input_buffer[0] = '\0';

    if (input_buffer[0] != '$') {
        volatile char dock_unlock[] = {4, 20, 8, 7, 31, 22, 4, 18, 8, 19, 5, 30, 17, 3, 0x57}; //SC_PHASE_DRIFT
        for (int i=0;i<sizeof(dock_unlock); ++i) {
            dock_unlock[i] ^= 0x57;
        }
        printf("Bad RX: %s\r\n", input_buffer);
        terminal_send(dock_unlock);
        return;
    }
    if (strncmp(input_buffer, "$UNLOCK ", 8) == 0) {
        const char *unlock_code = input_buffer + 8;
        int result = config_submit(unlock_code);
        if (result == 1) {
            stdio_puts("$UNLOCK OK");
        } else if (result == 0) {
            stdio_puts("$UNLOCK ALREADY UNLOCKED");
        } else {
            stdio_puts("$UNLOCK: INVALID");
        }
        return;
    } else if (strcmp(input_buffer, "$UNLOCK") == 0) {
        stdio_puts("$UNLOCK: requires unlock code argument. Usage: $UNLOCK <code>");
        return;
    }
    // The rest of these commands are only valid when the game is running, so ignore them otherwise
    if (game_running && strncmp(input_buffer, "$MP", 3) != 0) {
        printf("Game running, please undock\r\n");
        return;
    }
    //printf("RX: %s", input_buffer);
    terminal_enqueue_docking_command(input_buffer);
}

static void console_process_pending_command(void) {
    if (!pending_console_command_ready) {
        return;
    }

    pending_console_command_ready = false;
    console_handle_command(pending_console_command);
}

void terminal_update() {
    console_process_pending_command();
    if (stdio_usb_connected()) {
        if (!usb_banner_printed) {
            stdio_puts("\r\n");
            stdio_puts("============================== terminal connected ==============================\r\n");
            stdio_puts("                +                           D                       Y           \r\n");
            stdio_puts("     D                   T                          J                           \r\n");
            stdio_puts("                             S                               G                  \r\n");
            stdio_puts("           V                            +                                       \r\n");
            stdio_puts("                    H             B                     O                       \r\n");
            stdio_puts("================================================================================\r\n");
            stdio_printf("> ");
            usb_banner_printed = true;
        }
    } else {
        usb_banner_printed = false;
    }
}

static bool uart_timer_callback(struct repeating_timer *t) {
    (void)t;
    uart_poll();
    return true;
}

static void uart_irq(void) {
    while (uart_is_readable(UART_ID)) {
        int ch = uart_getc(UART_ID);
        //uart_putc(UART_ID, ch);
        if (ch == '\r' || ch == '\n') {
            if (uart_index == 0) {
                continue;
            }
            size_t len = uart_index;
            if (len >= UART_BUFFER_SIZE) {
                len = UART_BUFFER_SIZE - 1;
            }
            memcpy(uart_input_buffer, uart_buffer, len);
            uart_input_buffer[len] = '\0';
            uart_input_ready = true;
            memset(uart_buffer, 0, sizeof(uart_buffer));
            uart_index = 0;
        } else if (ch == '\b' || ch == 0x7F) {
            if (uart_index > 0) {
                uart_index--;
                uart_buffer[uart_index] = '\0';
            }
        } else {
            if (uart_index >= UART_BUFFER_SIZE - 1) {
                /* Drop the rest of the current line instead of overflowing the buffer. */
                uart_index = 0;
                memset(uart_buffer, 0, sizeof(uart_buffer));
                uart_input_buffer[0] = '\0';
                continue;
            }
            uart_buffer[uart_index++] = (char)ch;
            uart_buffer[uart_index] = '\0';
        }
    }
}

void terminal_init() {
    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_set_irq_enables(UART_ID, true, false);
    irq_set_exclusive_handler(UART0_IRQ, uart_irq);
    irq_set_enabled(UART0_IRQ, true);

    add_repeating_timer_ms(1, uart_timer_callback, NULL, &uart_timer);
}

void terminal_send(const char* command) {
    if (!command) {
        return;
    }

    size_t len = strlen(command);
    uart_write_blocking(UART_ID, (const uint8_t *)command, len);
    uart_write_blocking(UART_ID, (const uint8_t *)"\r\n", 2);
}

static void terminal_print_hex_bytes(const unsigned char *data, size_t len) {
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0; i < len; ++i) {
        unsigned char value = data[i];
        stdio_putchar(hex[(value >> 4) & 0x0F]);
        stdio_putchar(hex[value & 0x0F]);
        if ((i & 0x1F) == 0x1F) {
            sleep_ms(5);
        }
    }
    stdio_puts("\r\n");
}

int flag_unlock = 1;

static void console_handle_command(char *command) {
    char cmd[CONSOLE_BUFFER_SIZE];
    size_t len = strlen(command);
    size_t out = 0;

    while (len > 0 && (command[len - 1] == '\r' || command[len - 1] == '\n' || command[len - 1] == ' ' || command[len - 1] == '\t')) {
        len--;
    }

    strncpy(cmd,command, sizeof(cmd)-1);
    out = strlen(cmd);

    if (out == 0) {
        stdio_printf("> ");
        return;
    }

    if (strncasecmp(cmd, "UNLOCK ", 7) == 0) {
        const char *unlock_code = cmd + 7;
        for (size_t i = 0; i < out; ++i) {
            cmd[i] = toupper((unsigned char)cmd[i]);
        }
        int result = config_submit(unlock_code);
        if (result == 1) {
            stdio_puts("UNLOCK: OK");
        } else if (result == 0) {
            stdio_puts("UNLOCK: ALREADY UNLOCKED");
        } else {
            stdio_puts("UNLOCK: INVALID");
        }
    } else if (strncasecmp(cmd, "UNLOCK", 6) == 0) {
        stdio_puts("UNLOCK: requires unlock code argument. Usage: UNLOCK <code>");
    } else if (strncasecmp(cmd, "IDENT", 5) == 0) {
        stdio_printf("IDENT \"%s\"\r\n", config_get_handle());
    } else if (strncasecmp(cmd, "HANDLE ", 7) == 0) {
        const char *name = cmd + 7;
        while (*name == ' ') name++;
        int name_len = strlen(name);
        if (name_len < 3) {
            stdio_puts("Handle too short.");
        }
        else if (name_len > 14) {
            stdio_puts("Handle too long.");
        }
        else {
            int valid = true;
            for (int i=0; i<name_len; ++i) {
                char at = name[i];
                if (at==' ') continue;
                else if (at == '_') continue;
                else if ((at >= '-') && (at <= '9')) continue;
                else if ((at >= '@') && (at <= 'Z')) continue;
                else if ((at >= 'a') && (at <= 'z')) continue;
                else {
                    valid = false;
                    stdio_puts("Handle contains invalid characters.");
                    break;
                }
            }
            if (valid) {
                config_set_handle(name);
                stdio_printf("Your new handle is \"%s\"\r\n", config_get_handle());
            }
        }
    } else if (strncasecmp(cmd, "HANDLE", 6) == 0) {
        stdio_puts("HANDLE: requires new handle argument. Usage: HANDLE <new_handle>");
#ifdef WAYFARER_HOST
    } else if (strncasecmp(cmd, "DBG ", 4) == 0) {
        // Host emulator only: test hooks for scripted runs. Never compiled into the badge image.
        extern int wayfarer_debug_command(const char* args);
        stdio_printf("DBG %s\r\n", wayfarer_debug_command(cmd + 4) ? "OK" : "FAIL");
#endif
    } else if (strncasecmp(cmd, "HELP", 4) == 0) {
        stdio_puts("Commands: HANDLE <new_handle>, UNLOCK <code>, IDENT");
        volatile char cdc_unlock[] = {6, 22, 10, 17, 0, 6, 1, 10, 19, 25, 0, 13, 0x55}; //SC_DUST_FLUX
        for (int i=0;i<sizeof(cdc_unlock); ++i) {
            cdc_unlock[i] ^= 0x55;
        }
        stdio_puts(cdc_unlock);
    } else if (strncasecmp(cmd, "CLEAR", 5) == 0) {
        stdio_puts("\x1b[2J\x1b[H"); // ANSI escape code to clear screen and move cursor to home
    } else if (strncasecmp(cmd, "FLAG ", 5) == 0) {
        char flag[24] = "{Rusty_C0d3_1s_4w3s0m3}";
        if (cmd[5] == '3') {
            //{Fly1ng_C4r_Pr0m1s3}
            memcpy(flag, gost_enc, 24);
            gost28147_set_key(gost_key);
            gost28147_decrypt(flag);
            gost28147_decrypt(flag+8);
            gost28147_decrypt(flag+16);
            for (int i=0;i<23;++i) {
                flag[i] = '*';
            }
        } else if (cmd[5] == '2') {
            char eedata[8];
            eeprom_readbytes(0, eedata, 8);
            if (strncmp(eedata, "PLUM", 4) == 0) {
                // obfuscated bytes for "{R4ygun_G0th1c_Ph4s3}" 
                const unsigned char enc[] = {
                    0xCA,0xE3,0x27,0x80,0x41,0x3F,0x18,0x74,0x7D,0x47,0xDF,
                    0x42,0xAB,0x97,0x6A,0x77,0x68,0xE7,0x8F,0x9B,0xC2
                };
                int enc_len = (int)sizeof(enc);
                for (int i = 0; i < enc_len; ++i) {
                    uint8_t e = enc[i];
                    uint8_t step1 = e ^ (uint8_t)(0xA5 ^ (uint8_t)i);
                    uint8_t step2 = (uint8_t)(step1 + (uint8_t)(i * 3));
                    uint8_t step3 = (uint8_t)((step2 << 3) | (step2 >> 5));
                    flag[i] = (char)step3;
                }
                if (enc_len < (int)sizeof(flag)) {
                    flag[enc_len] = '\0';
                    if (enc_len + 1 < (int)sizeof(flag))
                        memset(flag + enc_len + 1, 0, sizeof(flag) - (enc_len + 1));
                }
            }   
            else {
                strncpy(flag, "INVALID", sizeof(flag));
            }
        } else if (cmd[5] == '1') {
            //{At0m1c_Ut0p1a_1955}
            const char volatile flag1[] = {
                0xb7, 0x8d, 0xb8, 0xfc, 0xa1, 0xfd, 0xaf, 0x93, 0x99, 0xb8, 0xfc, 0xbc,
                0xfd, 0xad, 0x93, 0xfd, 0xf5, 0xf9, 0xf9, 0xb1, 0xcc};
            for (int i=0;i<sizeof(flag1);++i) {
                flag[i] = flag1[i] ^ 0xcc;
            }
        } else {
            strcpy(flag, "INVALID ARGUMENT");
        }
        flag[23] = '\0';
        printf("Flag: %s\r\n", flag);
    } else if (strncasecmp(cmd, "FLAG", 4) == 0) {
        stdio_puts("Flag: requires argument");
    } else {
        stdio_puts("Unknown command. Type HELP for commands.");
    }

    stdio_printf("> ");
}

bool terminal_pop_docking_command(char *command, size_t max_len) {
    bool queued = false;
    uint32_t irq_state = save_and_disable_interrupts();
    if (docking_command_count > 0) {
        size_t len = strnlen(docking_command_queue[docking_command_head], max_len - 1);
        memcpy(command, docking_command_queue[docking_command_head], len);
        command[len] = '\0';
        docking_command_head = (docking_command_head + 1) % DOCKING_COMMAND_QUEUE_SIZE;
        docking_command_count--;
        queued = true;
    }
    restore_interrupts(irq_state);
    return queued;
}

static bool terminal_enqueue_docking_command(const char *command) {
    bool queued = false;
    uint32_t irq_state = save_and_disable_interrupts();
    if (docking_command_count < DOCKING_COMMAND_QUEUE_SIZE) {
        size_t len = strnlen(command, UART_BUFFER_SIZE - 1);
        memcpy(docking_command_queue[docking_command_tail], command, len);
        docking_command_queue[docking_command_tail][len] = '\0';
        docking_command_tail = (docking_command_tail + 1) % DOCKING_COMMAND_QUEUE_SIZE;
        docking_command_count++;
        queued = true;
    }
    else {
        printf("Docking command queue full, dropping command: %s\n", command);
    }
    restore_interrupts(irq_state);
    return queued;
}

void on_usb_rx(void *v) {
    int ch;
    while ((ch = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
        if ((ch == '\n') || (ch == '\r')) {
            if (console_index > 0) {
                stdio_putchar('\r');
                stdio_putchar('\n');
                console_buffer[console_index] = '\0';
                if (console_index + 1 < sizeof(pending_console_command)) {
                    memcpy(pending_console_command, console_buffer, console_index + 1);
                    pending_console_command_ready = true;
                }
                console_index = 0;
            }
        } else if (ch == '\b' || ch == 0x7F) {
            if (console_index > 0) {
                console_index--;
                stdio_putchar('\b');
                stdio_putchar(' ');
                stdio_putchar('\b');
            }
        } else {
            stdio_putchar(ch);
            if (console_index + 1 < CONSOLE_BUFFER_SIZE) {
                console_buffer[console_index++] = (char)ch;
            }
        }
    }
}

void console_init() {
    stdio_set_chars_available_callback(on_usb_rx, NULL);
}