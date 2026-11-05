#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <stdint.h>

#define EV_EFFECT_HULL 0
#define EV_EFFECT_FUEL 1
#define EV_EFFECT_CREDITS 2
#define EV_EFFECT_CREW 3
#define EV_EFFECT_CARGO 4
#define EV_EFFECT_REPUTATION 5
#define EV_EFFECT_XP 6
#define EV_EFFECT_UPGRADE 7
#define EV_EFFECT_FLAG_SET 8      // value = flag bit
#define EV_EFFECT_FLAG_CLEAR 9
#define EV_EFFECT_ORE 10          // goods: value = count (negative removes)
#define EV_EFFECT_GAS 11
#define EV_EFFECT_ALLOY 12
#define EV_EFFECT_PARTS 13
#define EV_EFFECT_RELIC 14
#define EV_EFFECT_CHAIN 15        // value = event id forced onto the next empty tile
#define EV_EFFECT_HE3 16
#define EV_EFFECT_DARK 17

#define EV_AT_EMPTY 0
#define EV_AT_POI 1
#define EV_AT_HOME 2
#define EV_AT_DERELICT 3

#define EV_FLAG_REQUIRES_PILOT 1
#define EV_FLAG_REQUIRES_ENGINEER 2
#define EV_FLAG_REQUIRES_SCIENTIST 4
#define EV_FLAG_REQUIRES_SOLDIER 8
#define EV_FLAG_REQUIRES_MEDIC 16
#define EV_FLAG_REQUIRES_CARGO 32
#define EV_FLAG_REQUIRES_CREDITS 64
#define EV_FLAG_REQUIRES_FUEL 128

typedef struct event_effect {
    uint8_t type;
    int16_t value;
} event_effect_t;

typedef struct event_outcome {
    const char* description;
    const char* name;
    uint8_t chance;
    uint8_t effect_count;
    event_effect_t effects[4]; // Up to 4 effects per outcome
} event_outcome_t;

typedef struct event_option {
    const char* description;
    uint8_t requirements;
    uint16_t required_count;
    uint8_t outcome_count;
    event_outcome_t outcomes[4]; // Up to 4 outcomes per event
} event_option_t;

typedef struct event {
    uint8_t id;
    const char* title;
    const char* description;
    uint8_t option_count;
    event_option_t options[4]; // Up to 4 options per event
    // story hooks (all zero for stock events)
    uint32_t req_flags;        // every bit must be set in state.story_flags
    uint32_t forbid_flags;     // none of these bits may be set
    uint8_t once;              // fire at most once per run
    uint8_t at;                // EV_AT_*
    uint8_t weight;            // 0 or 3 = normal, 1 = rare, 9 = common
} event_t;
#endif
