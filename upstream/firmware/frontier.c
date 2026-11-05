#include "frontier.h"
#include "game.h"
#include "multiplayer.h"
#include "framebuffer.h"
#include "buttons.h"
#include "ui.h"
#include "bsp/config.h"
#include "hexutils.h"
#include "eventbase.h"
#include "img/frontier_img.h"
#include "touch.h"
#include "pico/rand.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

extern game_state_t state;
extern game_config_t config;
extern game_character_t crew[5];
extern int crew_count;
extern const int hex_size;
extern const char* poi_names[10];
extern Point screen_pos;

const char* good_names[GOOD_COUNT] = {"Supplies", "Ore", "Gas", "Alloy", "Parts", "Relic", "He-3 ice", "Dark matter"};
const int good_value[GOOD_COUNT]   = {300, 150, 150, 450, 200, 1000, 350, 900};
const uint16_t good_colors[GOOD_COUNT] = {
    RGB565(184, 171, 138), RGB565(150, 110, 70), RGB565(90, 200, 220),
    RGB565(200, 200, 215), RGB565(230, 170, 60), RGB565(200, 90, 230),
    RGB565(120, 230, 150), RGB565(120, 40, 170)};
static const int good_base_price[GOOD_COUNT] = {0, 120, 120, 400, 180, 0, 160, 650};

static bool is_depot(int poi) { return (state.depot_mask >> poi) & 1; }
static bool has_exotic_perk(void) { return config_get_perks() & PERK_EXOTIC; }

static void yield_(void) { sleep_ms(1); }

// Half width of the round panel at text baseline y (text sits ~12 px above the baseline).
int round_half_width(int y) {
    int dy = (y - 6) - LCD_HEIGHT / 2;
    int r = LCD_WIDTH / 2 - 4;
    int d = r * r - dy * dy;
    if (d <= 0) return 0;
    int h = 0;
    while ((h + 1) * (h + 1) <= d) h++;
    return h;
}

bool frontier_has_role(char role) {
    for (int i = 0; i < crew_count; ++i) if (crew[i].specialty == role) return true;
    return false;
}

void frontier_sync_cargo(void) {
    int t = 0;
    for (int i = 0; i < GOOD_COUNT; ++i) t += state.hold[i];
    state.cargo = t;
}

int frontier_add_good(int g, int n) {
    if (g < 0 || g >= GOOD_COUNT || n <= 0) return 0;
    frontier_sync_cargo();
    int space = config.max_cargo - state.cargo;
    if (n > space) n = space;
    if (n < 0) n = 0;
    state.hold[g] += n;
    frontier_sync_cargo();
    return n;
}

int frontier_take_good(int g, int n) {
    if (g < 0 || g >= GOOD_COUNT || n <= 0) return 0;
    if (n > state.hold[g]) n = state.hold[g];
    state.hold[g] -= n;
    frontier_sync_cargo();
    return n;
}

int frontier_score(void) {
    int s = 0;
    for (int i = 0; i < GOOD_COUNT; ++i) s += state.hold[i] * good_value[i];
    return s;
}

int frontier_take_any(int n) {
    int taken = 0;
    for (int g = 0; g < GOOD_COUNT && taken < n; ++g) taken += frontier_take_good(g, n - taken);
    return taken;
}

int frontier_choose(const char* text) {
    buttons_clear();
    ui_draw_popup(text);
    while (true) {
        int b = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) b |= (tx > 240) ? BUTTON_B : BUTTON_A;
        }
        if (b & KEY_SELECT) { buttons_clear(); return BUTTON_A; }
        if (b & BUTTON_B) { buttons_clear(); return BUTTON_B; }
        mp_tick();
        sleep_ms(10);
    }
}

void frontier_gather_drill(int hits) {
    int ore = hits;
    if (frontier_has_role(CREW_SPECIALTY_ENGINEER)) ore += ore / 2;
    if (state.modules & MOD_MINING) ore *= 2;
    if (ore <= 0) return;
    int got = frontier_add_good(GOOD_ORE, ore);
    char msg[80];
    snprintf(msg, sizeof(msg), got < ore ? "Drilled %d Ore\n(hold full, %d left behind)" : "Drilled %d Ore", got, ore - got);
    frontier_popup_wait(msg);
}

void frontier_popup_wait(const char* text) {
    int tx, ty;
    buttons_clear();
    ui_draw_popup(text);
    while (buttons_get_and_clear() == 0 && !ui_take_tap(&tx, &ty)) {
        mp_tick();
        sleep_ms(10);
    }
}

// ---------------------------------------------------------------- goods page at an outpost

static int current_poi = 0;
static int buy_price(int g, int rate) {
    if (g == GOOD_SUPPLIES) return rate;
    if (g == GOOD_RELIC) return 0;
    if ((g == GOOD_HE3 || g == GOOD_DARK) && !is_depot(current_poi)) return 0;
    int p = good_base_price[g] * rate / 500;
    return p < 10 ? 10 : p;
}

static void draw_goods(int poi_idx, int rate, int sel) {
    char line[48];
    frame_clear();
    frame_draw_text(poi_names[poi_idx], title_color, 24);
    frame_draw_string_centered(46, "TRADE GOODS", description_color);
    frame_draw_string(44, 68, "good", description_color);
    frame_draw_string(168, 68, "held", description_color);
    frame_draw_string(214, 68, "buy", description_color);
    frame_draw_string(276, 68, "sell", description_color);
    for (int g = 0; g < GOOD_COUNT; ++g) {
        int y = 88 + g * 26;
        if (g == sel) frame_fill_rect(36, y - 17, 290, 23, sbackground_color);
        frame_draw_image(38, y - 13, good_icons[g], 16, 16);
        frame_draw_string(58, y, good_names[g], g == sel ? selected_color : option_color);
        snprintf(line, sizeof(line), "%d", state.hold[g]);
        frame_draw_string(172, y, line, option_color);
        int bp = buy_price(g, rate);
        if (bp > 0) {
            snprintf(line, sizeof(line), "$%d", bp);
            frame_draw_string(214, y, line, state.credits >= bp ? option_color : unavailable_color);
            snprintf(line, sizeof(line), "$%d", bp * 3 / 4);
            frame_draw_string(276, y, line, state.hold[g] > 0 ? option_color : unavailable_color);
        } else {
            frame_draw_string(214, y, (g == GOOD_HE3 || g == GOOD_DARK) ? "depots only" : "not traded", unavailable_color);
        }
    }
    frame_draw_string_centered(300, "< sell     buy >     B back", description_color);
    game_draw_ui();
    frame_refresh();
}

void frontier_run_goods_market(int poi_idx) {
    current_poi = poi_idx;
    int sel = 1;
    buttons_clear();
    while (true) {
        int rate = state.poi_market_rate[poi_idx];
        draw_goods(poi_idx, rate, sel);
        uint32_t keys = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) {
                int r = ui_tap_row(ty, 88, 26, GOOD_COUNT);
                if (r >= 0) {
                    sel = r;
                    if (tx >= 206 && tx < 270) keys |= BUTTON_RIGHT;      // buy column
                    else if (tx >= 270) keys |= BUTTON_LEFT;              // sell column
                } else if (ty > 285) keys |= BUTTON_B;
            }
        }
        if (keys & BUTTON_UP) sel = (sel + GOOD_COUNT - 1) % GOOD_COUNT;
        if (keys & BUTTON_DOWN) sel = (sel + 1) % GOOD_COUNT;
        int bp = buy_price(sel, rate);
        if ((keys & BUTTON_RIGHT) && bp > 0 && state.credits >= bp) {
            if (frontier_add_good(sel, 1)) {
                state.credits -= bp;
                if (sel == GOOD_SUPPLIES) state.poi_market_rate[poi_idx] += 1 + rate * 0.15;
                else state.poi_market_rate[poi_idx] += 1 + rate * 0.03;
            }
        }
        if ((keys & BUTTON_LEFT) && bp > 0 && state.hold[sel] > 0) {
            frontier_take_good(sel, 1);
            state.credits += bp * 3 / 4;
            if (sel == GOOD_SUPPLIES) state.poi_market_rate[poi_idx] -= rate * 0.10;
            else state.poi_market_rate[poi_idx] -= rate * 0.02;
            if (state.poi_market_rate[poi_idx] < 1) state.poi_market_rate[poi_idx] = 1;
        }
        if (keys & BUTTON_B) break;
        mp_tick();
        yield_();
    }
    buttons_clear();
}

// ---------------------------------------------------------------- ship menu on the map

enum { SM_REPAIR, SM_REFUEL, SM_HE3, SM_DARK, SM_REFINE, SM_PARTS, SM_CONTRACTS, SM_FLEET, SM_BACK, SM_COUNT };

static void draw_ship_menu(const char* labels[], const bool enabled[], int n, int sel) {
    char line[64];
    frame_clear();
    frame_draw_text("SHIP", title_color, 24);
    // hold as icon + count pairs, centred, one row
    int shown = 0;
    for (int g = 0; g < GOOD_COUNT; ++g) if (state.hold[g]) shown++;
    if (!shown) frame_draw_string_centered(52, "hold empty", description_color);
    else {
        int cell = 44, x = LCD_WIDTH / 2 - shown * cell / 2;
        for (int g = 0; g < GOOD_COUNT; ++g) {
            if (!state.hold[g]) continue;
            frame_draw_image(x, 38, good_icons[g], 16, 16);
            snprintf(line, sizeof(line), "%d", state.hold[g]);
            frame_draw_string(x + 19, 52, line, description_color);
            x += cell;
        }
    }
    snprintf(line, sizeof(line), "hull %d/%d   fuel %d/%d", state.hull, config.max_hull, state.fuel, config.max_fuel);
    frame_draw_string_centered(74, line, description_color);
    for (int i = 0; i < n; ++i) {
        int y = 102 + i * 25;
        int hw = round_half_width(y) - 12;
        if (i == sel) frame_fill_rect(LCD_WIDTH / 2 - hw, y - 17, 2 * hw, 23, sbackground_color);
        frame_draw_string_centered(y, labels[i], !enabled[i] ? unavailable_color : (i == sel ? selected_color : option_color));
    }
    game_draw_ui();
    frame_refresh();
}

void frontier_run_ship_menu(void) {
    char lrepair[40], lrefuel[40], lrefine[40], lparts[40], lhe3[40], ldark[40];
    const char* labels[SM_COUNT];
    bool enabled[SM_COUNT];
    int sel = 0;
    buttons_clear();
    while (true) {
        bool eng = frontier_has_role(CREW_SPECIALTY_ENGINEER), sci = frontier_has_role(CREW_SPECIALTY_SCIENTIST);
        snprintf(lrepair, sizeof(lrepair), "Repair hull  1 Parts -> +%d", eng ? 2 : 1);
        snprintf(lrefuel, sizeof(lrefuel), "Refuel  1 Gas -> +1 fuel");
        snprintf(lrefine, sizeof(lrefine), "Refine  2 Ore + 1 Gas -> Alloy");
        snprintf(lparts, sizeof(lparts), "Make parts  1 Alloy -> 2 Parts");
        bool perk = has_exotic_perk();
        snprintf(lhe3, sizeof(lhe3), perk ? "Burn He-3  1 -> +3 fuel" : "Burn He-3  (find a fuel depot)");
        snprintf(ldark, sizeof(ldark), perk ? "Burn dark matter  1 -> full tank" : "Burn dark matter  (find a fuel depot)");
        labels[SM_HE3] = lhe3; labels[SM_DARK] = ldark;
        enabled[SM_HE3] = perk && state.hold[GOOD_HE3] > 0 && state.fuel < config.max_fuel;
        enabled[SM_DARK] = perk && state.hold[GOOD_DARK] > 0 && state.fuel < config.max_fuel;
        labels[SM_REPAIR] = lrepair; labels[SM_REFUEL] = lrefuel; labels[SM_REFINE] = lrefine;
        labels[SM_PARTS] = lparts; labels[SM_CONTRACTS] = "Contracts"; labels[SM_FLEET] = "Fleet"; labels[SM_BACK] = "Back";
        enabled[SM_REPAIR] = state.hold[GOOD_PARTS] > 0 && state.hull < config.max_hull;
        enabled[SM_REFUEL] = state.hold[GOOD_GAS] > 0 && state.fuel < config.max_fuel;
        enabled[SM_REFINE] = state.hold[GOOD_ORE] >= 2 && state.hold[GOOD_GAS] >= 1 && (eng || sci);
        enabled[SM_PARTS] = state.hold[GOOD_ALLOY] >= 1;
        enabled[SM_CONTRACTS] = true;
        enabled[SM_FLEET] = mp_active;
        enabled[SM_BACK] = true;
        draw_ship_menu(labels, enabled, SM_COUNT, sel);
        uint32_t keys = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) {
                int r = ui_tap_row(ty, 102, 25, SM_COUNT);
                if (r >= 0) { sel = r; keys |= BUTTON_A; }
            }
        }
        if (keys & BUTTON_UP) sel = (sel + SM_COUNT - 1) % SM_COUNT;
        if (keys & BUTTON_DOWN) sel = (sel + 1) % SM_COUNT;
        if (keys & BUTTON_B) break;
        if ((keys & KEY_SELECT) && enabled[sel]) {
            switch (sel) {
            case SM_REPAIR:
                frontier_take_good(GOOD_PARTS, 1);
                state.hull += eng ? 2 : 1;
                if (state.hull > config.max_hull) state.hull = config.max_hull;
                frontier_popup_wait(eng ? "Engineer patches the hull.\nHull +2" : "Hull patched.\nHull +1");
                break;
            case SM_REFUEL:
                frontier_take_good(GOOD_GAS, 1);
                state.fuel += 1;
                frontier_popup_wait("Gas cell burned.\nFuel +1");
                break;
            case SM_HE3:
                frontier_take_good(GOOD_HE3, 1);
                state.fuel += 3;
                if (state.fuel > config.max_fuel) state.fuel = config.max_fuel;
                frontier_popup_wait("He-3 ice flashed in the drive.\nFuel +3");
                break;
            case SM_DARK:
                frontier_take_good(GOOD_DARK, 1);
                state.fuel = config.max_fuel;
                frontier_popup_wait("Dark matter annihilates in the core.\nTank full");
                break;
            case SM_REFINE:
                frontier_take_good(GOOD_ORE, 2);
                frontier_take_good(GOOD_GAS, 1);
                frontier_add_good(GOOD_ALLOY, 1);
                frontier_popup_wait("Ore smelted under gas flame.\nAlloy +1");
                break;
            case SM_PARTS:
                frontier_take_good(GOOD_ALLOY, 1);
                frontier_add_good(GOOD_PARTS, 2);
                frontier_popup_wait("Alloy machined.\nParts +2");
                break;
            case SM_CONTRACTS:
                contracts_show_summary();
                break;
            case SM_FLEET:
                mp_show_fleet();
                break;
            case SM_BACK:
                buttons_clear();
                return;
            }
        }
        mp_tick();
        yield_();
    }
    buttons_clear();
}

// ---------------------------------------------------------------- gathering

void frontier_gather_asteroid(int score) {
    int ore = score / 30;
    if (frontier_has_role(CREW_SPECIALTY_ENGINEER)) ore += ore / 2;
    if (state.modules & MOD_MINING) ore *= 2;
    if (ore <= 0) return;
    int got = frontier_add_good(GOOD_ORE, ore);
    char msg[80];
    snprintf(msg, sizeof(msg), got < ore ? "Mined %d Ore\n(hold full, %d left behind)" : "Mined %d Ore", got, ore - got);
    frontier_popup_wait(msg);
}

void frontier_gather_nebula(void) {
    int gas = 2 + (frontier_has_role(CREW_SPECIALTY_SCIENTIST) ? 1 : 0);
    int got = frontier_add_good(GOOD_GAS, gas);
    int he3 = has_exotic_perk() ? frontier_add_good(GOOD_HE3, 1) : 0;
    char msg[80];
    if (he3) snprintf(msg, sizeof(msg), "Harvested %d Gas\nand skimmed 1 He-3 ice", got);
    else snprintf(msg, sizeof(msg), got < gas ? "Harvested %d Gas\n(hold full)" : "Harvested %d Gas", got);
    frontier_popup_wait(msg);
}

// ---------------------------------------------------------------- derelicts

void frontier_draw_derelicts(int x, int y) {
    for (int i = 0; i < DERELICT_COUNT; ++i) {
        Point at = hex_to_pixel(state.derelicts[i], hex_size);
        bool looted = (state.derelict_looted >> i) & 1;
        frame_draw_image(x + at.x - WRECK_W / 2, y + at.y - WRECK_H / 2, looted ? scrap_image_data : wreck_image_data, WRECK_W, WRECK_H);
    }
}

void frontier_run_derelict(void) {
    int idx = -1;
    for (int i = 0; i < DERELICT_COUNT; ++i) if (hex_equal(state.derelicts[i], state.player)) idx = i;
    if (idx < 0) return;
    if ((state.derelict_looted >> idx) & 1) {
        frontier_popup_wait("Only scrap remains here.");
        return;
    }
    buttons_clear();
    ui_draw_popup("A derelict hulk drifts here.\nBoard it and reach the core?\nA: board   B: leave");
    int keys;
    while (((keys = buttons_get_and_clear()) & (BUTTON_A | BUTTON_B)) == 0) { mp_tick(); sleep_ms(10); }
    if (keys & BUTTON_B) return;
    int won = maze_play(60000);
    state.derelict_looted |= (1 << idx);
    if (won) contracts_on_salvage(idx);
    if (!won) {
        state.hull -= 1;
        frontier_popup_wait("The wreck shifts and crushes a strut.\nHull -1");
        return;
    }
    frontier_popup_wait("The core vault answers to an old beacon code.\nDecode the signal to open it.");
    if (!signal_lock_play(3, 3)) {
        int n = frontier_add_good(GOOD_PARTS, 1);
        frontier_popup_wait(n ? "The vault stays sealed.\nYou strip a panel instead.\nParts +1" : "The vault stays sealed\nand the hold is full.");
        return;
    }
    int roll = get_rand_32() % 100;
    if (roll < 50) {
        int n = frontier_add_good(GOOD_PARTS, 2);
        frontier_popup_wait(n ? "Salvaged spare parts.\nParts +2" : "Spare parts found,\nbut the hold is full.");
    } else if (roll < 80) {
        int n = frontier_add_good(GOOD_ALLOY, 1);
        frontier_popup_wait(n ? "Hull plating recovered.\nAlloy +1" : "Plating found,\nbut the hold is full.");
    } else if (roll < 92) {
        int n = frontier_add_good(GOOD_RELIC, 1);
        frontier_popup_wait(n ? "An alien relic glows in the core.\nRelic +1" : "A relic glows here,\nbut the hold is full.");
    } else if (roll < 97) {
        int n = frontier_add_good(GOOD_DARK, 1);
        frontier_popup_wait(n ? "A containment flask still hums.\nDark matter +1" : "A flask of dark matter,\nbut the hold is full.");
    } else {
        frontier_popup_wait("The core was stripped long ago.\nNothing here.");
    }
}

// ---------------------------------------------------------------- services menu (outposts and home)

bool frontier_has_shipyard(int board) {
    if (board == HOME_BOARD) return true;
    return (state.shipyard_mask >> board) & 1;
}

void frontier_draw_poi_marks(int x, int y) {
    for (int i = 0; i < POI_COUNT; ++i) {
        if (!((state.shipyard_mask >> i) & 1)) continue;
        Point at = hex_to_pixel(state.poi[i], hex_size);
        frame_draw_image(x + at.x + 8, y + at.y - 22, icon_shipyard_image_data, ICON_SHIPYARD_W, ICON_SHIPYARD_H);
    }
    for (int i = 0; i < POI_COUNT; ++i) {
        if (!is_depot(i)) continue;
        Point at = hex_to_pixel(state.poi[i], hex_size);
        frame_draw_image(x + at.x - 20, y + at.y - 22, icon_depot_image_data, ICON_DEPOT_W, ICON_DEPOT_H);
    }
}

void frontier_on_arrive_poi(int poi_idx) {
    if (!is_depot(poi_idx)) return;
    if (has_exotic_perk()) return;
    config_add_perk(PERK_EXOTIC);
    frontier_popup_wait("PERK UNLOCKED: Exotic burn\nThe fuelers here show you how\nto burn He-3 ice and dark matter.\nThis depot sells both.");
}

static int yard_hull_cost(void) { return 30 * (config.max_hull + 1); }
static int yard_fuel_cost(void) { return 20 * (config.max_fuel + 1); }
static int yard_cargo_cost(void) { return 50 * (config.max_cargo / 4); }
static int yard_repair_cost(void) { return 40 * (config.max_hull - state.hull); }

// Mission unlocked modules: bought per run at any shipyard once enough missions are on record.
static const struct { uint8_t bit; uint8_t missions; int cost; const char* name; const char* done; } modules[4] = {
    {MOD_MINING, 1,  600, "Mining rig  ore x2",     "Mining rig bolted to the bow.\nAsteroids yield double Ore."},
    {MOD_TANKS,  2,  800, "Long tanks  +5 fuel",    "Auxiliary tanks fitted.\nFuel capacity +5."},
    {MOD_SHIELD, 3, 1000, "Shield plate  +2 hull",  "Shield plating installed.\nHull capacity +2."},
    {MOD_HOLD,   5, 1200, "Smuggler hold  +8 bay",  "Hidden hold cut into the frame.\nCargo bay +8."},
};

void frontier_run_shipyard(int board) {
    enum { Y_FUEL, Y_HULL, Y_CARGO, Y_REPAIR, Y_MOD0, Y_BACK = Y_MOD0 + 4, Y_COUNT };
    char lbl[Y_COUNT][40];
    bool en[Y_COUNT];
    int sel = 0;
    bool eng = frontier_has_role(CREW_SPECIALTY_ENGINEER);
    int missions = config_get_missions();
    buttons_clear();
    while (true) {
        int disc = eng ? 80 : 100;   // engineers haggle 20 percent off
        int cf = yard_fuel_cost() * disc / 100, ch = yard_hull_cost() * disc / 100;
        int cc = yard_cargo_cost() * disc / 100, cr = yard_repair_cost() * disc / 100;
        snprintf(lbl[Y_FUEL], 40, "Fuel tank %d -> %d   $%d", config.max_fuel, config.max_fuel + 1, cf);
        snprintf(lbl[Y_HULL], 40, "Hull armor %d -> %d   $%d", config.max_hull, config.max_hull + 1, ch);
        snprintf(lbl[Y_CARGO], 40, "Cargo bay %d -> %d   $%d", config.max_cargo, config.max_cargo + 4, cc);
        if (state.hull < config.max_hull) snprintf(lbl[Y_REPAIR], 40, "Repair hull to %d   $%d", config.max_hull, cr);
        else snprintf(lbl[Y_REPAIR], 40, "Hull intact");
        snprintf(lbl[Y_BACK], 40, "Back");
        en[Y_FUEL] = config.max_fuel < 15 && state.credits >= cf;
        en[Y_HULL] = config.max_hull < 8 && state.credits >= ch;
        en[Y_CARGO] = config.max_cargo < 32 && state.credits >= cc;
        en[Y_REPAIR] = state.hull < config.max_hull && state.credits >= cr && cr > 0;
        for (int m = 0; m < 4; ++m) {
            int cost = modules[m].cost * disc / 100;
            if (state.modules & modules[m].bit) { snprintf(lbl[Y_MOD0 + m], 40, "%s  installed", modules[m].name); en[Y_MOD0 + m] = false; }
            else if (missions < modules[m].missions) { snprintf(lbl[Y_MOD0 + m], 40, "%s  [%d missions]", modules[m].name, modules[m].missions); en[Y_MOD0 + m] = false; }
            else { snprintf(lbl[Y_MOD0 + m], 40, "%s  $%d", modules[m].name, cost); en[Y_MOD0 + m] = state.credits >= cost; }
        }
        en[Y_BACK] = true;
        frame_clear();
        frame_draw_text(board == HOME_BOARD ? "SHIPYARD" : poi_names[board], title_color, 24);
        char line[48];
        snprintf(line, sizeof(line), "hull %d/%d   fuel %d/%d   bay %d", state.hull, config.max_hull, state.fuel, config.max_fuel, config.max_cargo);
        frame_draw_string_centered(50, line, description_color);
        snprintf(line, sizeof(line), eng ? "missions %d   engineer discount" : "missions %d", missions);
        frame_draw_string_centered(68, line, eng ? RGB565(80, 220, 120) : description_color);
        for (int i = 0; i < Y_COUNT; ++i) {
            int y = 92 + i * 24;
            int hw = round_half_width(y) - 10;
            if (i == sel) frame_fill_rect(LCD_WIDTH / 2 - hw, y - 17, 2 * hw, 22, sbackground_color);
            frame_draw_string_centered(y, lbl[i], !en[i] ? unavailable_color : (i == sel ? selected_color : option_color));
        }
        frame_refresh();
        uint32_t keys = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) {
                int r = ui_tap_row(ty, 92, 24, Y_COUNT);
                if (r >= 0) { sel = r; keys |= BUTTON_A; }
            }
        }
        if (keys & BUTTON_UP) sel = (sel + Y_COUNT - 1) % Y_COUNT;
        if (keys & BUTTON_DOWN) sel = (sel + 1) % Y_COUNT;
        if (keys & BUTTON_B) break;
        if ((keys & KEY_SELECT) && en[sel]) {
            switch (sel) {
            case Y_FUEL: state.credits -= cf; config.max_fuel++; state.fuel = config.max_fuel; frontier_popup_wait("Larger tank fitted and filled."); break;
            case Y_HULL: state.credits -= ch; config.max_hull++; state.hull = config.max_hull; frontier_popup_wait("Armor plating welded on.\nHull restored."); break;
            case Y_CARGO: state.credits -= cc; config.max_cargo += 4; frontier_popup_wait("Cargo bay extended.\n+4 crates"); break;
            case Y_REPAIR: state.credits -= cr; state.hull = config.max_hull; frontier_popup_wait("Hull fully repaired."); break;
            case Y_BACK: buttons_clear(); return;
            default: {
                int m = sel - Y_MOD0;
                state.credits -= modules[m].cost * disc / 100;
                state.modules |= modules[m].bit;
                if (modules[m].bit == MOD_TANKS) { config.max_fuel += 5; state.fuel = config.max_fuel; }
                if (modules[m].bit == MOD_SHIELD) { config.max_hull += 2; state.hull = config.max_hull; }
                if (modules[m].bit == MOD_HOLD) config.max_cargo += 8;
                frontier_popup_wait(modules[m].done);
                break;
            }
            }
        }
        mp_tick();
        yield_();
    }
    buttons_clear();
}

// Docked at the home station mid run: turn in, refit, jobs, or depart.
int frontier_run_home_dock(void) {
    enum { D_TURNIN, D_YARD, D_JOBS, D_DEPART, D_COUNT };
    char turnin[48];
    const char* labels[D_COUNT];
    bool en[D_COUNT];
    int sel = state.cargo > 0 ? D_TURNIN : D_DEPART;
    buttons_clear();
    while (true) {
        frontier_sync_cargo();
        int value = frontier_score();
        if (state.cargo > 0) snprintf(turnin, sizeof(turnin), "Complete mission: %d crates, %d pts", state.cargo, value);
        else snprintf(turnin, sizeof(turnin), "Complete mission: hold is empty");
        labels[D_TURNIN] = turnin; labels[D_YARD] = "Shipyard: upgrades and repair"; labels[D_JOBS] = "Station job board"; labels[D_DEPART] = "Depart";
        en[D_TURNIN] = state.cargo > 0; en[D_YARD] = true; en[D_JOBS] = true; en[D_DEPART] = true;
        frame_clear();
        frame_draw_text("HOME STATION", title_color, 24);
        frame_draw_string_centered(50, "refueled, repaired", RGB565(80, 220, 120));
        int shown = 0;
        for (int g = 0; g < GOOD_COUNT; ++g) if (state.hold[g]) shown++;
        if (!shown) frame_draw_string_centered(78, "hold empty", description_color);
        else {
            char line[8];
            int cell = 44, x = LCD_WIDTH / 2 - shown * cell / 2;
            for (int g = 0; g < GOOD_COUNT; ++g) {
                if (!state.hold[g]) continue;
                frame_draw_image(x, 64, good_icons[g], 16, 16);
                snprintf(line, sizeof(line), "%d", state.hold[g]);
                frame_draw_string(x + 19, 78, line, description_color);
                x += cell;
            }
        }
        for (int i = 0; i < D_COUNT; ++i) {
            int y = 120 + i * 28;
            int hw = round_half_width(y) - 12;
            if (i == sel) frame_fill_rect(LCD_WIDTH / 2 - hw, y - 17, 2 * hw, 23, sbackground_color);
            frame_draw_string_centered(y, labels[i], !en[i] ? unavailable_color : (i == sel ? selected_color : option_color));
        }
        game_draw_ui();
        frame_refresh();
        uint32_t keys = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) {
                int r = ui_tap_row(ty, 120, 28, D_COUNT);
                if (r >= 0) { sel = r; keys |= BUTTON_A; }
            }
        }
        if (keys & BUTTON_UP) sel = (sel + D_COUNT - 1) % D_COUNT;
        if (keys & BUTTON_DOWN) sel = (sel + 1) % D_COUNT;
        if (keys & BUTTON_B) { buttons_clear(); return 0; }
        if ((keys & KEY_SELECT) && en[sel]) {
            if (sel == D_TURNIN) { buttons_clear(); return 1; }
            if (sel == D_DEPART) { buttons_clear(); return 0; }
            if (sel == D_YARD) frontier_run_shipyard(HOME_BOARD);
            if (sel == D_JOBS) contracts_run_board(HOME_BOARD);
            buttons_clear();
        }
        mp_tick();
        yield_();
    }
}

void frontier_run_services(int board) {
    const char* labels[4];
    int n = 0;
    if (board != HOME_BOARD) labels[n++] = "Trade goods";
    labels[n++] = "Job board";
    if (frontier_has_shipyard(board)) labels[n++] = "Shipyard";
    labels[n++] = "Back";
    int sel = 0;
    buttons_clear();
    while (true) {
        frame_clear();
        frame_draw_text(board == HOME_BOARD ? "STATION SERVICES" : poi_names[board], title_color, 24);
        if (board != HOME_BOARD && frontier_has_shipyard(board)) frame_draw_string_centered(70, "shipyard outpost", RGB565(80, 220, 120));
        if (board != HOME_BOARD && is_depot(board)) frame_draw_string_centered(88, "exotic fuel depot", RGB565(242, 150, 48));
        for (int i = 0; i < n; ++i) {
            int y = 120 + i * 30;
            if (i == sel) frame_fill_rect(90, y - 17, 180, 23, sbackground_color);
            frame_draw_string_centered(y, labels[i], i == sel ? selected_color : option_color);
        }
        game_draw_ui();
        frame_refresh();
        uint32_t keys = buttons_get_and_clear();
        {
            int tx, ty;
            if (ui_take_tap(&tx, &ty)) {
                int r = ui_tap_row(ty, 120, 30, n);
                if (r >= 0) { sel = r; keys |= BUTTON_A; } else keys |= BUTTON_B;
            }
        }
        if (keys & BUTTON_UP) sel = (sel + n - 1) % n;
        if (keys & BUTTON_DOWN) sel = (sel + 1) % n;
        if (keys & BUTTON_B) break;
        if (keys & KEY_SELECT) {
            const char* pick = labels[sel];
            if (!strcmp(pick, "Back")) break;
            if (!strcmp(pick, "Trade goods")) frontier_run_goods_market(board);
            if (!strcmp(pick, "Job board")) contracts_run_board(board);
            if (!strcmp(pick, "Shipyard")) frontier_run_shipyard(board);
            buttons_clear();
        }
        mp_tick();
        yield_();
    }
    buttons_clear();
}

// ---------------------------------------------------------------- story engine

extern const event_t random_events[];
extern const int random_event_count;
extern int shuffled_events[100];
extern int event_index;

static bool event_eligible(const event_t* e, int idx, int at) {
    if ((e->at ? e->at : EV_AT_EMPTY) != at) return false;
    if ((state.story_flags & e->req_flags) != e->req_flags) return false;
    if (state.story_flags & e->forbid_flags) return false;
    if (e->once && ((state.events_used[idx >> 5] >> (idx & 31)) & 1)) return false;
    return true;
}

static void mark_used(int idx) { state.events_used[idx >> 5] |= 1u << (idx & 31); }

const event_t* frontier_pick_event(int at) {
    if (state.chain_event) {
        for (int i = 0; i < random_event_count; ++i) {
            if (random_events[i].id == state.chain_event) {
                state.chain_event = 0;
                mark_used(i);
                return &random_events[i];
            }
        }
        state.chain_event = 0;
    }
    int n = random_event_count;
    for (int tries = 0; tries < n; ++tries) {
        int idx = shuffled_events[event_index++ % n];
        const event_t* e = &random_events[idx];
        if (!event_eligible(e, idx, at)) continue;
        if (e->weight == 1 && (get_rand_32() % 3)) continue;   // rare: one in three
        mark_used(idx);
        return e;
    }
    return NULL;
}

void frontier_apply_story_effect(uint8_t type, int16_t value) {
    switch (type) {
    case EV_EFFECT_FLAG_SET:   state.story_flags |= 1u << (value & 31); break;
    case EV_EFFECT_FLAG_CLEAR: state.story_flags &= ~(1u << (value & 31)); break;
    case EV_EFFECT_ORE: case EV_EFFECT_GAS: case EV_EFFECT_ALLOY: case EV_EFFECT_PARTS: case EV_EFFECT_RELIC: {
        int g = GOOD_ORE + (type - EV_EFFECT_ORE);
        if (value > 0) frontier_add_good(g, value); else frontier_take_good(g, -value);
        break;
    }
    case EV_EFFECT_CHAIN: state.chain_event = (uint8_t)value; break;
    case EV_EFFECT_HE3: if (value > 0) frontier_add_good(GOOD_HE3, value); else frontier_take_good(GOOD_HE3, -value); break;
    case EV_EFFECT_DARK: if (value > 0) frontier_add_good(GOOD_DARK, value); else frontier_take_good(GOOD_DARK, -value); break;
    }
}

#ifdef WAYFARER_HOST
// Host only test hooks, driven from the USB console as "DBG <cmd> ...".
//   DBG TELEPORT q r    DBG GIVE good n    DBG CREDITS n    DBG HULL n    DBG FUEL n    DBG FLAGS hex
#include <stdlib.h>
// centre the camera so the stock truncating pixel_to_hex lands on the given hex
static void dbg_center_camera(Hex target) {
    Point at = hex_to_pixel(target, hex_size);
    for (int dy = -24; dy <= 24; dy += 2) {
        for (int dx = -24; dx <= 24; dx += 2) {
            int sx = LCD_WIDTH/2 - at.x + dx, sy = LCD_HEIGHT/2 - at.y + dy;
            Hex h = pixel_to_hex(LCD_WIDTH/2 - sx, LCD_HEIGHT/2 - sy, hex_size);
            if (hex_equal(h, target)) { screen_pos.x = sx; screen_pos.y = sy; return; }
        }
    }
    screen_pos.x = LCD_WIDTH/2 - at.x; screen_pos.y = LCD_HEIGHT/2 - at.y;
}

int wayfarer_debug_command(const char* args) {
    char what[16]; int a = 0, b = 0;
    if (sscanf(args, "%15s %d %d", what, &a, &b) < 1) return 0;
    if (!strcasecmp(what, "TELEPORT")) { state.player.q = a; state.player.r = b; }
    else if (!strcasecmp(what, "POI")) { if (a < 0 || a >= POI_COUNT) return 0; state.player = state.poi[a]; }
    else if (!strcasecmp(what, "WRECK")) { if (a < 0 || a >= DERELICT_COUNT) return 0; state.player = state.derelicts[a]; }
    if (!strcasecmp(what, "TELEPORT") || !strcasecmp(what, "POI") || !strcasecmp(what, "WRECK")) {
        dbg_center_camera(state.player);
        return 1;
    }
    // ROCK n / CLOUD: park one hex west of the tile with the camera on it, so "press a" flies onto it
    if (!strcasecmp(what, "ROCK") || !strcasecmp(what, "CLOUD")) {
        Hex t = !strcasecmp(what, "ROCK") ? ((a < 0 || a >= 6) ? state.astroids[0] : state.astroids[a]) : state.nebula;
        state.player = (Hex){t.q - 1, t.r};
        dbg_center_camera(t);
        return 1;
    }
    if (!strcasecmp(what, "GIVE")) { return frontier_add_good(a, b) > 0; }
    if (!strcasecmp(what, "CREDITS")) { state.credits = a; return 1; }
    if (!strcasecmp(what, "HULL")) { state.hull = a; return 1; }
    if (!strcasecmp(what, "FUEL")) { state.fuel = a; return 1; }
    if (!strcasecmp(what, "FLAGS")) { state.story_flags = (uint32_t)strtoul(args + 6, NULL, 16); return 1; }
    if (!strcasecmp(what, "CHAIN")) { state.chain_event = a; return 1; }
    if (!strcasecmp(what, "YARD")) { state.shipyard_mask = a; return 1; }
    if (!strcasecmp(what, "DEPOT")) { state.depot_mask = a; return 1; }
    if (!strcasecmp(what, "SIGNAL")) { return signal_lock_play(a > 0 ? a : 3, 3); }
    if (!strcasecmp(what, "DRILL")) { return drill_play(); }
    if (0) { if (a < 0 || a >= 6) return 0; state.player = state.astroids[a]; Point at = hex_to_pixel(state.player, hex_size); screen_pos.x = LCD_WIDTH/2 - at.x; screen_pos.y = LCD_HEIGHT/2 - at.y; return 1; }
    if (!strcasecmp(what, "CLOUD")) { state.player = state.nebula; Point at = hex_to_pixel(state.player, hex_size); screen_pos.x = LCD_WIDTH/2 - at.x; screen_pos.y = LCD_HEIGHT/2 - at.y; return 1; }
    if (!strcasecmp(what, "HOME")) { state.player.q = 0; state.player.r = 0; screen_pos.x = LCD_WIDTH/2; screen_pos.y = LCD_HEIGHT/2; return 1; }
    if (!strcasecmp(what, "LOOT")) { state.derelict_looted = a; return 1; }
    return 0;
}
#endif

