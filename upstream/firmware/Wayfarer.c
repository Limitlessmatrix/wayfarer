#include <stdio.h>
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "bsp/framebuffer.h"
#include "bsp/buttons.h"
#include "bsp/ws2812.h"
#include "bsp/terminal.h"
#include "ui.h"
#include "bsp/config.h"
#include "bsp/eeprom.h"
#include "blink.pio.h"
#include "img/boot.h"

const char unlock_code[] = "\0SC_THRUST_LOSS";
void run_main_loop();
extern bool offline_mode;

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq) {
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    pio->txf[sm] = (125000000 / (2 * freq)) - 3;
}

void show_bootscreen(int valid) {
    int eret = eeprom_test();
    uint64_t starthold = 0;
    ws2812_set_pattern_color((ws2812_rgb_t){16, 0, 16});    //Purple  
    
    while (true) {
        frame_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, RGB565(118, 66, 138));
        if (eret)   {
            frame_draw_text("SELF TEST PASSED", RGB565(0,200,0), 60);
        }
        else {
            frame_draw_text("EEPROM FAILED!", RGB565(200,0,0), 60);
        }
        if (valid) {
            frame_draw_text("EEPROM DATA VALID", RGB565(0,200,0), 90);
        }
        else {
            frame_draw_text("EEPROM DATA INVALID!", RGB565(200,0,0), 90);
        }
        int buttons = buttons_get_raw();
        int coords[7][2] = {{132,156}, {132,188}, {116,172}, {148,172}, {132,172}, {204,172}, {220,172}};
        for (int i=0; i<7; ++i) {
            if (buttons & (1<<i)) {
                frame_fill_rect(coords[i][0], coords[i][1], 16, 16, RGB565(63, 204, 172));
            }
            frame_draw_image(coords[i][0], coords[i][1], btn_icons[i], 16, 16);
        }

        frame_draw_rect(104, 259, 152, 16, RGB565(255,255,255));
        frame_draw_text("HOLD A TO RESET EEPROM DATA", RGB565(255,190,190), 250);
        uint64_t now = time_us_64();
        if (buttons & BUTTON_A) {
            if (starthold == 0) {
                starthold = now;
            }
        }
        else {
            starthold = 0;
        }
        if (starthold > 0) {
            uint64_t elapsed_ms = (now - starthold) / 1000;
            int w = elapsed_ms / 20;
            frame_fill_rect(105, 260, w, 14, RGB565(255, 100, 0));
            if (elapsed_ms > 3000) {
                frame_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, RGB565(128, 0, 0));
                frame_refresh();
                config_clear();
                return;
            }
        }
        frame_draw_image(108, 300, boot_code_image_data, 144, 13);
        frame_refresh();
    }
}

int main()
{
    //stdio_uart_init_full(uart0, 9600, 0, 1); 
    stdio_usb_init();
    stdio_puts(unlock_code);
    //stdio_init_all();
    terminal_init();
    console_init();
    eeprom_init();
    
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blink_program);
    blink_pin_forever(pio, 0, offset, MB_CLK_PIN, 1);
    
    buttons_init();
    frame_init();
    
    offline_mode = gpio_get(GPIO_OFFLINE) == 0;

    int valid_config = config_load();
    ws2812_init(pio0, 1, WS2812_PIN);

    if ((!valid_config) || (buttons_get_and_clear() & BUTTON_B)) { //Inital setup
        show_bootscreen(valid_config);
    }

    if (offline_mode) {
        printf("Offline mode enabled\n");
        config_achievement(offline);
    }

    switch(config_get_class()) {
        case 'A': ws2812_set_pattern_color((ws2812_rgb_t){0, 0, 16});break;   //Pilot = Blue
        case 'B': ws2812_set_pattern_color((ws2812_rgb_t){16, 6, 0});break;  //Engineer = Orange
        case 'C': ws2812_set_pattern_color((ws2812_rgb_t){16, 16, 16});break;   //Scientist = White
        case 'D': ws2812_set_pattern_color((ws2812_rgb_t){0, 16, 0});break;   //Soldier = Green
        case 'E': ws2812_set_pattern_color((ws2812_rgb_t){16, 6, 6});break;   //Medic = Pink
        default: ws2812_set_pattern_color((ws2812_rgb_t){16, 0, 16});    //Purple  
    }

    while (true) {
        run_main_loop();
    }
}

