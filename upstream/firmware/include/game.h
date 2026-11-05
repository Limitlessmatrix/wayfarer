#ifndef GAME_H
#define GAME_H

#include "hexutils.h"
#include "eventbase.h"
#include "bsp/common.h"

#define POI_COUNT       9

typedef struct {
    int seed;
    int poi_market_rate[POI_COUNT];
    Hex poi[POI_COUNT];
    Hex nebula;
    Hex astroids[6];
    Hex player;
    int heading;
    int hull;
    int fuel;
    int credits;
    int cargo;
    int reputation;
    int xp;
    uint8_t hold[8];          // frontier goods, state.cargo is their sum
    uint32_t story_flags;
    Hex derelicts[3];
    uint8_t derelict_looted;
    uint32_t events_used[4];   // once-only events already fired this run (by table index)
    uint8_t chain_event;       // event id forced onto the next empty tile, 0 = none
    uint32_t jobs_taken;       // bit poi*2+slot, jobs accepted this run (never re-offered)
    uint16_t shipyard_mask;    // outposts with a shipyard (bit = poi index)
    uint16_t depot_mask;       // outposts that are exotic fuel depots
    uint8_t modules;           // mission unlocked modules installed this run (MOD_*)
    uint8_t nebula_harvested;  // gas and He-3 come out of the nebula once per run
    uint8_t jobs_done;
    uint8_t job_gen[10];       // per board generation, bumps when a job from that board completes
    struct contract { uint8_t active, type, good, count, target, origin, slot, progress, deadline; int16_t reward; } contracts[3];
} game_state_t;

typedef struct {
    int xp;
    game_character_t self;
    uint32_t achievments; // Bitfield for achievments
} persistant_state_t;

typedef struct {
    int max_fuel;
    int max_hull;
    int max_cargo;
    int starting_credits;
} game_config_t;

enum tiletype {
    TILE_EMPTY,
    TILE_NEBULA,
    TILE_ASTROID,
    TILE_HOME,
    TILE_POI,
    TILE_DERELICT
};

void game_run();

#endif // GAME_H