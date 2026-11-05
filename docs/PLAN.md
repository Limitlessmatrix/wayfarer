# Wayfarer emulator and DLC project

Goal: run the SAINTCON 2026 badge game (Orion Trail, firmware name Wayfarer) on the PC, then extend it
with expansions, DLC, quests and story lines that run both in the emulator and on the real badge.

## What exists already

- Stock firmware source, MIT, by Professor Plum: https://github.com/Professor-plum/SAINTCON-BADGE-2026
  (commit 2168f62). Verified against the shipped 0.1 image: all 66 event titles and all 759 event
  strings match byte for byte. Vendored in `upstream/firmware`.
- Host platform layer, MIT, from PugTechnology's badge museum
  (https://github.com/PugTechnology/saintcon-badge-museum, `emulator/cores/2026/platform`): stub
  Pico SDK headers plus C replacements for the ST77916 display, PIO WS2812, I2C EEPROM, GPIO buttons,
  timers, RNG and USB console. The unmodified firmware compiles with host gcc or Emscripten.
  Vendored in `platform/`. The museum also ships a prebuilt browser build.
- Content pipeline in the firmware: `events.yml` -> `generate_events.py` -> `generated_events.c`.

## Architecture

```
upstream/firmware/   stock game sources (we will edit these for new mechanics)
platform/            host shim: platform_2026.c, pico_stubs/, cli_2026.c (headless runner)
tools/host_extra.c   RGB888 helper exported from the shared library
tools/wayfarer.py    desktop frontend: pygame window, keyboard, LEDs, EEPROM persistence, console
build/libwayfarer.so core ABI: emu_init, emu_step, emu_framebuffer, emu_leds, emu_eeprom ...
build/emu-cli        scripted headless runs with PNG screenshots (regression tests for DLC)
```

Same tree builds the real badge UF2 with `make badge` (pico-sdk in ~/badge/tools, `source ~/badge/env.sh`).

Core ABI (from platform_2026.h): `emu_init(seed)`, `emu_step(dt_ms, buttons)` runs the game fiber
until virtual time advances `dt_ms`; buttons bits are up, down, left, right, centre, B, A (bits 0 to 6);
`emu_framebuffer()` 360x360 RGB565; `emu_leds()` 9 x RGB; `emu_eeprom()` 1024 bytes, load before
`emu_init`, poll `emu_eeprom_dirty()` to persist; `platform_2026_usb_push_str()` feeds the USB console
(HELP, IDENT, HANDLE, UNLOCK, uppercase only).

## Game model (from game.c, docking.c, config.c)

- Hex map. Home station at 0,0. 9 POIs (outposts) at distance 10 down to 4 with market rates
  `550 + rand(50) - dist*50`. One nebula tile and 6 asteroid tiles (3 pairs), each with a minigame.
- Ship: hull (max 3), fuel (max 5), cargo hold 24, credits `1000 + 150 * achievements`.
  Home market sells upgrades.
- Empty tile: 1 in 3 chance of a random event drawn from a shuffled list of the 66 events.
  Event = title, description, up to 4 options (optional role requirement with count), each with up to
  4 weighted outcomes, each with up to 4 effects (hull, fuel, credits, crew, cargo, reputation, xp, upgrade).
- Crew: up to 5, roles Pilot, Engineer, Scientist, Soldier, Medic. Offline mode (no dock) generates 5.
- Return home with cargo = victory, score = cargo*300 + reputation + xp + crew*10, saved as a flight log.
- Game over on hull 0, fuel 0, crew 0, or home with no cargo and under 400 credits.
- 32 achievements as a bitmask in EEPROM (config.c), two of them tied to specific events
  (Relics of the First Builders, The Obsidian Monolith).

## DLC design (next)

1. Keep `events.yml` as the content format. Add a `packs/` directory, one YAML per expansion, merged by
   an extended generator (`tools/build_content.py`) into `generated_events.c`. Stock content stays a pack.
2. Quests and story lines need state the stock engine lacks. Add to `game_state_t`:
   `uint32_t story_flags` and a small `quest[]` table. New YAML fields on events and options:
   `requires_flag`, `sets_flag`, `clears_flag`, `once` (fire at most once per run), `at` (only on a given
   POI or tile type), `chain` (next event id forced on the next empty tile). This gives multi step quests
   with branching, all data driven.
3. Named POIs and outposts as data (today they are anonymous markets): `pois.yml` with name, flavor
   text, market bias, exclusive events.
4. New effect types as they are needed (crew role gain, upgrade grant, reputation thresholds).
5. Regression: `emu-cli` scripts with fixed seeds, screenshots diffed in CI style, so a pack never
   breaks the stock game.
6. Flash to the badge from the same tree; EEPROM record format untouched so achievements survive.

## Status 2026-11-04

Research done, project laid out, frontend written. Not yet compiled: the harness denied compiling the
vendored (downloaded) sources. Run `make` and `make cli` by hand once, after that iteration is normal.
