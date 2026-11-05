// Small host-side helpers exported from libwayfarer.so for the Python frontend.
#include <stdint.h>
#include <string.h>
#include "platform_2026.h"

// Convert the 360x360 RGB565 framebuffer to packed RGB888 into out (360*360*3 bytes).
// swap_bytes: set when the firmware stores pixels high byte first for the panel DMA.
void emu_framebuffer_rgb888(uint8_t *out, int swap_bytes) {
    const uint16_t *fb = emu_framebuffer();
    int n = emu_fb_width() * emu_fb_height();
    for (int i = 0; i < n; i++) {
        uint16_t p = fb[i];
        if (swap_bytes) p = (uint16_t)((p << 8) | (p >> 8));
        out[i * 3 + 0] = (uint8_t)(((p >> 11) & 0x1F) * 255 / 31);
        out[i * 3 + 1] = (uint8_t)(((p >> 5) & 0x3F) * 255 / 63);
        out[i * 3 + 2] = (uint8_t)((p & 0x1F) * 255 / 31);
    }
}
