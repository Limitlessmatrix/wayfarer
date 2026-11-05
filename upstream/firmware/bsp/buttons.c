#include "buttons.h"
#include "hardware/gpio.h"
#include <hardware/sync.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdint.h>
#include "config.h"

#define BUTTON_DEBOUNCE_MS 30   
#define NUM_BUTTONS 7

typedef struct {
    uint gpio;
    int mask;
} button_map_t;

static const button_map_t BUTTONS[NUM_BUTTONS] = {
    {GPIO_JOY_UP,    BUTTON_UP},
    {GPIO_JOY_DOWN,  BUTTON_DOWN},
    {GPIO_JOY_LEFT,  BUTTON_LEFT},
    {GPIO_JOY_RIGHT, BUTTON_RIGHT},
    {GPIO_JOY_PRESS, BUTTON_PRESS},
    {GPIO_BTN1,      BUTTON_B},
    {GPIO_BTN2,      BUTTON_A}
};

static uint64_t last_debounce_time[NUM_BUTTONS] = {0};
static bool last_stable_state[NUM_BUTTONS] = {false}; 
static uint64_t last_button_activity_us = 0;

// Critical changes: Shared state modified inside an ISR must be volatile
static volatile int b_state = 0;
static volatile int konami_seq = 0;

// Persistent structure to hold tracking context for our hardware repeating timer
static struct repeating_timer button_timer;

static void check_konami(uint gpio) {
    switch (konami_seq) {
        case 0: case 1: konami_seq = (gpio == GPIO_JOY_UP) ? konami_seq + 1 : 0; break;
        case 2: case 3: konami_seq = (gpio == GPIO_JOY_DOWN) ? konami_seq + 1 : 0; break;
        case 4: case 6: konami_seq = (gpio == GPIO_JOY_LEFT) ? konami_seq + 1 : 0; break;
        case 5: case 7: konami_seq = (gpio == GPIO_JOY_RIGHT) ? konami_seq + 1 : 0; break;
        case 8:         konami_seq = (gpio == GPIO_BTN1) ? konami_seq + 1 : 0; break;
        case 9:         konami_seq = (gpio == GPIO_BTN2) ? konami_seq + 1 : 0; break;
        default:        konami_seq = 0; break;
    }
}

// Timer ISR Callback wrapper 
// MUST return true to keep the timer repeating
bool buttons_timer_callback(struct repeating_timer *t) {
    uint64_t now = time_us_64();

    for (int i = 0; i < NUM_BUTTONS; i++) {
        bool raw_reading = !gpio_get(BUTTONS[i].gpio); 

        if (raw_reading != last_stable_state[i]) {
            if ((now - last_debounce_time[i]) >= (BUTTON_DEBOUNCE_MS * 1000ULL)) {
                last_stable_state[i] = raw_reading;
                
                if (raw_reading) {
                    b_state |= BUTTONS[i].mask;
                    last_button_activity_us = now;
                    check_konami(BUTTONS[i].gpio);
                }
            }
        } else {
            last_debounce_time[i] = now;
        }
    }
    return true; 
}

void buttons_init() {
    last_button_activity_us = time_us_64();

    for (int i = 0; i < NUM_BUTTONS; i++) {
        gpio_init(BUTTONS[i].gpio);
        gpio_set_dir(BUTTONS[i].gpio, GPIO_IN);
        gpio_pull_up(BUTTONS[i].gpio);
        
        last_stable_state[i] = false; 
        last_debounce_time[i] = 0;
    }

    gpio_init(GPIO_OFFLINE);
    gpio_set_dir(GPIO_OFFLINE, GPIO_IN);
    gpio_pull_up(GPIO_OFFLINE);

    // Fire off the background timer to run every 10ms
    // Negative delay (-10) guarantees precise periodic intervals over absolute start delays
    add_repeating_timer_ms(-10, buttons_timer_callback, NULL, &button_timer);
}

void buttons_clear() {
    b_state = 0;
}

int buttons_get() {
    return b_state;
}

int buttons_get_and_clear() {
    // Basic atomic snapshot pattern for ISR variables
    uint32_t ints = save_and_disable_interrupts();
    int current_state = b_state;
    b_state = 0;
    restore_interrupts(ints);
    return current_state;
}

int buttons_get_raw() {
    int raw_state = 0;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (!gpio_get(BUTTONS[i].gpio)) {
            raw_state |= BUTTONS[i].mask;
        }
    }
    return raw_state;
}

uint64_t buttons_get_last_activity_us() {
    if (konami_seq == 10) {
        konami_seq = 0;
        config_achievement(konami);
    }
    return last_button_activity_us;
}
