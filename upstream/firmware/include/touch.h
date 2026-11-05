#ifndef TOUCH_H
#define TOUCH_H
// Pointer input for the emulator (touch pad / mouse on the round screen). The badge has no touch
// panel, so on hardware every call reports nothing and the d-pad paths stay the only input.
#include <stdbool.h>
bool ui_take_tap(int* x, int* y);      // one shot tap at screen coordinates
bool ui_take_drag(int* dx, int* dy);   // pointer drag accumulated since the last call
bool ui_tap_in(int tx, int ty, int x, int y, int w, int h);
int  ui_tap_row(int ty, int y0, int step, int n);   // row index for a baseline list, -1 if none
#endif
