// Frontier minigames: signal lock (pattern memory on the d-pad and LEDs) and drill rhythm (timing bar).
#include "frontier.h"
#include "framebuffer.h"
#include "buttons.h"
#include "ui.h"
#include "ws2812.h"
#include "multiplayer.h"
#include "touch.h"
#include "pico/rand.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3

static const uint16_t dim_col = RGB565(40, 60, 70);
static const uint16_t lit_col = RGB565(90, 230, 240);
static const uint16_t good_col = RGB565(80, 220, 120);
static const uint16_t bad_col = RGB565(230, 60, 60);

// axis aligned arrow heads drawn as solid scanlines, tip at (tx,ty) pointing away from the centre
static void draw_arrow(int dir, bool lit) {
    const int cx = LCD_WIDTH / 2, cy = 170, d = 58, s = 22;
    uint16_t c = lit ? lit_col : dim_col;
    for (int i = 0; i <= 2 * s; ++i) {
        int half = i / 2;
        switch (dir) {
        case DIR_UP:    frame_draw_line(cx - half, cy - d - s + i, cx + half, cy - d - s + i, c); break;
        case DIR_DOWN:  frame_draw_line(cx - half, cy + d + s - i, cx + half, cy + d + s - i, c); break;
        case DIR_LEFT:  frame_draw_line(cx - d - s + i, cy - half, cx - d - s + i, cy + half, c); break;
        case DIR_RIGHT: frame_draw_line(cx + d + s - i, cy - half, cx + d + s - i, cy + half, c); break;
        }
    }
}

static void leds_for(int dir, bool on) {
    // LED chain order: 0 left mid, 1 left low, 2 left bottom, 3 right bottom, 4 right low,
    // 5 right mid, 6 right top, 7 nose, 8 left top
    static const uint8_t groups[4][3] = {{7, 6, 8}, {2, 3, 1}, {0, 1, 8}, {5, 4, 6}};
    for (int i = 0; i < 3; ++i) ws2812_set_led(groups[dir][i], on ? 0 : 0, on ? 40 : 0, on ? 48 : 0);
}

static void draw_signal(const char* status, uint16_t status_col, int round, int rounds, int lit) {
    char line[40];
    frame_clear();
    frame_draw_text("SIGNAL LOCK", title_color, 24);
    snprintf(line, sizeof(line), "round %d of %d", round, rounds);
    frame_draw_string_centered(50, line, description_color);
    for (int d = 0; d < 4; ++d) draw_arrow(d, d == lit);
    frame_draw_string_centered(290, status, status_col);
    frame_refresh();
}

static void sleep_poll(int ms) {
    mp_tick();
    sleep_ms(ms);
}

// Watch a growing pattern of directions, then repeat it. 1 on success.
int signal_lock_play(int rounds, int start_len) {
    ui_set_scene(SCENE_SIGNAL);
    int seq[16];
    int len = start_len;
    if (rounds + start_len > 16) rounds = 16 - start_len;
    for (int i = 0; i < 16; ++i) seq[i] = get_rand_32() % 4;
    draw_signal("decode the beacon: watch, then repeat", description_color, 1, rounds, -1);
    sleep_poll(900);
    for (int r = 0; r < rounds; ++r, ++len) {
        for (int i = 0; i < len; ++i) {
            leds_for(seq[i], true);
            draw_signal("listen", description_color, r + 1, rounds, seq[i]);
            sleep_poll(380);
            leds_for(seq[i], false);
            draw_signal("listen", description_color, r + 1, rounds, -1);
            sleep_poll(140);
        }
        buttons_clear();
        for (int i = 0; i < len; ++i) {
            draw_signal("repeat", lit_col, r + 1, rounds, -1);
            int dir = -1;
            uint64_t deadline = time_us_64() + 5000000ULL;
            while (dir < 0) {
                int b = buttons_get_and_clear();
                {
                    int tx, ty;
                    if (ui_take_tap(&tx, &ty)) {
                        int dx = tx - LCD_WIDTH / 2, dy = ty - 170;
                        if (dx * dx > dy * dy) b |= dx < 0 ? BUTTON_LEFT : BUTTON_RIGHT;
                        else b |= dy < 0 ? BUTTON_UP : BUTTON_DOWN;
                    }
                }
                if (b & BUTTON_UP) dir = DIR_UP;
                else if (b & BUTTON_DOWN) dir = DIR_DOWN;
                else if (b & BUTTON_LEFT) dir = DIR_LEFT;
                else if (b & BUTTON_RIGHT) dir = DIR_RIGHT;
                else if (b & BUTTON_B) { ws2812_clear(); ui_set_scene(SCENE_UI); return 0; }
                if (time_us_64() > deadline) break;
                sleep_poll(10);
            }
            if (dir != seq[i]) {
                draw_signal("signal lost", bad_col, r + 1, rounds, dir);
                for (int k = 0; k < 4; ++k) leds_for(k, false);
                sleep_poll(900);
                ws2812_clear();
                ui_set_scene(SCENE_UI);
                return 0;
            }
            leds_for(dir, true);
            draw_signal("repeat", lit_col, r + 1, rounds, dir);
            sleep_poll(160);
            leds_for(dir, false);
        }
        draw_signal("locked", good_col, r + 1, rounds, -1);
        sleep_poll(500);
    }
    draw_signal("SIGNAL DECODED", good_col, rounds, rounds, -1);
    sleep_poll(900);
    ws2812_clear();
    ui_set_scene(SCENE_UI);
    return 1;
}

// Press A when the cursor crosses the green band. Returns hits (0..8).
int drill_play(void) {
    const int beats = 8, track_x = 50, track_w = 260, track_y = 190;
    int hits = 0, misses = 0;
    float pos = 0.0f, speed = 150.0f;   // px per second
    int dir = 1;
    ui_set_scene(SCENE_DRILL);
    buttons_clear();
    for (int beat = 0; beat < beats && misses < 3; ++beat) {
        int zone_w = 56 - beat * 3;
        int zone_x = track_x + 10 + get_rand_32() % (track_w - zone_w - 20);
        uint64_t last = time_us_64();
        uint64_t deadline = last + 4000000ULL;
        bool resolved = false;
        while (!resolved) {
            uint64_t now = time_us_64();
            float dt = (now - last) / 1000000.0f;
            last = now;
            pos += dir * speed * dt;
            if (pos < 0) { pos = 0; dir = 1; }
            if (pos > track_w) { pos = track_w; dir = -1; }
            int b = buttons_get_and_clear();
            {
                int tx, ty;
                if (ui_take_tap(&tx, &ty)) b |= BUTTON_A;
            }
            bool pressed = b & (BUTTON_A | BUTTON_PRESS);
            char line[40];
            frame_clear();
            frame_draw_text("DRILL", title_color, 24);
            snprintf(line, sizeof(line), "beat %d of %d   ore %d   heat %d/3", beat + 1, beats, hits, misses);
            frame_draw_string_centered(52, line, description_color);
            frame_draw_string_centered(80, "A when the bit hits green", description_color);
            // rock face
            frame_fill_rect(track_x, track_y - 40, track_w, 30, RGB565(70, 55, 40));
            for (int i = 0; i < beats; ++i) {
                uint16_t c = i < hits ? good_col : (i < beat ? RGB565(60, 60, 60) : RGB565(110, 90, 60));
                frame_fill_rect(track_x + 8 + i * 32, track_y - 34, 20, 18, c);
            }
            // track and zone
            frame_fill_rect(track_x, track_y, track_w, 24, RGB565(30, 30, 36));
            frame_fill_rect(zone_x, track_y, zone_w, 24, RGB565(30, 110, 60));
            frame_draw_rect(track_x, track_y, track_w, 24, border_color);
            int cx = track_x + (int)pos;
            frame_fill_rect(cx - 3, track_y - 6, 6, 36, RGB565(240, 200, 80));
            frame_refresh();
            if (pressed || now > deadline) {
                bool hit = pressed && cx >= zone_x && cx <= zone_x + zone_w;
                if (hit) { hits++; speed += 28.0f; }
                else misses++;
                frame_fill_rect(cx - 3, track_y - 6, 6, 36, hit ? good_col : bad_col);
                frame_draw_string_centered(250, hit ? "ORE!" : (pressed ? "slipped" : "too slow"), hit ? good_col : bad_col);
                frame_refresh();
                sleep_poll(350);
                resolved = true;
            }
            if (b & BUTTON_B) { ui_set_scene(SCENE_UI); return hits; }
            sleep_poll(16);
        }
    }
    char msg[64];
    snprintf(msg, sizeof(msg), misses >= 3 ? "Drill overheated.\nOre chunks: %d" : "Seam exhausted.\nOre chunks: %d", hits);
    ui_set_scene(SCENE_UI);
    frontier_popup_wait(msg);
    return hits;
}
