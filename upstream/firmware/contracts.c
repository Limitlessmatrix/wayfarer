// Frontier contracts: delivery, collection, salvage and courier jobs posted by outposts and home.
#include "frontier.h"
#include "game.h"
#include "multiplayer.h"
#include "framebuffer.h"
#include "buttons.h"
#include "ui.h"
#include "hexutils.h"
#include "touch.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

extern game_state_t state;
extern const char* poi_names[10];

enum { CT_DELIVER = 0, CT_COLLECT, CT_SALVAGE, CT_COURIER, CT_TYPES };
#define JOB_SLOTS 2
#define HOME_BOARD 9

static uint32_t job_rng(uint32_t* s) { *s = *s * 1664525u + 1013904223u; return *s >> 8; }

// Deterministic job for (board, slot) from the map seed, so a multiplayer room shares the boards.
static Hex board_hex(int board) { return board == HOME_BOARD ? (Hex){0, 0} : state.poi[board]; }

// Pick a target outpost within reach of the posting board: the nearest few, never the board itself.
static int pick_near_target(int board, uint32_t* s) {
    // every outpost within 6 jumps of the board; if none, the single nearest one
    int near[POI_COUNT], n = 0, nearest = -1, nd = 999;
    for (int p = 0; p < POI_COUNT; ++p) {
        if (p == board) continue;
        int d = hex_distance(board_hex(board), state.poi[p]);
        if (d <= 6) near[n++] = p;
        if (d < nd) { nd = d; nearest = p; }
    }
    if (n == 0) return nearest;
    return near[job_rng(s) % n];
}

static struct contract make_job(int board, int slot) {
    struct contract c;
    memset(&c, 0, sizeof(c));
    uint32_t s = (uint32_t)state.seed ^ (uint32_t)(board * 0x9E3779B1u) ^ (uint32_t)(slot * 0x85EBCA6Bu)
                 ^ (uint32_t)(state.job_gen[board] * 0xC2B2AE35u);
    job_rng(&s);
    c.type = job_rng(&s) % CT_TYPES;
    if (slot > 0) {
        // never post the same job type twice on one board
        struct contract first = make_job(board, 0);
        if (c.type == first.type) c.type = (c.type + 1 + job_rng(&s) % (CT_TYPES - 1)) % CT_TYPES;
    }
    c.origin = board;
    c.slot = slot;
    int other = pick_near_target(board, &s);
    int dist = hex_distance(board_hex(board), state.poi[other]);
    switch (c.type) {
    case CT_DELIVER:
        c.good = GOOD_ORE + job_rng(&s) % 4;
        c.count = 2 + job_rng(&s) % 2;
        c.target = other;
        c.reward = c.count * (good_value[c.good] + 100) + 30 * dist;
        break;
    case CT_COLLECT:
        c.good = GOOD_ORE + job_rng(&s) % 4;
        c.count = 2 + job_rng(&s) % 2;
        c.reward = c.count * (good_value[c.good] + 30);
        break;
    case CT_SALVAGE: {
        int bd = 999;
        for (int i = 0; i < DERELICT_COUNT; ++i) {
            int d = hex_distance(board_hex(board), state.derelicts[i]);
            if (d < bd) { bd = d; c.target = i; }
        }
        c.reward = 400 + 40 * bd;
        break;
    }
    case CT_COURIER:
        c.target = other;
        c.deadline = dist + 4;
        c.reward = 100 + 50 * c.deadline;
        break;
    }
    return c;
}

static int job_distance(const struct contract* c) {
    Hex from = state.player;
    if (c->type == CT_DELIVER || c->type == CT_COURIER) return hex_distance(from, state.poi[c->target]);
    if (c->type == CT_SALVAGE) return hex_distance(from, state.derelicts[c->target]);
    return 0;
}

static void describe(const struct contract* c, char* out, size_t n) {
    switch (c->type) {
    case CT_DELIVER: snprintf(out, n, "Deliver %d %s to %s", c->count, good_names[c->good], poi_names[c->target]); break;
    case CT_COLLECT: snprintf(out, n, "Collect %d %s, any board", c->count, good_names[c->good]); break;
    case CT_SALVAGE: snprintf(out, n, "Salvage wreck %d, any board", c->target + 1); break;
    case CT_COURIER: snprintf(out, n, "Courier to %s, %d jumps", poi_names[c->target], c->deadline); break;
    }
}
extern int round_half_width(int y);

static bool job_taken(int board, int slot) { return (state.jobs_taken >> (board * 2 + slot)) & 1; }

static int free_contract(void) {
    for (int i = 0; i < 3; ++i) if (!state.contracts[i].active) return i;
    return -1;
}

// Is this accepted contract completable at the current board (poi index or HOME_BOARD)?
static bool can_turn_in(const struct contract* c, int board) {
    switch (c->type) {
    case CT_DELIVER: return board == c->target && state.hold[c->good] >= c->count;
    case CT_COLLECT: return state.hold[c->good] >= c->count;
    case CT_SALVAGE: return c->progress;
    case CT_COURIER: return board == c->target;
    }
    return false;
}

static void complete(struct contract* c) {
    if (c->type == CT_DELIVER || c->type == CT_COLLECT) frontier_take_good(c->good, c->count);
    state.credits += c->reward;
    state.reputation += 50;
    state.xp += 25;
    state.jobs_done++;
    state.jobs_taken &= ~(1u << (c->origin * 2 + c->slot));
    if (c->origin < 10) state.job_gen[c->origin]++;
    c->active = 0;
    char msg[96];
    snprintf(msg, sizeof(msg), "Contract complete.\nCredits +%d  Reputation +50  XP +25", c->reward);
    frontier_popup_wait(msg);
}

// Called after every jump: courier deadlines tick, arrivals auto complete couriers.
void contracts_on_jump(void) {
    for (int i = 0; i < 3; ++i) {
        struct contract* c = &state.contracts[i];
        if (!c->active || c->type != CT_COURIER) continue;
        if (hex_equal(state.player, state.poi[c->target])) { complete(c); continue; }
        if (c->deadline > 0) c->deadline--;
        if (c->deadline == 0) {
            c->active = 0;
            state.reputation -= 50;
            frontier_popup_wait("Courier deadline missed.\nReputation -50");
        }
    }
}

void contracts_on_salvage(int derelict_idx) {
    for (int i = 0; i < 3; ++i) {
        struct contract* c = &state.contracts[i];
        if (c->active && c->type == CT_SALVAGE && c->target == derelict_idx) c->progress = 1;
    }
}

// Job board screen. board = poi index, or HOME_BOARD at the station.
void contracts_run_board(int board) {
    char lines[5][64];
    char sub[5][32];
    const char* title = board == HOME_BOARD ? "STATION JOBS" : poi_names[board];
    int sel = 0;
    buttons_clear();
    while (true) {
        // rows: offered jobs (not taken), then accepted contracts
        struct contract offered[JOB_SLOTS];
        int rows = 0, offered_n = 0;
        bool turnin[5] = {0};
        int accepted_idx[5] = {0};
        for (int k = 0; k < JOB_SLOTS; ++k) {
            if (job_taken(board, k)) continue;
            offered[offered_n] = make_job(board, k);
            describe(&offered[offered_n], lines[rows], sizeof(lines[rows]));
            {
                int jd = job_distance(&offered[offered_n]);
                if (jd) snprintf(sub[rows], sizeof(sub[rows]), "pays $%d, %d jumps out", offered[offered_n].reward, jd);
                else snprintf(sub[rows], sizeof(sub[rows]), "pays $%d", offered[offered_n].reward);
            }
            offered_n++; rows++;
        }
        for (int i = 0; i < 3 && rows < 5; ++i) {
            if (!state.contracts[i].active) continue;
            char d[64];
            describe(&state.contracts[i], d, sizeof(d));
            turnin[rows] = can_turn_in(&state.contracts[i], board);
            snprintf(lines[rows], sizeof(lines[rows]), "%s", d);
            snprintf(sub[rows], sizeof(sub[rows]), turnin[rows] ? "READY: A to turn in, $%d" : "accepted, $%d", state.contracts[i].reward);
            accepted_idx[rows] = i;
            rows++;
        }
        if (sel >= rows) sel = rows ? rows - 1 : 0;
        frame_clear();
        frame_draw_text(title, title_color, 24);
        frame_draw_string_centered(46, "JOB BOARD", description_color);
        if (!rows) frame_draw_string_centered(120, "No work posted here today.", unavailable_color);
        for (int r = 0; r < rows; ++r) {
            int y = 82 + r * 40;
            int hw = round_half_width(y + 16) - 10;
            int x0 = LCD_WIDTH / 2 - hw;
            if (r == sel) frame_fill_rect(x0, y - 15, 2 * hw, 38, sbackground_color);
            frame_draw_string_fit(x0 + 4, y, lines[r], r == sel ? selected_color : option_color, 2 * hw - 8);
            frame_draw_string_fit(x0 + 4, y + 17, sub[r], turnin[r] ? RGB565(80, 220, 120) : description_color, 2 * hw - 8);
        }
        frame_draw_string_centered(296, "A accept or turn in   B back", description_color);
        game_draw_ui();
        frame_refresh();
        uint32_t keys = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) {
                int r = ui_tap_row(ty, 82, 40, rows);
                if (r >= 0) { sel = r; keys |= BUTTON_A; } else if (ty > 280) keys |= BUTTON_B;
            }
        }
        if (keys & BUTTON_UP) sel = (sel + rows - 1) % (rows ? rows : 1);
        if (keys & BUTTON_DOWN) sel = (sel + 1) % (rows ? rows : 1);
        if (keys & BUTTON_B) break;
        if ((keys & KEY_SELECT) && rows) {
            if (sel < offered_n) {
                int f = free_contract();
                if (f < 0) frontier_popup_wait("You already hold 3 contracts.");
                else {
                    state.contracts[f] = offered[sel];
                    state.contracts[f].active = 1;
                    state.jobs_taken |= 1u << (board * 2 + offered[sel].slot);
                    frontier_popup_wait("Contract accepted.");
                }
            } else if (turnin[sel]) {
                complete(&state.contracts[accepted_idx[sel]]);
            }
        }
        mp_tick();
        sleep_ms(1);
    }
    buttons_clear();
}

// Summary for the ship menu.
void contracts_show_summary(void) {
    char text[320];
    int len = snprintf(text, sizeof(text), "CONTRACTS\n");
    int n = 0;
    for (int i = 0; i < 3; ++i) {
        if (!state.contracts[i].active) continue;
        char d[64];
        describe(&state.contracts[i], d, sizeof(d));
        len += snprintf(text + len, sizeof(text) - len, "%s\n", d);
        n++;
    }
    if (!n) snprintf(text + len, sizeof(text) - len, "none accepted\nvisit a job board at an outpost");
    frontier_popup_wait(text);
}
