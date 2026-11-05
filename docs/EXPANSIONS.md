# Orion Trail expansions: brainstorm and engineering notes

Written 2026-11-04 after reading the stock source. Each idea names what changes in C and what is pure data,
because everything here has to run on the RP2350 (520 KB RAM, 16 MB flash, 1 KB EEPROM, 360x360 screen,
7 buttons) as well as in the emulator.

## What the stock game is

One run = one trading voyage. Hex map with home station at the centre, 9 outposts at distance 4 to 10,
one nebula, 6 asteroid tiles. Ship has hull, fuel, credits, one generic cargo count. Each move onto an
empty tile has a 1 in 3 chance of a random event (66 of them, YAML driven, up to 4 options, weighted
outcomes, role gated options). Asteroid and nebula tiles are arcade minigames that pay credits.
Outposts buy and sell generic crates and fuel with a drifting market rate. Home sells fuel tank and
hull upgrades. Return home with cargo to score. Crew come from the physical dock station, where other
attendees' badges become your crew members and give role gated options. Achievements live in EEPROM.

Multiplayer already exists in a weak asynchronous form: the dock station (a Pico W running
`station/main.py`) collects `$ID` and flight logs from each badge over UART, pushes `$CREW` lists
built from other players, a leaderboard `$STATION` line, and construction votes `$OPTIONS`.

## 1. Resources and trade

Today cargo is one number. Proposal: typed goods with supply and demand so routes matter.

- Goods: Ore, Food, Medicine, Tech, Fuel Cells, Relics (rare, quest only). `uint8_t hold[6]` replaces
  `int cargo`; `max_cargo` stays the slot cap.
- Each outpost gets a profile from `pois.yml`: name, produces, consumes, base prices. Buying raises the
  local price, selling drops it (the stock 15 percent and 10 percent rules, per good).
- Contracts: outposts post "deliver 4 Medicine to Beacon Station, pays 900 plus reputation". Taken
  contracts live in `game_state_t` as `contract[3]`.
- Score keeps the stock formula but counts goods by value, so a Relic beats a crate of Ore.

C work: state struct, market screen (two columns of goods instead of buy/sell), score. UI is the cost;
the market screen needs a scrolling list. Data: `pois.yml` plus a `goods` effect type in events
(`Ore: -2`, `Relic: +1`).

## 2. Gathering

- Asteroid mining: the shooter minigame pays Ore instead of raw credits, with a yield multiplier for an
  Engineer on the crew. Cleared fields respawn after N turns instead of vanishing.
- Nebula harvesting: the nebula minigame pays Fuel Cells, Scientist multiplier.
- Salvage: debris field tiles (new tile type) with a short scan minigame; rolls on a loot table
  (`loot.yml`), can yield Relics or trigger a salvage event chain.
- Scanning: a Scientist action from the map that reveals the nearest hidden POI or salvage tile.
- Away missions: landing on a planet tile picks an event from a planet specific pool.

C work: new tile types in `enum tiletype`, `game_init` placement, one new minigame or reuse of the maze
game (`mazegame.c` is already in the tree and unused, a derelict ship crawl fits it perfectly).
Data: loot tables and planet event pools in YAML.

## 3. Repairs and ship systems

- Subsystems: Engine, Sensors, Life Support, Cargo Bay, each 0 to 3 condition. Damage effects in events
  can target a subsystem instead of hull. Low Engine raises fuel burn, low Sensors hides POIs, low Life
  Support risks crew, low Cargo Bay loses goods on hits.
- Repair kits as a good; an Engineer repairs 1 point per turn in flight for free, anyone else spends a kit.
- Shipyard at home: repair all, buy upgrades, install modules (Ore Refinery, Shield Capacitor,
  Long Range Scanner, Medbay) that unlock new event options and change rules.
- Hull breach events become repair decisions rather than flat damage.

C work: `subsys[4]` in state, movement cost and event filters read them, shipyard screen gains a page.
Data: `subsystem` effect type, module definitions.

## 4. Quests and story lines

The engine needs four small additions and then everything is YAML:

- `story_flags` (uint32) in game state, `requires_flag`, `sets_flag`, `clears_flag` on events and options.
- `once: true` events fire at most once per run; `weight` adjusts frequency instead of the flat shuffle.
- `at:` restricts an event to a tile type, a named POI, or the home station.
- `chain: <id>` queues a follow up event for the next empty tile.

With that, a quest is a chain of events with flags, and persistent story progress is a few bits in the
EEPROM record (there is room next to the achievement mask). Story arcs to write:

- The First Builders: relic hunt across three outposts, ends with the Monolith (stock events 3 and 5
  already hint at it and have achievements).
- Crimson Nebula raiders: faction reputation, pay them off, fight them, or join them. A second ending.
- Frontier Stations construction: tie the dock votes to in game outposts that upgrade over the con.
- Crew personal quests: each role gets a short arc that levels that crew member.
- Smuggler route: the hooded figure events become an opt in black market with Customs heat.

## 5. Multiplayer

Status 2026-11-04: Tier A is built and tested on the emulator (relay, rooms, shared seed, ghost
ships, fleet list, BYE on disconnect). See README. Next: finish and race scoring, cargo transfer,
browser build so attendees only need a link, relay on a public host.

Three tiers, each buildable on the previous.

Tier A, emulator only, fastest: a host side link. `platform_2026.c` already stubs UART0. Route it over
TCP between two emulator instances and the game code sees a dock style serial peer. First mode:
Convoy, two ships on the same seed and shared market state, cargo transfer when adjacent, shared score.
Second mode: Race, same seed, first home with the quota wins, with sabotage events you can send.
Also a lobby server in Python so the browser build can join. Everything here also runs on real badges
cabled together through the dock connector (GPIO 0 and 1 are UART0, GPIO 29 is the detect line), so
the protocol is written once.

Tier B, station hosted shared galaxy: extend `station/main.py` so the dock holds a persistent world,
outpost prices move with everyone's trades, contracts are posted by the station, and a bounty board
tracks who did what. Badges sync when docked, the emulator syncs over TCP. Asynchronous, works at a con
with no radio.

Tier C, live co op on hardware: needs a radio. The badge has none, but the dock connector exposes
UART and power, so a small ESP32 dongle on the connector gives WiFi and the same TCP protocol as Tier A.
That makes the emulator and the badge first class peers in one session.

Protocol sketch: line based like the dock, `$HELLO`, `$STATE seed,turn,q,r,hull,fuel,credits`,
`$MARKET poi,prices`, `$GIVE good,count`, `$EVENT id` for sent sabotage, `$BYE`. Signed with the same
komihash tag the dock uses so a badge cannot be spoofed by a stranger.

## 6. Smaller wins

- Difficulty tiers and daily seed runs with a shareable seed code.
- Named ship and ship paint from the achievement count.
- Flight log replay in the emulator from a seed plus input script (the headless CLI already does this).
- Captain's log entries written by events so a run reads like a story afterwards.
- New achievements per expansion, 12 community bits remain free in the mask.

## Suggested order

1. Engine hooks for quests (flags, once, at, chain) and the pack merger. Small, unlocks all story work.
2. First story pack, First Builders, three events, to prove the pipeline on emulator and badge.
3. Typed goods and named outposts. Largest UI change, do it before systems depend on it.
4. Gathering rewrites of the two minigames plus salvage tiles using the unused maze game.
5. Subsystems and repairs.
6. Multiplayer Tier A on the emulator, then cable test on two badges.
7. Station hosted galaxy.

Each step ships as a UF2 and a pack of YAML. Regression is the headless CLI with fixed seeds.
