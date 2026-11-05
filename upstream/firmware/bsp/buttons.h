#ifndef BUTTONS_H
#define BUTTONS_H
#include "pico/stdlib.h"

#define BUTTON_UP    (1 << 0)
#define BUTTON_DOWN  (1 << 1)
#define BUTTON_LEFT  (1 << 2)
#define BUTTON_RIGHT (1 << 3)
#define BUTTON_PRESS (1 << 4)
#define BUTTON_B     (1 << 5)
#define BUTTON_A     (1 << 6)

#define GPIO_BTN1       16
#define GPIO_BTN2       17
#define GPIO_JOY_PRESS  20
#define GPIO_JOY_DOWN   21
#define GPIO_JOY_UP     23
#define GPIO_JOY_RIGHT  19
#define GPIO_JOY_LEFT   22
#define GPIO_OFFLINE    29

void buttons_init();
void buttons_clear();
//int buttons_get();
int buttons_get_and_clear();
int buttons_get_raw();
uint64_t buttons_get_last_activity_us();

#endif // BUTTONS_H