#include "multiplayer.h"
#include "game.h"
#include "bsp/terminal.h"
#include "bsp/config.h"
#include "bsp/buttons.h"
#include "ui.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern game_state_t state;
extern int game_running;

bool mp_active = false;
uint32_t mp_seed = 0;
static mp_peer_t peers[MP_MAX_PEERS];
static uint32_t last_heartbeat_ms = 0;

static void wire_name(char* dst, size_t n, const char* src, char from, char to) {
    size_t i = 0;
    for (; src[i] && i + 1 < n; ++i) dst[i] = (src[i] == from) ? to : src[i];
    dst[i] = '\0';
}

static mp_peer_t* find_peer(const char* id, bool create) {
    for (int i = 0; i < MP_MAX_PEERS; ++i) {
        if (peers[i].active && strncmp(peers[i].id, id, 5) == 0) return &peers[i];
    }
    if (!create) return NULL;
    for (int i = 0; i < MP_MAX_PEERS; ++i) {
        if (!peers[i].active) {
            memset(&peers[i], 0, sizeof(peers[i]));
            peers[i].active = true;
            strncpy(peers[i].id, id, 5);
            peers[i].id[5] = '\0';
            peers[i].score = -1;
            return &peers[i];
        }
    }
    return NULL;
}

const mp_peer_t* mp_peers(void) { return peers; }

int mp_peer_count(void) {
    int n = 0;
    for (int i = 0; i < MP_MAX_PEERS; ++i) if (peers[i].active) ++n;
    return n;
}

static uint32_t now_ms(void) { return to_ms_since_boot(get_absolute_time()); }

void mp_send_hello(void) {
    char line[96], name[16];
    wire_name(name, sizeof(name), config_get_handle(), ' ', '_');
    snprintf(line, sizeof(line), "$MP HELLO %s %s %c", config_get_id(), name, config_get_class());
    terminal_send(line);
}

void mp_send_state(void) {
    if (!mp_active || !game_running) return;
    char line[160], name[16];
    wire_name(name, sizeof(name), config_get_handle(), ' ', '_');
    snprintf(line, sizeof(line), "$MP POS %s %s %c %d %d %d %d %d %d %d %d %d",
             config_get_id(), name, config_get_class(),
             state.player.q, state.player.r, state.heading,
             state.hull, state.fuel, state.credits, state.cargo, state.xp, state.reputation);
    terminal_send(line);
    last_heartbeat_ms = now_ms();
}

void mp_send_done(int score) {
    if (!mp_active) return;
    char line[96], name[16];
    wire_name(name, sizeof(name), config_get_handle(), ' ', '_');
    snprintf(line, sizeof(line), "$MP DONE %s %s %d", config_get_id(), name, score);
    terminal_send(line);
}

void mp_handle_command(const char* line) {
    char cmd[16], id[8], name[24];
    char cls = '?';
    if (sscanf(line, "$MP %15s", cmd) != 1) return;
    if (strcmp(cmd, "ROOM") == 0) {
        unsigned long seed = 0; int slot = 0;
        if (sscanf(line, "$MP ROOM %lu %d", &seed, &slot) >= 1) {
            mp_seed = (uint32_t)seed;
            mp_active = true;
            memset(peers, 0, sizeof(peers));
            mp_send_hello();
            if (game_running) mp_send_state();
        }
        return;
    }
    if (sscanf(line, "$MP %*s %7s", id) != 1) return;
    if (strncmp(id, config_get_id(), 5) == 0) return;   // our own line echoed back
    if (strcmp(cmd, "BYE") == 0) {
        mp_peer_t* p = find_peer(id, false);
        if (p) p->active = false;
        return;
    }
    bool known = find_peer(id, false) != NULL;
    mp_peer_t* p = find_peer(id, true);
    if (!p) return;
    p->last_seen_ms = now_ms();
    if (strcmp(cmd, "HELLO") == 0) {
        if (sscanf(line, "$MP HELLO %*s %23s %c", name, &cls) == 2) {
            wire_name(p->name, sizeof(p->name), name, '_', ' ');
            p->cls = cls;
        }
        if (!known) mp_send_hello();          // late joiners learn about us too (once per peer)
        if (game_running) mp_send_state();   // let the newcomer see us right away
    } else if (strcmp(cmd, "POS") == 0) {
        int q, r, h, hull, fuel, credits, cargo, xp, rep;
        if (sscanf(line, "$MP POS %*s %23s %c %d %d %d %d %d %d %d %d %d",
                   name, &cls, &q, &r, &h, &hull, &fuel, &credits, &cargo, &xp, &rep) == 11) {
            wire_name(p->name, sizeof(p->name), name, '_', ' ');
            p->cls = cls; p->q = q; p->r = r; p->heading = h;
            p->hull = hull; p->fuel = fuel; p->credits = credits; p->cargo = cargo;
            p->xp = xp; p->reputation = rep;
            p->done = false;
        }
    } else if (strcmp(cmd, "DONE") == 0) {
        int score = -1;
        if (sscanf(line, "$MP DONE %*s %23s %d", name, &score) == 2) {
            wire_name(p->name, sizeof(p->name), name, '_', ' ');
            p->done = true;
            p->score = score;
        }
    }
}

void mp_tick(void) {
    char command[512];
    terminal_update();
    while (terminal_pop_docking_command(command, sizeof(command))) {
        if (strncmp(command, "$MP", 3) == 0) mp_handle_command(command);
    }
    if (mp_active && game_running && now_ms() - last_heartbeat_ms > 2000) {
        mp_send_state();
    }
}

void mp_show_fleet(void) {
    char text[400];
    int len = snprintf(text, sizeof(text), "FLEET  (seed %08lX)\n", (unsigned long)mp_seed);
    int shown = 0;
    for (int i = 0; i < MP_MAX_PEERS && len < (int)sizeof(text) - 40; ++i) {
        const mp_peer_t* p = &peers[i];
        if (!p->active) continue;
        if (p->done) {
            len += snprintf(text + len, sizeof(text) - len, "%s: %s %d\n", p->name,
                            p->score < 0 ? "lost" : "home", p->score);
        } else {
            len += snprintf(text + len, sizeof(text) - len, "%s: $%d C%d H%d F%d\n",
                            p->name, p->credits, p->cargo, p->hull, p->fuel);
        }
        ++shown;
    }
    if (!shown) snprintf(text + len, sizeof(text) - len, "no other ships yet");
    buttons_clear();
    ui_draw_popup(text);
    while (buttons_get_and_clear() == 0) {
        mp_tick();
        sleep_ms(10);
    }
}
