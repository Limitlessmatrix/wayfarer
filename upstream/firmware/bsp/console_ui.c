#include "framebuffer.h"
#include "buttons.h"
#include "pico/stdlib.h"


void show_console_ui(const char* message) {
    frame_fill_rect_alpha(20, 120, LCD_WIDTH - 40, LCD_HEIGHT - 240, RGB565(0, 0, 0), 128); // Semi-transparent black background
    // TODO draw boarder around the console UI
    int x = 20;
    int y = 120;
    frame_refresh();
    for (const char* ptr = message; *ptr; ptr++) {
        int delay = 40; // Base delay for each character
        if (*ptr == '\n') {
            x = 20;
            y += frame_get_yadvance(); // Move down for new line
            delay = 300; // Longer delay for new lines
        } else {
            x += frame_draw_char(x, y, *ptr, RGB565(0, 255, 0)); // Green color for console text
        }
        if (x > LCD_WIDTH - 20) {
            x = 20;
            y += frame_get_yadvance(); // Move down if we reach the end of the line
        }
        if (*ptr == '.' || *ptr == '!' || *ptr == '?') {
            delay = 800; // Longer delay for punctuation
        }

        if (!(buttons_get_raw() & BUTTON_PRESS)) {
             sleep_ms(delay); // Only delay if no button is pressed, allowing users to skip through the text faster
        }
        else {
             sleep_ms(5); // Short delay to debounce button presses
        }
        frame_refresh();
    }
    while(buttons_get_raw()) {
        sleep_ms(10);
        // Wait for all buttons to be released before allowing exit
    }
    buttons_clear(); // Clear any button presses that happened during the text display
    while (!(buttons_get_and_clear() & BUTTON_PRESS)) {
        sleep_ms(10);
        // Wait for button press to exit console UI
    }
}