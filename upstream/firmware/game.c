#include "game.h"
#include "framebuffer.h"
#include "buttons.h"
#include <stdio.h>
#include "pico/rand.h"
#include "pico/stdlib.h"
#include "hexutils.h"
#include "img/nebula.h"
#include "img/astroids.h"
#include "img/poi.h"
#include "img/rocket.h"
#include "img/crew.h"
#include "img/icons.h"
#include <string.h>
#include "bsp/config.h"
#include "bsp/common.h"
#include "bsp/base64.h"
#include "bsp/eeprom.h"
#include "bsp/terminal.h"
#include "ui.h"
#include "img/gameui.h"
#include "asteroids.h"
#include "multiplayer.h"
#include "frontier.h"
#include "touch.h"

extern unsigned short get_crew_color(const char c);
static uint32_t map_rng_state = 1;
static uint32_t map_rand(void) {
    uint32_t x = map_rng_state;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    map_rng_state = x;
    return x;
}

const int hex_size = 36;
const int fuel_cost = 25; // fuel cost per unit

game_config_t config = {
    .max_fuel = 15,
    .max_hull = 6,
    .max_cargo = 24,
    .starting_credits = 1000
};
game_state_t state;

extern const event_t random_events[];
extern const unsigned int random_event_count;

int game_running = false;
int crew_count;
game_character_t crew[5];
char game_stats[80];
Hex path[11];
int path_len;
bool offline_mode;
int shuffled_events[100];
int event_index = 0;

Point screen_pos;

const uint16_t sineLookupTable[] = {
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1,
    1, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0};
    

uint16_t get_rand_skin_tone() {
    // Generate a random skin tone in RGB565 format
    uint8_t r = (get_rand_32() % 128) + 64; // 64-191 for better visibility
    uint8_t g = (get_rand_32() % 128) + 64;
    uint8_t b = (get_rand_32() % 128) + 64;
    return RGB565(r, g, b);
}

Hex get_random_hex(int distance) {
    int ring_size = 6 * distance;
    Hex ring[ring_size];
    int index = 0;
    for (int q=-distance; q <= distance; q++) {
        for (int r=-distance; r <= distance; r++) {
            if (abs(q) + abs(r) + abs(-q-r) == 2*distance) {
                ring[index++] = (Hex){q, r};
            }
        }
    }
    index = map_rand() % ring_size;
    return ring[index];
}

enum tiletype get_hex_type(Hex hex) {
    if (hex.q == 0 && hex.r == 0) return TILE_HOME;
    for (int i = 0; i < DERELICT_COUNT; ++i) if (hex_equal(state.derelicts[i], hex)) return TILE_DERELICT;
    if (hex_equal(state.nebula, hex)) return TILE_NEBULA;
    if ((state.nebula.q+1) == hex.q && state.nebula.r == hex.r) return TILE_NEBULA;
    if ((state.nebula.q+1) == hex.q && (state.nebula.r-1) == hex.r) return TILE_NEBULA;
    for (int i = 0; i < 6; i++) {
        if (hex_equal(state.astroids[i], hex)) return TILE_ASTROID;
    }
    for (int i = 0; i < POI_COUNT; i++) {
        if (hex_equal(state.poi[i], hex)) return TILE_POI;
    }
    return TILE_EMPTY;
}

static void yield() {
    mp_tick();
    terminal_update();
    sleep_ms(1);
}

const char* crew_names[5] = {"Astra", "Zara", "Kai", "Luna", "Jax"};

bool has_open_slot() {
    for (int i=0; i<EEPROM_SLOTS; ++i) {
        char check;
        eeprom_readbytes((i+1)*64, &check, 1);
        if (check != '$') {
            return true;
        }
    }
    return false;
}

int queue_mission_results(int score) {
    if (offline_mode) {
        return 0; // Do not queue mission results in offline mode
    }
    char entry[64] = {0};
    char game_id_str[9] = {0};
    int game_id = get_rand_32();
    char* myid=config_get_id();

    base64_encode((uint8_t*)&game_id, sizeof(int), game_id_str, sizeof(game_id_str));
    game_id_str[5] = '\0';
    snprintf(entry, sizeof(entry)-32, "$GAME %5.5s%5.5s %5d %2d ",
    myid, game_id_str, score, state.cargo);
    for (int i=0; i<crew_count; ++i) {
        if (strcmp(crew[i].id, myid)==0)
            continue;
        strncat(entry, crew[i].id, sizeof(entry) - strlen(entry) - 1);
    }
    strncat(entry, " ", sizeof(entry) - strlen(entry) - 1);
    sign_message(entry, sizeof(entry));

    for (int i=0; i<EEPROM_SLOTS; ++i) {
        char check;
        eeprom_readbytes((i+1)*64, &check, 1);
        if (check != '$') {
            //printf("Saving game to slot\r\n", i);
            eeprom_writebytes((i+1)*64, entry, 64);
            return 0;
        }
    }
    return -1; // no room for another entry
}

void game_init() {
    int achievements = config_get_achievement_count();
    config.max_fuel = 8;    // balance: outposts sit 4 to 10 jumps out, 5 was a wall
    config.max_hull = 4;
    config.max_cargo = 24;
    state.seed = mp_active ? (int)mp_seed : (int)get_rand_32();
    map_rng_state = ((uint32_t)state.seed) | 1u;
    Hex t;
    do {
        t= get_random_hex(4);
    } while (get_hex_type(t)!=TILE_EMPTY);
    state.nebula = t;

    for (int i=0; i<random_event_count; ++i) {
        shuffled_events[i] = i;
    }

    for (int i=0; i<random_event_count; ++i) {
        int temp = shuffled_events[i];
        int j = get_rand_32() % random_event_count;
        shuffled_events[i] = shuffled_events[j];
        shuffled_events[j] = temp;
    }
    event_index = 0;

    for (int i = 0; i < 6; i+=2) {
        do {
            t = get_random_hex(i + 3);
        } while (get_hex_type(t)!=TILE_EMPTY);
        state.astroids[i] = t;
        Hex at = state.astroids[i];
        Hex near[6] = {{at.q,at.r-1}, {at.q+1,at.r-1}, {at.q+1,at.r}, {at.q,at.r+1}, {at.q-1,at.r+1}, {at.q-1,at.r}};
        for (int j=0; j<6; ++j) {
            t = near[j];
            int o = map_rand()%6;
            near[j] = near[o];
            near[o] = t;
        }
        for (int j=0; j<6; ++j) {
            if (get_hex_type(near[j])==TILE_EMPTY) {
                state.astroids[i+1] = near[j];
                break;
            }
        }
    }
    for (int i = 0; i < POI_COUNT; i++) {
        int dist = 10 - i*4/5;
        do {
            t = get_random_hex(dist);
        } while (get_hex_type(t)!=TILE_EMPTY);
        state.poi[i] = t;
        state.poi_market_rate[i] = 550 + (map_rand() % 50) - dist * 50;
    } 
    memset(state.hold, 0, sizeof(state.hold));
    memset(state.events_used, 0, sizeof(state.events_used));
    memset(state.contracts, 0, sizeof(state.contracts));
    state.chain_event = 0;
    state.jobs_taken = 0;
    state.shipyard_mask = 0;
    state.modules = 0;
    state.nebula_harvested = 0;
    while (__builtin_popcount(state.shipyard_mask) < 3) state.shipyard_mask |= 1u << (map_rand() % POI_COUNT);
    state.depot_mask = 0;
    while (__builtin_popcount(state.depot_mask) < 2) {
        int p = map_rand() % POI_COUNT;
        if (!((state.shipyard_mask >> p) & 1)) state.depot_mask |= 1u << p;
    }
    state.jobs_done = 0;
    memset(state.job_gen, 0, sizeof(state.job_gen));
    state.story_flags = 0;
    state.derelict_looted = 0;
    for (int i = 0; i < DERELICT_COUNT; ++i) {
        state.derelicts[i].q = 100; state.derelicts[i].r = 100;
        do {
            t = get_random_hex(5 + i * 2);
        } while (get_hex_type(t) != TILE_EMPTY);
        state.derelicts[i] = t;
    }
    state.player.q = 0;
    state.player.r = 0;
    state.heading = 0;
    state.hull = config.max_hull;
    state.fuel = config.max_fuel;
    config.starting_credits = 3000 + 50 * config_get_achievement_count();
    state.credits = config.starting_credits;
    state.cargo = 0;
    state.reputation = 0;
    state.xp = 0;
    Point at = hex_to_pixel(state.player, hex_size);
    screen_pos.x = LCD_WIDTH/2 - at.x;
    screen_pos.y = LCD_HEIGHT/2 - at.y;

    if (offline_mode) {
        //printf("Offline mode: generating crew.\n");
        for (int i = 0; i < 5; i++) {
            char tid[9];
            uint32_t tind = get_rand_32();
            base64_encode((uint8_t*)&tind, sizeof(tind), tid, sizeof(tid));
            tid[5] = '\0';
            crew[i].level = 1;
            crew[i].specialty = 'A' + get_rand_32() % 5; // 0 = Pilot, 1 = Engineer, 2 = Scientist, 3 = Soldier
            memcpy(crew[i].id, tid, 5);
            crew[i].id[5] = '\0';
            snprintf(crew[i].name, sizeof(crew[i].name), "%s", crew_names[i]);
        }
        crew_count = 5;
    } else if (crew_count == 0) {
        crew[0].level = 1;
        crew[0].specialty = config_get_class();
        memcpy(crew[0].id, config_get_id(), 5);
        crew[0].id[5] = '\0';
        snprintf(crew[0].name, sizeof(crew[0].name), "%s", config_get_handle());
        crew_count = 1;
    }
}

void game_draw_character(int x, int y, const game_character_t* character) {
    uint16_t color = character->skin_tone;
    frame_draw_image(x, y, player_img_data[character->specialty - 'A'], 15, 16); 
}

void game_draw_ui() {
    const uint16_t hull_color = RGB565(167,36,18);
    const uint16_t fuel_color = RGB565(57,141,141);
    
    frame_draw_image(1, LCD_HEIGHT/2 - 31, fuel_image_data, 14, 13);
    frame_fill_rect(0, LCD_HEIGHT/2 - 18, 12, 4*config.max_fuel, background_color); // Fuel background
    for (int i=0; i<state.fuel; ++i) {
        frame_fill_rect(1, LCD_HEIGHT/2 -16 + i*4, 10, 3, fuel_color);
    }
    frame_draw_line(1, LCD_HEIGHT/2 - 18, 10, LCD_HEIGHT/2 - 18, border_color); // Top border
    frame_draw_line(1, LCD_HEIGHT/2 - 17 + 4*config.max_fuel, 10, LCD_HEIGHT/2 - 17 + 4*config.max_fuel, border_color); // Bottom
    frame_draw_line(0, LCD_HEIGHT/2 - 17, 0, LCD_HEIGHT/2 - 18 + 4*config.max_fuel, border_color); // Left
    frame_draw_line(11, LCD_HEIGHT/2 - 17, 11, LCD_HEIGHT/2 - 18 + 4*config.max_fuel, border_color); // Right
    
    frame_draw_image(LCD_WIDTH - 13, LCD_HEIGHT/2 - 31, hull_image_data, 12, 13);
    frame_fill_rect(LCD_WIDTH - 13, LCD_HEIGHT/2 - 18, 12, 1+8*config.max_hull, background_color); // Hull background
    for (int i=0; i<state.hull; ++i) {
        frame_fill_rect(LCD_WIDTH - 12, LCD_HEIGHT/2 -16 + i*8, 10, 6, hull_color);
    }
    frame_draw_line(LCD_WIDTH - 13, LCD_HEIGHT/2 - 18, LCD_WIDTH - 2, LCD_HEIGHT/2 - 18, border_color); // Top border
    frame_draw_line(LCD_WIDTH - 13, LCD_HEIGHT/2 - 17 + 8*config.max_hull, LCD_WIDTH - 2, LCD_HEIGHT/2 - 17 + 8*config.max_hull, border_color); // Bottom
    frame_draw_line(LCD_WIDTH - 14, LCD_HEIGHT/2 - 17, LCD_WIDTH - 14, LCD_HEIGHT/2 - 18 + 8*config.max_hull, border_color); // Left
    frame_draw_line(LCD_WIDTH - 1, LCD_HEIGHT/2 - 17, LCD_WIDTH - 1, LCD_HEIGHT/2 - 18 + 8*config.max_hull, border_color); // Right

    frame_fill_rect(LCD_WIDTH/2 - 54, LCD_HEIGHT - 42, 108, 41, background_color); 
    frame_draw_line(LCD_WIDTH/2 - 55, LCD_HEIGHT - 43, LCD_WIDTH/2 + 55, LCD_HEIGHT - 43, border_color); // Top border
    frame_draw_line(LCD_WIDTH/2 - 54, LCD_HEIGHT - 44, LCD_WIDTH/2 + 54, LCD_HEIGHT - 44, border_color); // Top border 2
    frame_draw_line(LCD_WIDTH/2 - 54, LCD_HEIGHT - 21, LCD_WIDTH/2 + 54, LCD_HEIGHT - 21, border_color); // Bottom
    frame_draw_line(LCD_WIDTH/2 - 55, LCD_HEIGHT - 43, LCD_WIDTH/2 - 55, LCD_HEIGHT    , border_color); // Left
    frame_draw_line(LCD_WIDTH/2 - 56, LCD_HEIGHT - 42, LCD_WIDTH/2 - 56, LCD_HEIGHT - 1, border_color); // Left 2
    frame_draw_line(LCD_WIDTH/2 + 55, LCD_HEIGHT - 43, LCD_WIDTH/2 + 55, LCD_HEIGHT    , border_color); // Right
    frame_draw_line(LCD_WIDTH/2 + 56, LCD_HEIGHT - 42, LCD_WIDTH/2 + 56, LCD_HEIGHT - 1, border_color); // Right 2
    frame_draw_line(LCD_WIDTH/2 - 5, LCD_HEIGHT - 41, LCD_WIDTH/2 -5, LCD_HEIGHT - 20, border_color); // Divider between cargo and credits
    frame_draw_pixel(LCD_WIDTH/2 - 54, LCD_HEIGHT - 42, border_color);
    frame_draw_pixel(LCD_WIDTH/2 + 54, LCD_HEIGHT - 42, border_color);
    
    for (int i=0; i<crew_count; ++i) {
        game_draw_character(LCD_WIDTH/2-48 + i*20 + 10*(5-crew_count), LCD_HEIGHT-19, &crew[i]);
    }
    
    {
        int i = 0;
        for (int g = 0; g < GOOD_COUNT; ++g) {
            int pitch = config.max_cargo > 32 ? 4 : (config.max_cargo > 24 ? 5 : 6);
            for (int k = 0; k < state.hold[g] && i < 40; ++k, ++i) {
                frame_fill_rect(LCD_WIDTH /2 - 11 - 6*(i%8), LCD_HEIGHT - 26 - pitch*(i/8), 4, 4, good_colors[g]);
            }
        }
    }

    char credits_str[6];
    snprintf(credits_str, sizeof(credits_str), "$%d", state.credits);
    int dw = frame_calculate_text_width(credits_str);
    frame_draw_string(LCD_WIDTH/2 +25 - dw/2, LCD_HEIGHT - 26, credits_str, RGB565(244, 173, 44)); 
}

void game_show_message(const char* title, const char* message) {
    frame_clear();
    game_draw_ui();
    frame_draw_text(title, title_color, 30); // Yellow color
    frame_draw_text(message, description_color, LCD_HEIGHT/4); // White color
    frame_refresh();
}

bool game_is_option_available(const event_option_t* option) {
    // Check crew requirements
    if (option->requirements & EV_FLAG_REQUIRES_PILOT) {
        for (int i=0; i<5; ++i) {
            if (crew[i].specialty == CREW_SPECIALTY_PILOT && crew[i].level >= option->required_count) {
                return true;
            }
        }
        return false;
    }
    if (option->requirements & EV_FLAG_REQUIRES_ENGINEER) {
        for (int i=0; i<5; ++i) {
            if (crew[i].specialty == CREW_SPECIALTY_ENGINEER && crew[i].level >= option->required_count) {
                return true;
            }
        }
        return false;
    }
    if (option->requirements & EV_FLAG_REQUIRES_SCIENTIST) {
        for (int i=0; i<5; ++i) {
            if (crew[i].specialty == CREW_SPECIALTY_SCIENTIST && crew[i].level >= option->required_count) {
                return true;
            }
        }
        return false;
    }
    if (option->requirements & EV_FLAG_REQUIRES_SOLDIER) {
        for (int i=0; i<5; ++i) {
            if (crew[i].specialty == CREW_SPECIALTY_SOLDIER && crew[i].level >= option->required_count) {
                return true;
            }
        }
        return false;
    }
    if (option->requirements & EV_FLAG_REQUIRES_MEDIC) {
        for (int i=0; i<5; ++i) {
            if (crew[i].specialty == CREW_SPECIALTY_MEDIC && crew[i].level >= option->required_count) {
                return true;
            }
        }
        return false;
    }

    // Check resource requirements
    if (option->requirements & EV_FLAG_REQUIRES_CARGO) {
        if (state.cargo < option->required_count) return false;
    }
    if (option->requirements & EV_FLAG_REQUIRES_CREDITS) {
        if (state.credits < option->required_count) return false;
    }
    if (option->requirements & EV_FLAG_REQUIRES_FUEL) {
        if (state.fuel < option->required_count) return false;
    }

    return true;
}

static int event_opt_top[4], event_opt_bottom[4];

void game_draw_event(const event_t* event, int selected_index) {
    frame_clear();
    int y_offset = 30;
    y_offset += frame_draw_text(event->title, title_color, y_offset); // Yellow color
    y_offset += frame_draw_text(event->description, description_color, y_offset); // White color
    y_offset = 230; // Space between description and options
    for (int i = 0; i < event->option_count; i++) {
        bool special_option = event->options[i].requirements > 0 && event->options[i].requirements < EV_FLAG_REQUIRES_CARGO;
        int color = special_option ? special_option_color :option_color;
        if (!game_is_option_available(&event->options[i])) {
            color = unavailable_color; // Dim unavailable options
        }
        if (i == selected_index) {
            int w = frame_calculate_text_width(event->options[i].description);
            absolute_time_t current_time = get_absolute_time();
            uint32_t milliseconds_since_boot = to_ms_since_boot(current_time);
            int sel_offset = sineLookupTable[(milliseconds_since_boot>>5)%32];
            frame_draw_image(LCD_WIDTH/2 - w/2 - 25 - sel_offset, y_offset - 14, sel1_image_data, 16, 16);
            frame_draw_image(LCD_WIDTH/2 + w/2 + 9 + sel_offset, y_offset - 14, sel2_image_data, 16, 16); 
        }
        event_opt_top[i] = y_offset - 16;
        y_offset += frame_draw_text(event->options[i].description, color, y_offset) + 2; // Space between options
        event_opt_bottom[i] = y_offset - 14;
    }
    game_draw_ui();
    frame_refresh();
}

void game_show_outcome(const char* message, uint8_t effects_count, const event_effect_t* effects) {
    frame_clear();
    int y_offset = 60 + frame_draw_text(message, description_color, 60); // White color
    y_offset = 220; // Space between description and effects
    for (int i = 0; i < effects_count; i++) {
        char effect_str[50];
        const char* effect_type;
        switch (effects[i].type) {
            case EV_EFFECT_HULL: effect_type = "Hull"; break;
            case EV_EFFECT_FUEL: effect_type = "Fuel"; break;
            case EV_EFFECT_CREDITS: effect_type = "Credits"; break;
            case EV_EFFECT_CREW: effect_type = "Crew"; break;
            case EV_EFFECT_CARGO: effect_type = "Cargo"; break;
            case EV_EFFECT_REPUTATION: effect_type = "Reputation"; break;
            case EV_EFFECT_XP: effect_type = "XP"; break;
            case EV_EFFECT_UPGRADE: effect_type = "Unlock"; break;
            case EV_EFFECT_ORE: effect_type = "Ore"; break;
            case EV_EFFECT_GAS: effect_type = "Gas"; break;
            case EV_EFFECT_ALLOY: effect_type = "Alloy"; break;
            case EV_EFFECT_PARTS: effect_type = "Parts"; break;
            case EV_EFFECT_RELIC: effect_type = "Relic"; break;
            case EV_EFFECT_HE3: effect_type = "He-3"; break;
            case EV_EFFECT_DARK: effect_type = "Dark matter"; break;
            case EV_EFFECT_FLAG_SET: case EV_EFFECT_FLAG_CLEAR: case EV_EFFECT_CHAIN: effect_type = NULL; break;
            default: effect_type = "Unknown"; break;
        }
        int color = effects[i].value > 0 ? RGB565(0, 255, 0) : RGB565(255, 0, 0); // Green for positive, Red for negative
        if (effect_type == NULL) continue;   // story bookkeeping, nothing to show
        if (effects[i].type == EV_EFFECT_CREW) {
            //Handle crew changes with more detail here
            if (effects[i].value < 0) {
                    snprintf(effect_str, sizeof(effect_str), "Crew lost: ");
                    for (int j=0; j>effects[i].value; --j) {
                        if (crew_count == 0) break; //No one left
                        int idx = get_rand_32() % crew_count;
                        crew_count--;
                        crew[idx] = crew[crew_count]; // Remove crew member by replacing with last
                        if (j < 0) {
                            strncat(effect_str, ", ", sizeof(effect_str) - strlen(effect_str) - 1);
                        }
                        strncat(effect_str, crew[idx].name, sizeof(effect_str) - strlen(effect_str) - 1);
                    }       
            }
            else {
                if (crew_count < 5) {
                    crew[crew_count].specialty = 'A' + get_rand_32() % 5; // Random specialty
                    crew[crew_count].level = 3; // Starting level for new crew
                    memcpy(crew[crew_count].id, "NPCID", 5);
                    crew[crew_count].id[5] = '\0';
                    snprintf(crew[crew_count].name, sizeof(crew[crew_count].name), "%s", crew_names[crew_count]);
                    snprintf(crew[crew_count].name, sizeof(crew[crew_count].name), "%s joins your crew", crew[crew_count].name); 
                    crew_count++;
                } else {
                    strncat(effect_str, "Crew full, cannot add more", sizeof(effect_str) - strlen(effect_str) - 1);
                    color = RGB565(255, 100, 0); // Orange color for failed crew addition
                }
            }
        }
        else if (effects[i].type == EV_EFFECT_UPGRADE) {
            snprintf(effect_str, sizeof(effect_str), "Upgrade unlocked!");
            color = RGB565(0, 255, 255); // Cyan color for upgrades
        }
        else {
            snprintf(effect_str, sizeof(effect_str), "%s: %d", effect_type, effects[i].value);
        }
        frame_draw_string_centered(y_offset + i * 30, effect_str, color); // Red color for effects 
   }   
    frame_refresh();
}


void game_run_event(const event_t* event) {

    //First, show the event description
    buttons_clear();

    int selected_option = -1;
    do {
        selected_option = (selected_option + 1) % event->option_count;
    } while (!game_is_option_available(&event->options[selected_option]));
    while (1) {
        game_draw_event(event, selected_option);
        uint32_t keys = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) {
                for (int i = 0; i < event->option_count; ++i) {
                    if (ty >= event_opt_top[i] && ty < event_opt_bottom[i] && game_is_option_available(&event->options[i])) {
                        selected_option = i;
                        keys |= BUTTON_A;
                    }
                }
            }
        }

        if (keys & BUTTON_UP) {
            do {
                selected_option = (selected_option - 1 + event->option_count) % event->option_count;
            } while (!game_is_option_available(&event->options[selected_option]));
        }
        if (keys & BUTTON_DOWN) {
            do {
                selected_option = (selected_option + 1) % event->option_count;
            } while (!game_is_option_available(&event->options[selected_option]));
        }
        if (keys & (BUTTON_A | BUTTON_PRESS)) {
            break; 
        }
        yield();
    }

    int rand_range = 0;
    for (int i = 0; i < event->options[selected_option].outcome_count; i++) {
        rand_range += event->options[selected_option].outcomes[i].chance;
    }
    const event_outcome_t* selected_outcome;
    if (rand_range == 0) {
        selected_outcome = &event->options[selected_option].outcomes[0];
    } else {
        int rand_value = get_rand_32() % rand_range;
        int cumulative_chance = 0;
        for (int i = 0; i < event->options[selected_option].outcome_count; i++) {
            cumulative_chance += event->options[selected_option].outcomes[i].chance;
            if (rand_value < cumulative_chance) {
                selected_outcome = &event->options[selected_option].outcomes[i];
                break;
            }
        }
    }

    // Show outcome description
    game_show_outcome(selected_outcome->description, selected_outcome->effect_count, selected_outcome->effects);
    for (int i = 0; i < selected_outcome->effect_count; i++) {
        switch (selected_outcome->effects[i].type) {
            case EV_EFFECT_HULL: 
                state.hull += selected_outcome->effects[i].value; 
                if (state.hull > config.max_hull) state.hull = config.max_hull;
                break;
            case EV_EFFECT_FUEL: 
                state.fuel += selected_outcome->effects[i].value;
                if (state.fuel > config.max_fuel) state.fuel = config.max_fuel;
                break;
            case EV_EFFECT_CREDITS: 
                state.credits += selected_outcome->effects[i].value; 
                break;
            case EV_EFFECT_CREW: {
                // Handled in game_show_outcome for more detailed crew changes, so we can skip applying it here
            } break;
            case EV_EFFECT_CARGO:
                if (selected_outcome->effects[i].value > 0) frontier_add_good(GOOD_SUPPLIES, selected_outcome->effects[i].value);
                else frontier_take_any(-selected_outcome->effects[i].value);
                break;
            case EV_EFFECT_REPUTATION: state.reputation += selected_outcome->effects[i].value; break;
            case EV_EFFECT_XP: state.xp += selected_outcome->effects[i].value; break;
            case EV_EFFECT_UPGRADE: config_achievement(selected_outcome->effects[i].value + game_event1); break;
            default: frontier_apply_story_effect(selected_outcome->effects[i].type, selected_outcome->effects[i].value); break;
        }
        if (state.cargo < 0) state.cargo = 0;
        if (state.fuel < 0) state.fuel = 0;
        if (state.hull < 0) state.hull = 0;
    }
    //wait for player to press a button to continue
    buttons_clear();
    while (buttons_get_and_clear() == 0) {
        yield();
    }

}

void game_draw_ghosts(int x, int y) {
    if (!mp_active) return;
    const mp_peer_t* p = mp_peers();
    for (int i = 0; i < MP_MAX_PEERS; ++i) {
        if (!p[i].active || p[i].done) continue;
        Point at = hex_to_pixel((Hex){p[i].q, p[i].r}, hex_size);
        uint16_t col = get_crew_color(p[i].cls);
        frame_draw_hexagon(x+at.x, y+at.y, hex_size-6, col);
        frame_draw_image(x+at.x-16, y+at.y-16, rocket_image_data[p[i].heading % 6], 32, 32);
        int w = frame_calculate_text_width(p[i].name);
        frame_draw_string(x+at.x - w/2, y+at.y+18, p[i].name, col);
    }
}

void game_draw_map(int x, int y, int xoff, int yoff) {
    pseudo_random_init(state.seed);
    for (int i = 0; i < 2000; i++) {
        int xt = psuedo_random() % 700;
        int yt = psuedo_random() % 700;
        int brightness = abs(psuedo_random() % 256);
        frame_draw_pixel(x+xt, y+yt, RGB565(brightness, brightness, brightness)); // White star
    }

    frame_draw_hexagon_grid(x, y, hex_size, RGB565(30,30,30));
    Point at = hex_to_pixel(state.nebula, hex_size);
    frame_draw_image(x+at.x-32, y+at.y-62, nebula_image_data, 128, 128);
    for (int i=0; i<6; ++i) {
        at = hex_to_pixel(state.astroids[i], hex_size);
        frame_draw_image(x+at.x-33, y+at.y-31, astroid_image_data[i], 66, 63);
    }
    frontier_draw_derelicts(x, y);

    if ((xoff == 0) && (yoff == 0)) {
        Hex hex = pixel_to_hex(LCD_WIDTH/2-x, LCD_HEIGHT/2-y, hex_size);
        Point hex_pixel = hex_to_pixel(hex, hex_size);
        int distance = hex_distance(hex, state.player);
        path_len = hex_path(state.player, hex, path, 11);
        Point p0 = hex_to_pixel(path[path_len-1], hex_size);
        uint16_t color = distance <= state.fuel ? RGB565(0, 200, 0) : RGB565(200, 0, 0);
        int dx = time_us_64() / 50000;
        for (int i=path_len-2; i>=0; --i) {
            Point p = hex_to_pixel(path[i], hex_size);
            dx = frame_draw_dashed_line(x+p0.x, y+p0.y, x+p.x, y+p.y, color, dx);
            p0=p;
        }
        if (distance == 0){
            frame_draw_hexagon(x+hex_pixel.x, y+hex_pixel.y, hex_size, RGB565(200, 200, 200));
        }
        else if (distance > 0) {
            frame_draw_hexagon(x+hex_pixel.x, y+hex_pixel.y, hex_size, color);
        }
    }

    absolute_time_t current_time = get_absolute_time();
    uint32_t milliseconds_since_boot = to_ms_since_boot(current_time);
    int poi_offset = sineLookupTable[(milliseconds_since_boot>>6)%32];
    for (int i = 0; i < POI_COUNT; i++) {
        at = hex_to_pixel(state.poi[i], hex_size);
        frame_draw_image(x+at.x-16, y+at.y-16+poi_offset, poi_image_data[i], 32, 32);
    }
    frontier_draw_poi_marks(x, y);
    at = hex_to_pixel((Hex){0,0}, hex_size);
    frame_draw_image(x+at.x-31, y+at.y-28, station_image_data, 62, 48); // Space station at center

    game_draw_ghosts(x, y);
    at = hex_to_pixel(state.player, hex_size);
    frame_draw_image(x+at.x-16+xoff, y+at.y-16+yoff, rocket_image_data[state.heading % 6], 32, 32); // Player ship
    game_draw_ui();
}

int game_run_map() {
    while (true) {
        frame_clear();
        game_draw_map(screen_pos.x, screen_pos.y, 0, 0);
        frame_refresh();
        int buttons = buttons_get_raw();
        if (buttons & BUTTON_UP) {
            screen_pos.y += 4;
        }
        if (buttons & BUTTON_DOWN) {
            screen_pos.y -= 4;
        }
        if (buttons & BUTTON_LEFT) {
            screen_pos.x += 4;
        }
        if (buttons & BUTTON_RIGHT) {
            screen_pos.x -= 4;
        }
        {
            int ddx, ddy;
            if (ui_take_drag(&ddx, &ddy)) { screen_pos.x += ddx; screen_pos.y += ddy; }
        }
        if (screen_pos.x < -400) screen_pos.x = -400;
        if (screen_pos.x > 750) screen_pos.x = 750;
        if (screen_pos.y < -440) screen_pos.y = -440;
        if (screen_pos.y > 820) screen_pos.y = 820;
        buttons = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) {
                if (ty >= LCD_HEIGHT - 44 && tx > LCD_WIDTH/2 - 56 && tx < LCD_WIDTH/2 + 56) {
                    buttons |= BUTTON_PRESS;             // HUD tap: ship menu
                } else {
                    Hex th = pixel_to_hex(tx - screen_pos.x, ty - screen_pos.y, hex_size);
                    path_len = hex_path(state.player, th, path, 11);
                    buttons |= BUTTON_A;                 // fly one step toward the tapped hex, or re-enter
                }
            }
        }
        if (buttons & BUTTON_A) {
            int mframes=30;
            //int distance = hex_distance(hex, state.player);
            if (path_len == 1) {
                Hex hex = path[0];
                enum tiletype here = get_hex_type(hex);
                if (here == TILE_POI || here == TILE_DERELICT || here == TILE_HOME) {
                    return 0; // Enter the outpost or board the wreck again
                }
            }
            if (path_len > 1) {
                Hex hex = path[1];
                Point hex_pixel = hex_to_pixel(hex, hex_size);
                state.heading = hex_get_heading(state.player, hex);
                Point player_at = hex_to_pixel(state.player, hex_size);
                int dx = (hex_pixel.x - player_at.x);
                int dy = (hex_pixel.y - player_at.y);
                for (int i=1; i<=mframes; ++i) {
                    frame_clear();
                    game_draw_map(screen_pos.x, screen_pos.y, dx*i/mframes, dy*i/mframes);
                    frame_refresh();
                }
                state.player = hex;
                state.fuel--; // Consume fuel for movement
                return 0;
            }
        }
        if (buttons & BUTTON_PRESS) {
            frontier_run_ship_menu();
        }
        if (buttons & BUTTON_B) {
            sleep_ms(50);
            ui_draw_popup("Press A to abandon game.");
            
            while ((buttons = buttons_get_and_clear()) == 0) {
                yield();
            };
            if (buttons & BUTTON_A) {
                game_running = false;
                return -1;
            }
        }
        yield();
    }
}

void game_draw_market(const char* poi_name, int market_rate, int selected_option) {
    char option_line[50];
    uint16_t color; 

    frame_clear();
    frame_draw_image(120, 29, wingl_image_data, 20, 12);
    frame_draw_image(220, 29, wingr_image_data, 20, 12);
    frame_draw_image(146, 30, outpost_image_data, 68, 10);
    frame_draw_text(poi_name, title_color, 64); 
    frame_draw_rounded_rect(80, 80, 200, 68, border_color);
    frame_draw_image(92, 99, market_image_data, 37, 35);
    frame_draw_image(134, 86, marketcond_image_data, 128, 8);

    const char* market_con[10] = {"Fire Sale", "Surplus", "Glut", "Soft", "Balanced", "Active", "Tight", "Scarcity", "Desperate", "Crisis"};
    const uint16_t market_color[10] = {
        RGB565(55, 100, 255),
        RGB565(35, 150, 230),
        RGB565(30, 200, 190),
        RGB565(45, 220, 145),
        RGB565(90, 220, 100),
        RGB565(255, 205, 45),
        RGB565(255, 165, 30),
        RGB565(255, 120, 30),
        RGB565(255, 75, 35),
        RGB565(255, 45, 45)
    };
    int rate_idx = market_rate / 50;
    if (rate_idx > 9) rate_idx = 9;
    frame_draw_string(160, 120, market_con[rate_idx], market_color[rate_idx]);
    frame_draw_string_centered(301, "C: GOODS AND JOBS", description_color);
    

    int sx=0, sy=0;
    if (selected_option == 0) {
        sx = 32, sy = 164;
    } else if (selected_option == 1) {
        sx = 188, sy = 164;
    } else if (selected_option == 2) {
        sx = 32, sy = 230;
    } else if (selected_option == 3) {
        sx = 188, sy = 230;
    }
    frame_fill_rect(sx+1, sy+2, 138, 48, sbackground_color);

    frame_draw_rounded_rect(32, 164, 140, 50, border_color);
    frame_draw_rounded_rect(188, 164, 140, 50, border_color);
    frame_draw_rounded_rect(32, 230, 140, 50, border_color);
    frame_draw_rounded_rect(188, 230, 140, 50, border_color);

    frame_draw_image(40, 171, cargo_image_data, 27, 34);
    frame_draw_image(196, 171, cargo_image_data, 27, 34);
    frame_draw_image(40, 239, fuelb_image_data, 24, 32);
    frame_draw_image(196, 239, exit_image_data, 27, 32);

    frame_draw_string(260, 250, "EXIT", option_color);
    frame_draw_string(240, 270, "OUTPOST", option_color);
    
    snprintf(option_line, sizeof(option_line), "$%d Ea.", market_rate); 
    color = (state.cargo < config.max_cargo && state.credits >= market_rate) ? option_color : unavailable_color;
    frame_draw_string(70, 184, "BUY CARGO", color);
    frame_draw_string(80, 204, option_line, color);
    
    snprintf(option_line, sizeof(option_line), "$%d Ea", (int)(market_rate * 0.75)); 
    color = (state.cargo > 0) ? option_color : unavailable_color;
    frame_draw_string(226, 184, "SELL CARGO", color);
    frame_draw_string(236, 204, option_line, color);
    
    snprintf(option_line, sizeof(option_line), "$%d", fuel_cost * (config.max_fuel - state.fuel)); 
    color = (state.fuel < config.max_fuel && state.credits >= fuel_cost) ? option_color : unavailable_color;
    frame_draw_string(80, 250, "BUY FUEL", color);
    frame_draw_string(100, 270, option_line, color);
    
    game_draw_ui();
    frame_refresh();
}

void game_run_market() {

    buttons_clear();

    int selected_option = 0;
    int poi_idx = -1;
    for (int i=0; i<10; ++i) {
        if (hex_equal(state.poi[i], state.player)) {
            poi_idx = i;
            break;
        }
    }
    while (true)
    {
        int market_rate = state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0]; // Get market rate for current POI, or default if not found
        while (1) {
            game_draw_market(poi_names[poi_idx >= 0 ? poi_idx : 0], market_rate, selected_option);
            uint32_t keys = buttons_get_and_clear();
            {
                int tx, ty;
                if (ui_take_tap(&tx, &ty)) {
                    if (ui_tap_in(tx, ty, 32, 164, 140, 50)) { selected_option = 0; keys |= BUTTON_A; }
                    else if (ui_tap_in(tx, ty, 188, 164, 140, 50)) { selected_option = 1; keys |= BUTTON_A; }
                    else if (ui_tap_in(tx, ty, 32, 230, 140, 50)) { selected_option = 2; keys |= BUTTON_A; }
                    else if (ui_tap_in(tx, ty, 188, 230, 140, 50)) { selected_option = 3; keys |= BUTTON_A; }
                    else keys |= BUTTON_PRESS;           // anywhere else: goods and jobs
                }
            }

            if (keys & BUTTON_UP) {
                if (selected_option > 1) selected_option -= 2;
            }
            else if (keys & BUTTON_DOWN) {
                if (selected_option < 2) selected_option += 2;
            }
            else if (keys & BUTTON_LEFT) {
                if (selected_option % 2) selected_option--;
            }
            else if (keys & BUTTON_RIGHT) {
                if ((selected_option % 2)==0) selected_option++;
            }
            if (keys & BUTTON_PRESS) {
                frontier_run_services(poi_idx >= 0 ? poi_idx : 0);
            }
            if (keys & BUTTON_A) {
                break; // Option selected
            }
            if (keys & BUTTON_B) {
                selected_option = 3; // Force select "Leave Market" option
                break;
            }
            yield();
        }
        if (selected_option == 0) {
            // Handle buying supplies
            int buy_amount = 1;
            if ((state.cargo < config.max_cargo) && (state.credits >= market_rate)) {
                frontier_add_good(GOOD_SUPPLIES, buy_amount);
                state.credits -= buy_amount * market_rate; // Example pricing formula
                state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0] += 1 + state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0] * 0.15; // Market rate increases by 15% after buying
                if (state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0] < 1) {
                    state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0] = 1; // Minimum market rate
                }
            }
            
        } else if (selected_option == 1) {
            // Handle selling cargo
            if (state.hold[GOOD_SUPPLIES] > 0) {
                int sell_amount = frontier_take_good(GOOD_SUPPLIES, 1); // Sell 1 crate at a time
                state.credits += sell_amount * market_rate * .75; // Example pricing formula with 25% loss on selling
                state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0] -= state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0] * 0.10; // Market rate decreases by 10% after selling
                if (state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0] < 1) {
                    state.poi_market_rate[poi_idx >= 0 ? poi_idx : 0] = 1; // Minimum market rate
                }
            }
        } else if (selected_option == 2) {
            // Handle refueling
            int refuel_amount = config.max_fuel - state.fuel; // Refuel up to max fuel of 10
            if (refuel_amount > 0) {
                if (state.credits < refuel_amount * fuel_cost) {
                    refuel_amount = state.credits / fuel_cost; // Can't refuel more than you can afford
                } 
                state.fuel += refuel_amount;
                state.credits -= refuel_amount * fuel_cost; // Example pricing formula
            }
        } else if (selected_option == 3) {
            break; // Leave market
        }
        yield();
    }
}

int hull_upgrade_cost() {
    return 30 * (config.max_hull + 1); // Cost increases with each hull upgrade
}

int fuel_upgrade_cost() {
    return 20 * (config.max_fuel + 1); // Cost increases with each fuel upgrade
}

int cargo_upgrade_cost() {
    return 100; // Flat cost for cargo upgrade
}

void game_draw_home_market(int selected_option) {
    char option_line[50];
    uint16_t color; 

    frame_clear();
    frame_draw_image(118, 29, wingl_image_data, 20, 12);
    frame_draw_image(222, 29, wingr_image_data, 20, 12);
    frame_draw_image(144, 30, shipyard_image_data, 73, 10);
    frame_draw_rounded_rect(56, 70, 248, 110, border_color);
    frame_draw_image(64, 74, hometxt_image_data, 232, 101);

    int sx=0, sy=0;
    if (selected_option == 0) {
        sx = 32, sy = 200;
    } else if (selected_option == 1) {
        sx = 188, sy = 200;
    } else if (selected_option == 2) {
        sx = 110, sy = 260;
    } 
    frame_fill_rect(sx+1, sy+2, 138, 48, sbackground_color);

    frame_draw_rounded_rect(32, 200, 140, 50, border_color);
    frame_draw_rounded_rect(188, 200, 140, 50, border_color);
    frame_draw_rounded_rect(110, 260, 140, 50, border_color);
    
    frame_draw_image(40, 210, fuelt_image_data, 25, 30);
    frame_draw_image(194, 212, hullb_image_data, 26, 26);
    frame_draw_image(126, 269, exit_image_data, 27, 32);

    frame_draw_string(170, 280, "START", option_color);
    frame_draw_string(160, 300, "MISSION", option_color);
    frame_draw_string_centered(60, "C: STATION JOBS", description_color);
    
    snprintf(option_line, sizeof(option_line), "$%d", fuel_upgrade_cost()); 
    color = (config.max_fuel < 15 && state.credits >= fuel_upgrade_cost()) ? option_color : unavailable_color;
    frame_draw_string(75, 220, "FUEL TANK", color);
    frame_draw_string(100, 240, option_line, color);
    
    snprintf(option_line, sizeof(option_line), "$%d", hull_upgrade_cost()); 
    color = (config.max_hull < 8 && state.credits >= hull_upgrade_cost()) ? option_color : unavailable_color;
    frame_draw_string(220, 220, "HULL ARMOR", color);
    frame_draw_string(256, 240, option_line, color);
    
    game_draw_ui();
    frame_refresh();

    const char* instuctions = "Your account has been credited with $%d. Outfit your ship as you see fit, but remember: every captain is expected to deliver supplies to the Frontier Stations. Wise planning today keeps tomorrow's pioneers moving.";
    char instructions_buffer[350];
    snprintf(instructions_buffer, sizeof(instructions_buffer), instuctions, config.starting_credits);
}

void game_run_home_market() {
    buttons_clear();

    int selected_option = 0;
    
    while (true)
    {
        while (1) {
            game_draw_home_market(selected_option);
            uint32_t keys = buttons_get_and_clear();
            {
                int tx, ty;
                if (ui_take_tap(&tx, &ty)) {
                    if (ui_tap_in(tx, ty, 32, 200, 140, 50)) { selected_option = 0; keys |= BUTTON_A; }
                    else if (ui_tap_in(tx, ty, 188, 200, 140, 50)) { selected_option = 1; keys |= BUTTON_A; }
                    else if (ui_tap_in(tx, ty, 110, 260, 140, 50)) { selected_option = 2; keys |= BUTTON_A; }
                    else keys |= BUTTON_PRESS;           // text box or elsewhere: station services
                }
            }

            if (keys & BUTTON_UP) {
                selected_option = 0;
            }
            else if (keys & BUTTON_DOWN) {
                selected_option = 2;
            }
            else if (keys & BUTTON_LEFT) {
                selected_option = 0;
            }
            else if (keys & BUTTON_RIGHT) {
                selected_option = 1;
            }
            if (keys & BUTTON_PRESS) {
                frontier_run_services(HOME_BOARD);
            }
            if (keys & BUTTON_A) {
                break; // Option selected
            }
            if (keys & BUTTON_B) {
                return; //cancel game
            }
            yield();
        }
        if (selected_option == 0) {
            // Handle fuel upgrade
            if (config.max_fuel >= 15 || state.credits < fuel_upgrade_cost()) {
                selected_option++; 
                break;
            }
            state.credits -= fuel_upgrade_cost(); 
            config.max_fuel++;
            state.fuel = config.max_fuel; // Refuel to new max after upgrade
        } else if (selected_option == 1) {
            // Handle hull upgrade
            if (config.max_hull >= 8 || state.credits < hull_upgrade_cost()) {
                selected_option++; 
                break;
            }
            state.credits -= hull_upgrade_cost(); 
            config.max_hull++;
            state.hull = config.max_hull; // Repair hull to new max after upgrade
        } else if (selected_option == 2) {
            break; // Leave market
        }
        yield();
    }
}


const char* game_intro1_text = "Your ship has been commissioned to support the Frontier Stations, the newest supply hubs linking the scattered outposts of our expanding frontier.\nEvery successful voyage strengthens the stations, extends our reach, and brings tomorrow's technology closer.\nYour assignment begins today.";
const char* game_intro2_text = "Before departure, our aptitude engine has assigned you the role of %s.\nEvery expedition depends on specialists who keep the frontier moving forward.\nYour official call sign has also been registered as %s.\nGood luck, Captain.\nThe frontier is waiting.";

void game_show_gameover(const char* reason, const char* message) {
    frame_clear();
    frame_draw_image(95, 26, gameover_image_data, 167, 13);
    frame_draw_image(112, 326, pressa_image_data, 133, 8);
    frame_draw_text(reason, RGB565(172,50,50), 60);
    frame_draw_rounded_rect(40, 80, 280, 212, border_color);
    frame_draw_image(112, 300, abandoned_image_data, 136, 8);
    frame_draw_long_string(44, 98, message, description_color, 272);
    //frame_draw_text(message, description_color, 124);
    frame_refresh();
    while ((buttons_get_and_clear() & BUTTON_A) == 0) {
        yield();
    }
}

void game_show_victory(int score) {
    char buf[10];
    frame_clear();
    frame_draw_image(95, 20, complete_image_data, 169, 32);
    frame_draw_image(83, 280, dockm_image_data, 194, 31);
    int yoff = 64;
    frame_draw_rounded_rect(70, yoff, 220, 200, border_color);
    for (int i=0; i<3;++i) {
        frame_draw_line(80, yoff+50 + i*50, 280, yoff+50 + i*50, border_color);
    }
    frame_draw_image(85, yoff+20, xp_image_data, 26, 13);
    frame_draw_image(84, yoff+65, reputation_image_data, 103, 21);
    frame_draw_image(85, yoff+115, cargod_image_data, 141, 24);
    frame_draw_image(87, yoff+165, star_image_data, 18, 18);
    frame_draw_image(114, yoff+172, tscore_image_data, 78, 8);
    frame_draw_image(112, 326, pressa_image_data, 133, 8);
    snprintf(buf, sizeof(buf), "%5d", state.xp);
    frame_draw_string(230, yoff+32, buf, description_color);
    snprintf(buf, sizeof(buf), "%5d", state.reputation);
    frame_draw_string(230, yoff+82, buf, description_color);
    snprintf(buf, sizeof(buf), "%2d/%d", state.cargo, config.max_cargo);
    frame_draw_string(230, yoff+132, buf, description_color);
    snprintf(buf, sizeof(buf), "%5d", score);
    frame_draw_string(230, yoff+182, buf, description_color);
    frame_refresh();
    while ((buttons_get_and_clear() & BUTTON_A) == 0) {
        yield();
    }
}

void game_run() {
    int mp_final_score = -1;
    game_running = true;
    game_init();
    
    if (!has_open_slot()) {
        frame_clear();
        printf("No free slots before new game\r\n");
        ui_draw_popup("Warning\nThere are no free slots to save game.\nDock at a station to free up slots");
        int buttons;
        while ((buttons = buttons_get_and_clear()) == 0) {
            yield();
        };
    }

    // Only show this until they've completed one game, to avoid annoying the player with repeated messages
    if ((config_get_achievements() & (1<<one_game))== 0) 
    {
        char message[400];
        gui_show_message(game_intro1_text);
        snprintf(message, sizeof(message), game_intro2_text, get_class_name(config_get_class()), config_get_handle());
        gui_show_message(message);
    }

    game_run_home_market();
    while (game_running) {
        int ret = game_run_map();
        if (ret == -1) {
            game_running = false;
            break;
        }
        mp_send_state();
        contracts_on_jump();
        enum tiletype tiletype = get_hex_type(state.player);
        buttons_clear();
        switch (tiletype)
        {
        case TILE_EMPTY:
            if (state.chain_event || (get_rand_32() % 3) == 0) {
                const event_t* event = frontier_pick_event(EV_AT_EMPTY);
                if (event) {
                    game_run_event(event);
                }
            }
            break;
        case TILE_ASTROID:
            if (frontier_choose("Asteroid field ahead.\nA: blast it (shooter)\nB: drill it (rhythm)") == BUTTON_B) {
                int hits = drill_play();
                state.credits += hits * 20;
                frontier_gather_drill(hits);
                for (int i = 0; i < 6; i++) {
                    if (hex_equal(state.astroids[i], state.player)) {
                        state.astroids[i].q = 100;
                        state.astroids[i].r = 100;
                    }
                }
                break;
            }
            {
               int ret = astroids_play();
               if (ret < 0) {
                   state.hull -= 1;
                   state.credits += -ret - 1;
                   frontier_gather_asteroid(-ret - 1);
               }
               else {
                    state.credits += ret;
                    frontier_gather_asteroid(ret);
                    for (int i = 0; i < 6; i++) {
                        if (hex_equal(state.astroids[i], state.player)) {
                            state.astroids[i].q = 100;
                            state.astroids[i].r = 100;
                        };
                    }
                }
            }
            break;
        case TILE_NEBULA:
            int ret = nebula_play();
            if (ret < 1) {
                state.fuel -= 2;
            }
            else {
                state.credits += ret;
                if (!state.nebula_harvested) { state.nebula_harvested = 1; frontier_gather_nebula(); }
            }
            break;
        case TILE_DERELICT:
            frontier_run_derelict();
            {
                const event_t* ev = frontier_pick_event(EV_AT_DERELICT);
                if (ev) game_run_event(ev);
            }
            break;
        case TILE_HOME:
            buttons_clear();
            {
                const event_t* ev = frontier_pick_event(EV_AT_HOME);
                if (ev) game_run_event(ev);
            }
            state.fuel = config.max_fuel; // Refuel at home base
            state.hull = config.max_hull; // Repair hull at home base
            if (frontier_run_home_dock()) {
                int score = frontier_score() + state.reputation + state.xp + crew_count * 10;
                game_show_victory(score);
                mp_final_score = score;
                if (-1==queue_mission_results(score)) {
                    printf("No room to save flight log!\n");
                    ui_draw_popup("No room to save flight log!");
                    int buttons;
                    while ((buttons = buttons_get_and_clear()) == 0) {
                        yield();
                    };
                }
                config_add_mission(score);
                game_running = false;
                config_achievement(one_game);
            }
            break;
        case TILE_POI:
            {
                const event_t* ev = frontier_pick_event(EV_AT_POI);
                if (ev) game_run_event(ev);
                for (int i = 0; i < POI_COUNT; ++i) if (hex_equal(state.poi[i], state.player)) frontier_on_arrive_poi(i);
            }
            game_run_market();
            break;
        }
        if (state.hull <= 0) {
            game_show_gameover("Hull Failure", "Structural integrity collapses under mounting stress.\nThe hull fractures, then shatters.\nWarning alarms cut out mid-scream.\nFor a brief moment, your ship becomes stardust; indistinguishable from the debris it joins.\nMission terminated.");
            game_running = false;
        }
        else if (state.fuel <= 0) {
            game_show_gameover("Fuel Exhausted", "Your engines fall silent as the last reserves burn away.\nYou drift between the stars, momentum carrying you nowhere.\nDistress signals echo into the void... but no one answers.\nYour journey ends in quiet orbit around nothing.");
            game_running = false;
        }
        else if (crew_count <= 0) {
            game_show_gameover("Crew Lost", "The final crew member falls silent.\nNo voices remain on the comms.\nSystems continue their routines, but no one is left to guide them.\nYour ship drifts on; a perfectly functioning vessel with no one aboard.\nMission abandoned.");
            game_running = false;
        }
        yield();
    }
    if (mp_active) mp_send_done(mp_final_score);
}
