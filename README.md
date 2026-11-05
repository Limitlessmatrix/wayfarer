# Wayfarer: SAINTCON 2026 badge game on the PC, plus expansions

Stock Orion Trail firmware (MIT, Professor Plum) built against a host platform layer (MIT, PugTechnology
badge museum). Same tree builds the real badge.

    make                      # build/libwayfarer.so
    source ../.venv/bin/activate   # or: source ~/badge/env.sh
    tools/wayfarer.py --log   # desktop window, EEPROM persists in save/eeprom.bin
    tools/wayfarer.py --import ../backup/20261028-111412/eeprom.bin   # start from your real badge profile, saves to save/eeprom.bin

    make cli && build/emu-cli --script tests/smoke.script --out save/smoke --verbose   # headless regression

    make events               # events.yml -> generated_events.c
    source ~/badge/env.sh && make badge     # Wayfarer.uf2 for the badge

Keys: arrows or WASD, Enter/Space/E = centre, Z = A, X = B, F1 fast, F5 save EEPROM, F12 screenshot.
Touch pad style: the round screen behaves like a phone. Tap a menu row, market button, event option or
map hex to act on it, drag the map to pan, tap the HUD for the ship menu, tap arrows in the signal lock
and around the maze. Right click = B, middle = centre, wheel = up/down; the side panel also works. Taps
no screen claims become an A press after 150 ms (popups, title, shooter). The joystick press is the
third button: it opens the ship menu and services and selects inside every menu and event.
Pointer input lives in upstream/firmware/touch.c; on the badge it reports nothing.
Console commands (HELP, IDENT, HANDLE, UNLOCK) are read from stdin.

## Multiplayer (emulator to emulator)

    tools/wayfarer.py --join NEW  --server wss://wayfarer-relay.nrp-nautilus.io   # prints a 4 letter room code
    tools/wayfarer.py --join ABCD --server wss://wayfarer-relay.nrp-nautilus.io   # friends join with the code
    tools/relay.py --port 8766                                   # or run your own relay (ws://host:8766)

The public relay runs on NRP (deploy/relay.yaml, namespace saintcon, python:3.12-slim with relay.py
from a ConfigMap, haproxy ingress with TLS). Redeploy after editing relay.py:
`kubectl -n saintcon create configmap wayfarer-relay --from-file=relay.py=tools/relay.py --dry-run=client -o yaml | kubectl apply -f - && kubectl -n saintcon rollout restart deploy/wayfarer-relay`

Everyone in a room flies the same galaxy (the relay owns the map seed). Other ships appear on the map
as ghost rockets with a ring in their class colour and their handle; centre press on the map opens the
fleet list (credits, cargo, hull, fuel, finished scores). Protocol lines start with `$MP` and ride the
dock UART channel, so the same code works badge to badge over the dock connector. Headless regression:

    tools/headless.py --script tests/mp_a.script --join T1 --name Alpha --seed 1001 --realtime &
    tools/headless.py --script tests/mp_b.script --join T1 --name Bravo --seed 2002 --realtime

## Frontier expansion (goods, gathering, repairs, contracts, story)

Built on top of the stock game, see docs/FRONTIER.md for the design:

- Six goods in a typed hold (Supplies, Ore, Gas, Alloy, Parts, Relic), coloured crates in the HUD,
  victory score counts each by value.
- Gathering: asteroids pay Ore, the nebula pays Gas, three derelict wrecks per map with a timed
  maze crawl for Parts, Alloy or a Relic.
- Ship menu (centre press on the map): repair hull with Parts, burn Gas for fuel, refine Ore into
  Alloy, machine Alloy into Parts, contracts, fleet.
- Outposts: centre press opens services: trade goods, job board, and at the three outposts marked
  with a green wrench on the map, a shipyard (fuel tank, hull armor, cargo bay up to 32, paid repair,
  engineer discount) usable any time during a run. Home: centre press on the shipyard screen opens
  station jobs and the same shipyard. Contracts: deliver, collect, salvage, courier.
- Home station: flying home now docks (refuel and repair are free) and opens a dock menu: complete
  the mission by turning in the hold, shipyard, station job board, or depart and keep flying.
  Starting credits are 3000 plus 50 per achievement.
- Mission unlocked modules: completed missions persist in the EEPROM record (the old unused byte)
  and unlock modules at any shipyard: Mining rig (1 mission, ore x2), Long tanks (2, +5 fuel),
  Shield plate (3, +2 hull), Smuggler hold (5, +8 bay). Bought per run.
- Exotic fuel: two outposts per map are fuel depots (orange flame badge on the map). Reaching one
  unlocks the persistent Exotic burn perk (stored next to the mission count in the EEPROM record).
  Depots sell He-3 ice (burn 1 for +3 fuel) and dark matter (burn 1 for a full tank) from the ship
  menu; with the perk the nebula also skims He-3 and wrecks sometimes hold dark matter.
- Minigames: asteroid tiles ask blast (shooter) or drill (rhythm bar, A when the bit hits green,
  8 beats, 3 slips end it, each hit is an ore chunk). Wreck cores are sealed behind a signal lock:
  watch a growing d-pad pattern echoed on the badge LEDs, repeat it, three rounds; failing leaves
  only a panel of Parts. Host test hooks: `DBG SIGNAL n`, `DBG DRILL`, `DBG ROCK n`, `DBG CLOUD`.
- Story engine: events in `upstream/firmware/packs/*.yml` can require or forbid flags, fire once,
  attach to a tile kind (empty, poi, home, derelict), chain into a follow up, and grant goods or flags.
  `make events` merges the stock `events.yml` with every pack. First pack: `packs/first_builders.yml`.

Scripted walkthroughs (screenshots land in save/): `tests/frontier.script`, `tests/jobs_story.script`,
`tests/ghost.script`. The host build has `DBG` console hooks (TELEPORT, POI, WRECK, GIVE, CREDITS,
HULL, FUEL, FLAGS, CHAIN, LOOT) that never compile into the badge image.

See docs/PLAN.md for the game model and docs/EXPANSIONS.md for the expansion brainstorm.
