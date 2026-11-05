#ifndef FRONTIER_H
#define FRONTIER_H
// Frontier expansion: typed goods, gathering, repairs, ship menu, derelicts. See docs/FRONTIER.md.
#include <stdint.h>
#include <stdbool.h>

#include "bsp/buttons.h"
#define KEY_SELECT (BUTTON_A | BUTTON_PRESS)   // joystick press doubles as the third button
#define GOOD_COUNT 8
enum good { GOOD_SUPPLIES = 0, GOOD_ORE, GOOD_GAS, GOOD_ALLOY, GOOD_PARTS, GOOD_RELIC, GOOD_HE3, GOOD_DARK };
#define PERK_EXOTIC 1      // persistent: learned to burn exotic matter at a fuel depot
void frontier_on_arrive_poi(int poi_idx);          // depot discovery, perk unlock
#define DERELICT_COUNT 3

void game_draw_ui(void);                 // game.c HUD

extern const char* good_names[GOOD_COUNT];
extern const int good_value[GOOD_COUNT];
extern const uint16_t good_colors[GOOD_COUNT];

bool frontier_has_role(char role);
int  frontier_add_good(int g, int n);    // returns how many fit in the hold
int  frontier_take_good(int g, int n);   // returns how many were actually removed
int  frontier_score(void);               // value of everything in the hold
void frontier_sync_cargo(void);
void frontier_run_goods_market(int poi_idx);
void frontier_run_ship_menu(void);
void frontier_gather_asteroid(int score);
void frontier_gather_nebula(void);
void frontier_run_derelict(void);
void frontier_draw_derelicts(int x, int y);
void frontier_popup_wait(const char* text);

int maze_play(uint32_t limit_ms);        // mazegame.c: 1 = reached the core, 0 = failed or quit
int signal_lock_play(int rounds, int start_len);   // minigames.c: 1 = decoded
int drill_play(void);                              // minigames.c: ore chunks mined 0..8
int  frontier_take_any(int n);           // remove n crates of anything, supplies first
int  frontier_choose(const char* text);  // popup, returns BUTTON_A or BUTTON_B
void frontier_gather_drill(int hits);

// contracts.c
#define HOME_BOARD 9
void contracts_run_board(int board);
void contracts_show_summary(void);
void contracts_on_jump(void);
void contracts_on_salvage(int derelict_idx);

// story engine (frontier.c)
struct event;
const struct event* frontier_pick_event(int at);   // NULL when nothing eligible
void frontier_apply_story_effect(uint8_t type, int16_t value);
void frontier_run_services(int board);             // outpost / station services menu (goods, jobs, shipyard)
void frontier_run_shipyard(int board);             // upgrades mid run at home or a shipyard outpost
bool frontier_has_shipyard(int board);
int  frontier_run_home_dock(void);                 // 1 = turn in and complete the mission, 0 = depart
#define MOD_MINING 1
#define MOD_TANKS  2
#define MOD_SHIELD 4
#define MOD_HOLD   8
void frontier_draw_poi_marks(int x, int y);        // shipyard badges over outposts on the map

#endif
