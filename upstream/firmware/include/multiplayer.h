#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H
// Emulator to emulator (and badge to badge over the dock UART) multiplayer.
// Line protocol carried on the dock terminal channel, every line starts with "$MP".
//   $MP ROOM <seed> <slot>                              host -> this badge, joined a room
//   $MP HELLO <id> <name> <class>                        announce yourself
//   $MP POS <id> <name> <class> <q> <r> <heading> <hull> <fuel> <credits> <cargo> <xp> <rep>
//   $MP DONE <id> <name> <score>                         run finished, score or -1 if lost
//   $MP BYE <id>                                         peer left
// Names carry '_' in place of spaces on the wire.
#include <stdint.h>
#include <stdbool.h>

#define MP_MAX_PEERS 8

typedef struct {
    bool active;
    bool done;
    char id[6];
    char name[15];
    char cls;
    int q, r, heading;
    int hull, fuel, credits, cargo, xp, reputation;
    int score;
    uint32_t last_seen_ms;
} mp_peer_t;

extern bool mp_active;
extern uint32_t mp_seed;

void mp_handle_command(const char* line);
void mp_tick(void);              // call every map frame: drains $MP lines, heartbeats
void mp_send_hello(void);
void mp_send_state(void);
void mp_send_done(int score);
void mp_show_fleet(void);        // popup with every peer, waits for a button
const mp_peer_t* mp_peers(void);
int mp_peer_count(void);

#endif
