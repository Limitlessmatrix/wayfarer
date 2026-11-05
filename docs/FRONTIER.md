# Frontier expansion: materials, gathering, repairs, contracts, quests

Engineering spec for the first gameplay expansion. Everything runs on the badge and the emulator.
Written 2026-11-04, implemented in stages below.

## Materials (goods)

`game_state_t` gains `uint8_t hold[GOOD_COUNT]`; `state.cargo` stays the total so the HUD crates,
events and the stock score keep working. Goods:

| idx | name     | source                           | value at home | notes |
|----:|----------|----------------------------------|--------------:|-------|
| 0 | Supplies | outpost markets (the stock crates) | 300 | stock cargo |
| 1 | Ore      | asteroid fields                  | 150 | refine 2 Ore + 1 Gas -> 1 Alloy |
| 2 | Gas      | nebula                           | 150 | fuel cells, 1 Gas -> 1 fuel anywhere |
| 3 | Alloy    | refining, derelicts              | 450 | 1 Alloy -> 2 Parts |
| 4 | Parts    | derelicts, outposts (buy)        | 200 | 1 Parts -> +1 hull, in flight |
| 5 | Relic    | derelicts, story events          | 1000 | quest item, never sold at outposts |

Outposts trade Supplies as before and also buy Ore, Gas, Alloy and sell Parts at prices scaled by the
outpost market rate. Relics only count at home. Victory score = sum(hold[i] * value[i]) + reputation
+ xp + crew * 10, so the stock formula is unchanged when only Supplies are carried.

## Gathering

- Asteroid tile: minigame pays credits as before plus Ore = score / 30 crates (Engineer on board +50%).
- Nebula tile: plus 2 Gas (Scientist +1).
- Derelict tile (new, 3 per map, drawn with the station sprite recoloured): the unused maze game is
  the wreck crawl. Reaching the core yields a loot roll: Parts 50%, Alloy 30%, Relic 15%, nothing 5%.
  Failing costs 1 hull. A cleared derelict stays on the map as scrap and cannot be looted again.
- Use Gas: from the ship menu, convert 1 Gas to 1 fuel.

## Repairs and the ship menu

Centre press on the map opens the ship menu (it already opened the fleet list in multiplayer):

    Repair hull   1 Parts -> +1 hull (Engineer: 1 Parts -> +2)
    Refuel        1 Gas -> +1 fuel
    Refine        2 Ore + 1 Gas -> 1 Alloy (needs Engineer or Scientist)
    Make parts    1 Alloy -> 2 Parts
    Contracts     list of accepted jobs and progress
    Fleet         other ships (multiplayer only)
    Back

Outposts get a Goods page: up/down picks a good, left sells one, right buys one, B returns.

## Shipyards (added 2026-11-04)

Three outposts per map carry a shipyard (deterministic from the map seed, green wrench badge on the
map, `state.shipyard_mask`). Services there and at home offer fuel tank, hull armor, cargo bay (+4
to 32, HUD switches to a 5 px crate pitch) and paid hull repair, with a 20 percent engineer discount.

## Exotic fuel and depots (added 2026-11-04)

Goods 6 and 7 are He-3 ice (value 350) and dark matter (900). `state.depot_mask` marks two outposts
per map, never shipyards. First arrival at a depot grants PERK_EXOTIC via `config_add_perk`, kept in
the top two bits of the missions byte (missions capped at 63). Ship menu gains Burn He-3 (+3 fuel)
and Burn dark matter (full tank). Nebula adds 1 He-3 with the perk; derelict loot has a 5 percent
dark matter slice. Depots trade both at 160 and 650 base price; elsewhere they show depots only.

## Balance pass (2026-11-04)

Starting ship: fuel 8 (was 5, outposts sit 4 to 10 jumps out), hull 4 (was 3), bay 24, credits 3000
plus 50 per achievement. Fuel tank upgrades cost 20 per level instead of 25. Jobs pick targets among
the four nearest outposts to the posting board, deliver and collect counts are 2 or 3, salvage always
names the wreck nearest the board, courier deadlines are distance plus 4, rewards scale with distance,
the board shows how many jumps away each job is from where you stand, and a completed job frees its
slot with a fresh one (per board generation counter).

## Minigame balance and title (2026-11-04)

Asteroids: 20 s (was 10), 5 rocks (was 6), max radius 30, drift 20 px/s (was 30), turn 3.6 rad/s,
thrust 70, 1.5 s collision grace at start, 15 per hit, a collision now costs 1 hull and keeps the
credits and ore earned so far. Nebula: 18 s (was 12), 36 clouds (was 50), in cloud speed cap 32 and
damping 0.93. Maze: 60 s. Title screen: drifting wreck sprite, FRONTIER EXPANSION tagline, mission
tally once at least one mission is logged.

Footprint: firmware 1.58 MB of 16 MB flash (stock was 1.53 MB), RAM 274 KB of 520 KB of which the
360x360 framebuffer is 259 KB. Sprites live in frontier_img.c (generated), ~14 KB.

## Minigames added 2026-11-04 (minigames.c)

signal_lock_play(rounds, start_len): d-pad pattern memory with LED groups per direction, 5 s per input,
B aborts. Used at every wreck core after the maze; failure yields 1 Parts instead of the loot roll.
drill_play(): timing bar, cursor speed 150 px/s plus 28 per hit, zone shrinks 3 px per beat, 4 s per
beat, returns hits. Asteroid tiles offer blast or drill; drill pays 20 credits and 1 ore per hit (engineer
and mining rig multipliers apply) with no hull risk, and consumes the rock.

Bugs fixed the same night: SELL CARGO paid out with no supplies in the hold (hold total was checked, not
supplies) which gave infinite credits once any other good was aboard; negative cargo event effects now
remove any goods (supplies first); the nebula harvest happens once per run instead of every re-entry.

## Contracts (tasks and gigs)

The home station and every outpost post jobs. Up to 3 accepted at a time, stored in
`state.contract[3]`. Generated from the map seed and the outpost index so a multiplayer room shares the
same job board. Types:

- Deliver: N of good G to outpost P. Reward credits + reputation.
- Collect: gather N of G (any source), turn in anywhere. Reward credits + xp.
- Salvage: loot the derelict nearest P. Reward credits + a Relic.
- Courier: reach outpost P within T jumps. Reward reputation + xp.

Turn in from the Jobs page of the posting outpost or home. Completed contracts add to a per run
counter that feeds a new achievement bit (Frontier Contractor, community bit 31 if free).

## Quests and story (engine hooks)

Event data gains, all optional in YAML:

    requires_flags: [name, ...]   event only offered when all flags set
    forbids_flags:  [name, ...]   event skipped when any flag set
    once: true                    at most once per run
    at: empty | asteroid | nebula | derelict | poi | home   (default empty)
    weight: 1..9                  relative frequency (default 3)
    chain: <event id>             force this event on the next empty tile

Options and outcomes gain effects: `set_flag: name`, `clear_flag: name`, `ore: +2` etc for goods,
`chain: id`. Flags are a uint32 in `game_state_t` with names declared in a `flags:` block of the YAML.
Selection: walk the shuffled list, skip ineligible events, honour a pending chain first.

First story pack (packs/first_builders.yml): three chained events ending at the Obsidian Monolith,
rewards a Relic and the existing game_event2 achievement.

## Status 2026-11-04

Stages 1 to 5 are implemented and verified with scripted screenshots on the host build. Weight only
distinguishes rare (1) from normal; outposts are still the stock nine with stock names. Subsystems,
persistent story progress and contract achievements are not done.

## Stages

1. Goods, hold, score, outpost goods page, shipyard refine. Tests: buy ore at an outpost headless, screenshot.
2. Gathering yields and derelict tiles with the maze crawl.
3. Ship menu with repair, refuel, refine, parts.
4. Contracts.
5. Story hooks in the generator and engine, first pack.
Each stage: build host, headless screenshots, emu-cli smoke, then `make badge` for the UF2.
