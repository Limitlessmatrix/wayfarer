#include <stdint.h>

#define RGB565(r, g, b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

#define LCD_WIDTH 360
#define LCD_HEIGHT 360

void frame_init();
void frame_clear();
void frame_refresh();
void frame_draw_pixel(int16_t x, int16_t y, uint16_t color);
void frame_draw_pixel_alpha(int16_t x, int16_t y, uint16_t color, uint8_t alpha);
void frame_draw_digit(uint16_t x, uint16_t y, uint8_t digit, uint16_t color);
void frame_draw_number(int16_t x, int16_t y, uint32_t number, uint16_t color);
void frame_draw_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);
void frame_fill_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);
void frame_fill_rect_alpha(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t alpha);
int frame_draw_char(int16_t x, int16_t y, char c, uint16_t color);
void frame_draw_string(int16_t x, int16_t y, const char* str, uint16_t color);
int frame_get_yadvance();
int frame_calculate_max_width(int y);
int frame_calculate_text_height(const char* str);
int frame_calculate_text_width(const char* str);
int frame_calculate_text_fit(const char* str, int max_width);
char* frame_draw_string_fit(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t max_width);
void frame_draw_string_centered(int16_t y, const char* str, uint16_t color);
void frame_draw_long_string(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t max_width);
int frame_draw_text(const char* str, uint16_t color, int16_t y);
void frame_draw_text_overflow(const char* str, uint16_t color, int16_t y_offset);
void frame_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
int frame_draw_dashed_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, int offset);
void frame_draw_image(int16_t x, int16_t y, const uint16_t* image_data, uint16_t width, uint16_t height);
void frame_draw_sprite(int16_t x, int16_t y, const uint16_t* image_data, uint16_t width, uint16_t height, uint16_t img_x, uint16_t img_y, uint16_t img_pitch);
void frame_draw_image_alpha(int16_t x, int16_t y, const uint16_t* image_data, uint16_t width, uint16_t height, uint8_t alpha);
void frame_draw_hexagon(int16_t x, int16_t y, uint16_t size, uint16_t color);
void frame_draw_hexagon_grid(int16_t x, int16_t y, uint16_t size, uint16_t color);
void frame_draw_rounded_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);
void frame_draw_circle(int16_t x, int16_t y, uint16_t r, uint16_t color);
void show_console_ui(const char* message);