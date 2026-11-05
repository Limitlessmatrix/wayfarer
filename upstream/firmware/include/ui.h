#ifndef UI_H
#define UI_H

#include "framebuffer.h"

void gui_show_message(const char* message);
void gui_get_input(const char* prompt, char* buffer, int buffer_size);
void gui_show_menu(const char* title, const char** options, int option_count, int selected_index);
void gui_show_crew(const char* name, const char class, int y);
void ui_draw_popup(const char* str);
extern const unsigned short sel1_image_data[256];
extern const unsigned short sel2_image_data[256];

static const uint16_t selected_color = RGB565(80, 80, 250);
static const uint16_t unavailable_color = RGB565(40, 40, 40);
static const uint16_t title_color = RGB565(108, 209, 183);
static const uint16_t description_color = RGB565(150, 150, 150);
static const uint16_t option_color = RGB565(150, 150, 150);
static const uint16_t border_color = RGB565(198,150,69);
static const uint16_t background_color = RGB565(25, 25, 25);
static const uint16_t special_option_color = RGB565(100, 100, 200); // Blue for special options
static const uint16_t sbackground_color = RGB565(15, 54, 37);

#endif // UI_H