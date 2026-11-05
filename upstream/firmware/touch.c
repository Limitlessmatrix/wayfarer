#include "touch.h"

static volatile int tap_x = 0, tap_y = 0, tap_pending = 0;
static volatile int drag_dx = 0, drag_dy = 0;
static volatile int ptr_x = 0, ptr_y = 0, ptr_down = 0;
static volatile int scene = 0;

#ifdef WAYFARER_HOST
// Exported to the host frontend.
void platform_2026_tap(int x, int y) { tap_x = x; tap_y = y; tap_pending = 1; }
void platform_2026_drag(int dx, int dy) { drag_dx += dx; drag_dy += dy; }
int  platform_2026_tap_pending(void) { return tap_pending; }
void platform_2026_pointer(int x, int y, int down) { ptr_x = x; ptr_y = y; ptr_down = down; }
int  platform_2026_scene(void) { return scene; }
void platform_2026_tap_clear(void) { tap_pending = 0; }
#endif

bool ui_pointer(int* x, int* y) {
    if (!ptr_down) return false;
    *x = ptr_x; *y = ptr_y;
    return true;
}

void ui_set_scene(int s) { scene = s; }

bool ui_take_tap(int* x, int* y) {
    if (!tap_pending) return false;
    tap_pending = 0;
    *x = tap_x; *y = tap_y;
    return true;
}

bool ui_take_drag(int* dx, int* dy) {
    if (!drag_dx && !drag_dy) return false;
    *dx = drag_dx; *dy = drag_dy;
    drag_dx = drag_dy = 0;
    return true;
}

bool ui_tap_in(int tx, int ty, int x, int y, int w, int h) {
    return tx >= x && tx < x + w && ty >= y && ty < y + h;
}

// Rows are drawn with text baselines y0 + i*step; a row owns roughly [baseline-17, baseline+step-17).
int ui_tap_row(int ty, int y0, int step, int n) {
    int i = (ty - (y0 - 17)) / step;
    if (ty < y0 - 17 || i < 0 || i >= n) return -1;
    return i;
}
