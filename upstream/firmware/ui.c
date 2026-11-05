#include "ui.h"
#include "framebuffer.h"
#include "buttons.h"
#include <stdint.h>
#include <string.h>
#include "img/crew.h"
#include "config.h"
#include "terminal.h"
#include "img/message.h"
#include <stdio.h>

void gui_get_input(const char* prompt, char* buffer, int buffer_size) {

    int selected_index = 0;
    int buffer_index = 0;
    while (true) {
        frame_clear();
        frame_draw_text(prompt, title_color, 30); // Yellow color
        frame_draw_text(buffer, description_color, 70); // White color
        int sx = selected_index % 10;
        int sy = selected_index / 10;
        frame_fill_rect(78 + sx * 20, 102 + sy * 30, 16, 26, selected_color); // Highlight selected character
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 10; col++) {
                if (row * 10 + col < 26) {
                    frame_draw_char(col * 20 + 80, row * 30 + 120, 'A' + row * 10 + col, option_color);
                } else if (row * 10 + col == 26) {
                    frame_draw_char(col * 20 + 80, row * 30 + 120, '_', option_color);
                } else if (row * 10 + col == 27) {
                    frame_draw_char(col * 20 + 80, row * 30 + 120, ' ', option_color);
                } else if (row * 10 + col == 28) {
                    frame_draw_char(col * 20 + 80, row * 30 + 120, '<', option_color);
                } else if (row * 10 + col == 29) {
                    frame_draw_char(col * 20 + 80, row * 30 + 120, '>', option_color);
                }
            }
        }
        frame_refresh();

        uint32_t keys = buttons_get_and_clear();
        if (keys & BUTTON_UP) {
            selected_index = (selected_index - 10 + 30) % 30;
        }
        if (keys & BUTTON_DOWN) {
            selected_index = (selected_index + 10) % 30;
        }
        if (keys & BUTTON_LEFT) {
            selected_index = (selected_index - 1 + 30) % 30;
        }
        if (keys & BUTTON_RIGHT) {
            selected_index = (selected_index + 1) % 30;
        }
        if (keys & BUTTON_PRESS) {
            if (buffer_index < buffer_size - 1) {
                char selected_char = (selected_index < 26) ? ('A' + selected_index) : '_'; // Map 0-25 to A-Z, 26-29 to '_'
                if (selected_index == 26) selected_char = '_';
                else if (selected_index == 27) selected_char = ' ';
                else if (selected_index == 28) { //special case for backspace
                    if (buffer_index > 0) { 
                        buffer_index--;
                        buffer[buffer_index] = '\0';
                    }
                    continue; // Don't add a character for backspace

                }
                else if (selected_index == 29) { //special case for return
                    return; // Finish input on return
                }
                buffer[buffer_index++] = selected_char;
                buffer[buffer_index] = '\0';
                if (buffer_index >= buffer_size - 1) {
                    buffer_index = buffer_size - 1;
                }
            }
            else if (selected_index == 28) { // Allow backspace if buffer is full
                if (buffer_index > 0) { 
                    buffer_index--;
                    buffer[buffer_index] = '\0';
                }            }
            else if (selected_index == 29) { // Allow return even if buffer is
                return; // Buffer full, finish input
            }
        }
    }
}


void gui_show_message(const char* message) {
    int idx=1;
    for (const char* ptr = message; *ptr; ptr++) {
        frame_clear();
        frame_draw_image(174, 2, radio_image_data, 31, 30);
        frame_draw_image(78, 38, incoming_image_data, 204, 12);
        frame_draw_image(123, 300, secure_image_data, 110, 8);
        frame_draw_rounded_rect(30, 80, 300, 200, title_color);
        
        const char *str = message;
        int16_t y = 99;
        int ccount=0;
        while (ccount < idx) {
            int line_chars = frame_calculate_text_fit(str, 292);
            char temp_str[256];
            memcpy(temp_str, str, line_chars);
            temp_str[line_chars] = '\0';
            int width = frame_calculate_text_width(temp_str);
            if (ccount + line_chars > idx) {
                temp_str[idx - ccount] = '\0'; // Truncate to the current index if it exceeds
            }
            //Dirty hack!
            uint16_t color = description_color;
            char c = config_get_class();
            char* pat=strstr(temp_str, get_class_name(c));
            if (pat) {
                color = get_crew_color(c);
            }
            pat = strstr(temp_str, config_get_handle());
            if (pat) {
                char t2[20];
                *pat = '\0';
                int dx = frame_calculate_text_width(temp_str);
                snprintf(t2, sizeof(t2), "%s.", config_get_handle());
                frame_draw_string(34 + dx, y, t2, RGB565(250,0,250));
            }
            frame_draw_string(34, y, temp_str, color); 
            y += frame_get_yadvance(); // Move to the next line (height of the font + some spacing)
            ccount+=line_chars;
            str += line_chars; // Move the pointer forward by the number of characters that fit
        }

        if (message[++idx] == '\0') {
            frame_draw_image(112, 326, pressa_image_data, 133, 8);
        }

        frame_refresh();
        int delay = 30; // Base delay for each character
        if (*ptr == '\n') {
            delay = 200; // Longer delay for new lines
        } else if (*ptr == '.' || *ptr == '!' || *ptr == '?') {
            delay = 600; // Longer delay for punctuation
        }
        terminal_update();
        if (!(buttons_get_raw() & (BUTTON_A | BUTTON_B))) {

             sleep_ms(delay); // Only delay if no button is pressed, allowing users to skip through the text faster
        }
        else {
             //sleep_ms(1); // Short delay to debounce button presses
        }
    }
    buttons_clear();
    while (buttons_get_and_clear() == 0) {
        terminal_update();
        sleep_ms(100);
    }
}

void gui_show_crew(const char* name, const char class, int y) {
    frame_draw_rounded_rect(80, y, 200, 44, border_color);
    if (name) {
        const char* pclass = get_class_name(class);
        frame_draw_text(name, description_color, y + 18);
        int xoff = frame_calculate_text_width(pclass) + 20;
        frame_draw_string((LCD_WIDTH - xoff)/2 + 20, y + 38 , pclass, get_crew_color(class));
        frame_draw_image((LCD_WIDTH - xoff)/2, y + 22, player_img_data[class - 'A'], 15, 16);
    }
    else {
        frame_draw_text("Vacant", unavailable_color, y + 28);
    }
}

void ui_draw_popup(const char* str) {
    int pad = 5;
    int h = frame_calculate_text_height(str) + 2*pad;
    int w = frame_calculate_text_width(str) + 2*pad;
    int cx = LCD_WIDTH / 2;
    int cy = LCD_HEIGHT /2;

    int x = cx - w/2;
    int y = cy - h/2;

    int y0 = frame_get_yadvance();
    frame_fill_rect(x+2, y+2, w-4, h-4, background_color);
    frame_draw_rounded_rect(x, y, w, h, border_color);
    frame_draw_text(str, selected_color, y + y0 + pad -2);
    sleep_ms(5);
    frame_refresh();
}