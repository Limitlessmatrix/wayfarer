#include "ws2812.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include <string.h>
#include "hardware/timer.h"
#include "pico/time.h"
#include "config.h"

static PIO ws2812_pio;
static uint ws2812_sm;
static uint ws2812_offset;

struct repeating_timer led_timer = {0};

#ifdef VERSION_01
const uint16_t led_pattern[5] = {0x100, 0x81, 0x42, 0x24, 0x18}; 
#else
const uint16_t led_pattern[5] = {0x80, 0x140, 0x21, 0x12, 0x0C}; 
#endif


ws2812_rgb_t led_pattern_buffer[WS2812_NUM_LEDS];
uint8_t led_pattern_index = 0;
ws2812_rgb_t led_color = (ws2812_rgb_t){16, 0, 16};
ws2812_rgb_t alert_color;
int alert=0;

extern char* achieve_str;

bool ws2812_callback(__unused struct repeating_timer *t) {
    ws2812_pattern_update();
    return true; // Return true to repeat the alarm, false to cancel it
}

inline static void set_led_speed() {
    int count = config_get_achievement_count();
    cancel_repeating_timer(&led_timer);
    add_repeating_timer_ms(-170 + count * 5, ws2812_callback, NULL, &led_timer);
}
// Convert RGB to GRB (WS2812 format)
static uint32_t rgb_to_grb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void ws2812_init(PIO pio, uint sm, uint pin) {
    ws2812_pio = pio;
    ws2812_sm = sm;

    // Load the WS2812 program into PIO memory
    ws2812_offset = pio_add_program(pio, &ws2812_program);

    // Initialize the program
    ws2812_program_init(pio, sm, ws2812_offset, pin, 800000.0f, false);

    set_led_speed();
}

void ws2812_show(const ws2812_rgb_t *colors, uint num_leds) {
    for (uint i = 0; i < num_leds; i++) {
        uint32_t grb = rgb_to_grb(colors[i].r, colors[i].g, colors[i].b);
        put_pixel(ws2812_pio, ws2812_sm, grb);
    }
    // Wait for the data to be sent
    // The PIO will automatically handle the reset timing
}

void ws2812_set_led(uint index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= WS2812_NUM_LEDS) return;

    uint32_t grb = rgb_to_grb(r, g, b);
    put_pixel(ws2812_pio, ws2812_sm, grb);
}

void ws2812_clear(void) {
    for (uint i = 0; i < WS2812_NUM_LEDS; i++) {
        put_pixel(ws2812_pio, ws2812_sm, 0);
    }
}

void ws2812_set_pattern_color(ws2812_rgb_t color) {
    led_color = color;
    set_led_speed();
}

void ws2812_pattern_update(void) {
    for (uint i = 0; i < WS2812_NUM_LEDS; i++) {
        if (alert) {
            if ((led_pattern_index)%2) {
                led_pattern_buffer[i] = (ws2812_rgb_t){0, 0, 0};
            }
            else {
                led_pattern_buffer[i] = alert_color;
            }
        }
        else {
            if (led_pattern[led_pattern_index%5] & (1 << i)) {
                led_pattern_buffer[i] = led_color; 
            } else {
                led_pattern_buffer[i] = (ws2812_rgb_t){0, 0, 0};
            }
        }
    }
    ws2812_show(led_pattern_buffer, WS2812_NUM_LEDS);
    led_pattern_index++;
    if (led_pattern_index >= 5) {
        led_pattern_index = 0;
        if (alert > 0) {
            alert--;
        }
        else {
            achieve_str = NULL;
        }
    }
}

void ws2812_show_alert(ws2812_rgb_t color) {
    set_led_speed();
    alert_color = color;
    alert=3;
    led_pattern_index=0;
}