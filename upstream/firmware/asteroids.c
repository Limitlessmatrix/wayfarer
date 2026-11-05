#include "asteroids.h"
#include "framebuffer.h"
#include "ui.h"
#include "buttons.h"
#include <stdio.h>
#include "pico/rand.h"
#include "pico/stdlib.h"
#include "touch.h"
#include <stdint.h>
#include <math.h>

#define ASTEROID_MAX 5
#define ASTEROID_MIN_VERT 6
#define ASTEROID_MAX_VERT 10
#define ASTEROID_MIN_RADIUS 12
#define ASTEROID_MAX_RADIUS 30

#define rand32  get_rand_32

#define BULLET_MAX 8
#define BULLET_SPEED 240.0f
#define BULLET_LIFE_MS 1500

typedef struct {
    float x, y;
    float vx, vy;
    int verts;
    float radius;
    float ang_offset;
    float verts_r[ASTEROID_MAX_VERT];
} asteroid_t;

static asteroid_t asteroids[ASTEROID_MAX];
static int asteroid_count = 0;

static float ship_x, ship_y;
static float ship_vx, ship_vy;
static float ship_heading; // radians
static const uint16_t COL_BG = RGB565(0, 0, 0);
static const uint16_t COL_FG = RGB565(255, 255, 255);

static const float PLAY_CX = (LCD_WIDTH / 2.0f);
static const float PLAY_CY = (LCD_HEIGHT / 2.0f);
static const float PLAY_R = (LCD_WIDTH < LCD_HEIGHT ? LCD_WIDTH : LCD_HEIGHT) / 2.0f;

typedef struct {
    float x, y;
    float vx, vy;
    int life_ms;
    int active;
} bullet_t;

static bullet_t bullets[BULLET_MAX];

static void spawn_asteroid(int i);
static void spawn_bullet(float x, float y, float vx, float vy);
static void circular_wrap(float *x, float *y);


int player_heading;
int player_speed;
int player_loc[2];

void astroids_draw_ship() {
    // draw a simple triangle ship using current ship_x, ship_y and heading
    float size = 8.0f;
    float hx = cosf(ship_heading);
    float hy = sinf(ship_heading);
    // tip
    int tx = (int)roundf(ship_x + hx * (size * 1.5f));
    int ty = (int)roundf(ship_y + hy * (size * 1.5f));
    // rear left
    float ldx = cosf(ship_heading + 2.4f);
    float ldy = sinf(ship_heading + 2.4f);
    int lx = (int)roundf(ship_x + ldx * size);
    int ly = (int)roundf(ship_y + ldy * size);
    // rear right
    float rdx = cosf(ship_heading - 2.4f);
    float rdy = sinf(ship_heading - 2.4f);
    int rx = (int)roundf(ship_x + rdx * size);
    int ry = (int)roundf(ship_y + rdy * size);

    frame_draw_line(tx, ty, lx, ly, COL_FG);
    frame_draw_line(tx, ty, rx, ry, COL_FG);
    frame_draw_line(lx, ly, rx, ry, COL_FG);
}

void astroids_draw() {
    // draw each asteroid as polygon
    for (int i = 0; i < asteroid_count; i++) {
        asteroid_t *a = &asteroids[i];
        int px[ASTEROID_MAX_VERT];
        int py[ASTEROID_MAX_VERT];
        for (int v = 0; v < a->verts; v++) {
            float ang = a->ang_offset + ((float)v / (float)a->verts) * (2.0f * M_PI);
            float r = a->radius * a->verts_r[v];
            px[v] = (int)roundf(a->x + cosf(ang) * r);
            py[v] = (int)roundf(a->y + sinf(ang) * r);
        }
        for (int v = 0; v < a->verts; v++) {
            int n = (v + 1) % a->verts;
            frame_draw_line(px[v], py[v], px[n], py[n], COL_FG);
        }
    }
}

static void spawn_asteroid(int i) {
    asteroid_t *a = &asteroids[i];
    // ensure it's not too close to the ship
    const float ship_safe_dist = ASTEROID_MAX_RADIUS + 48.0f; // safe distance from ship
    int attempts = 0;
    while (true) {
        a->x = (float)(rand32() % LCD_WIDTH);
        a->y = (float)(rand32() % LCD_HEIGHT);
        float dx = a->x - ship_x;
        float dy = a->y - ship_y;
        float dist2 = dx*dx + dy*dy;
        if (dist2 >= (ship_safe_dist * ship_safe_dist)) break;
        attempts++;
        if (attempts > 50) {
            // fallback: place asteroid roughly opposite the ship at a reasonable radius
            float ox = ship_x - PLAY_CX;
            float oy = ship_y - PLAY_CY;
            float len = sqrtf(ox*ox + oy*oy);
            if (len < 1.0f) {
                // ship near center, just put asteroid near edge
                float ang = ((float)(rand32() % 10000) / 10000.0f) * (2.0f * M_PI);
                a->x = PLAY_CX + cosf(ang) * (PLAY_R * 0.7f);
                a->y = PLAY_CY + sinf(ang) * (PLAY_R * 0.7f);
            } else {
                // opposite direction
                float nx = -ox / len;
                float ny = -oy / len;
                float jitter_ang = (((float)(rand32() % 6000) / 10000.0f) - 0.3f) * (2.0f * M_PI);
                float dist = PLAY_R * (0.6f + ((float)(rand32() % 2000) / 10000.0f));
                a->x = PLAY_CX + (nx * dist) + cosf(jitter_ang) * 6.0f;
                a->y = PLAY_CY + (ny * dist) + sinf(jitter_ang) * 6.0f;
            }
            break;
        }
    }
    a->vx = ((float)( (int)rand32() % 200 ) - 100.0f) / 100.0f;
    a->vy = ((float)( (int)rand32() % 200 ) - 100.0f) / 100.0f;
    a->verts = ASTEROID_MIN_VERT + (rand32() % (ASTEROID_MAX_VERT - ASTEROID_MIN_VERT + 1));
    if (a->verts > ASTEROID_MAX_VERT) a->verts = ASTEROID_MAX_VERT;
    a->radius = (float)(ASTEROID_MIN_RADIUS + (rand32() % (ASTEROID_MAX_RADIUS - ASTEROID_MIN_RADIUS + 1)));
    a->ang_offset = ((float)(rand32() % 10000) / 10000.0f) * (2.0f * M_PI);
    for (int v = 0; v < a->verts; v++) {
        a->verts_r[v] = 0.7f + ((float)(rand32() % 5000) / 10000.0f);
    }
}

static void spawn_bullet(float x, float y, float vx, float vy) {
    for (int i = 0; i < BULLET_MAX; i++) {
        if (!bullets[i].active) {
            bullets[i].active = 1;
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].vx = vx;
            bullets[i].vy = vy;
            bullets[i].life_ms = BULLET_LIFE_MS;
            return;
        }
    }
}

static void circular_wrap(float *x, float *y) {
    float dx = *x - PLAY_CX;
    float dy = *y - PLAY_CY;
    float dist = sqrtf(dx*dx + dy*dy);
    if (dist > PLAY_R) {
        float nx = PLAY_CX - dx * ((PLAY_R - 1.0f) / dist);
        float ny = PLAY_CY - dy * ((PLAY_R - 1.0f) / dist);
        *x = nx;
        *y = ny;
    }
}


int astroids_play() {

    frame_clear();
    ui_draw_popup("Clear the asteroid field");
    sleep_ms(500);
    buttons_clear();
    while (buttons_get_and_clear() != BUTTON_A) {
        sleep_ms(10);
    }
    // initialize physics and asteroids
    ship_x = LCD_WIDTH / 2.0f;
    ship_y = LCD_HEIGHT / 2.0f;
    ship_vx = ship_vy = 0.0f;
    ship_heading = -M_PI_2; // point up
    int score = 0;

    asteroid_count = ASTEROID_MAX;
    for (int i = 0; i < asteroid_count; i++) {
        spawn_asteroid(i);
    }

    // init bullets
    for (int i = 0; i < BULLET_MAX; i++) bullets[i].active = 0;

    ui_set_scene(SCENE_ASTEROIDS);
    uint64_t next_auto_fire = 0;
    uint64_t start = time_us_64();
    uint64_t end = start + (20ULL * 1000000ULL);   // balance: 20 s, was 10
    uint64_t grace_until = start + 1500000ULL;        // no collisions for the first 1.5 s
    uint64_t last = start;

    bool collided = false;

    while (time_us_64() < end && !collided) {
        uint64_t now = time_us_64();
        float dt = (now - last) / 1000000.0f;
        if (dt <= 0) dt = 0.001f;
        last = now;

        // input
        int b = buttons_get_raw();
        int events = buttons_get_and_clear();
        {
            // touch hold: turn toward the finger at the normal rate and fire five times a second
            int px, py;
            if (ui_pointer(&px, &py)) {
                float want = atan2f((float)py - ship_y, (float)px - ship_x);
                float diff = want - ship_heading;
                while (diff > M_PI) diff -= 2.0f * M_PI;
                while (diff < -M_PI) diff += 2.0f * M_PI;
                if (diff > 0.05f) b |= BUTTON_RIGHT;
                else if (diff < -0.05f) b |= BUTTON_LEFT;
                if (now >= next_auto_fire) { events |= BUTTON_A; next_auto_fire = now + 200000ULL; }
            }
        }
        if (b & BUTTON_LEFT) {
            ship_heading -= 3.6f * dt; // radians per second
        }
        if (b & BUTTON_RIGHT) {
            ship_heading += 3.6f * dt;
        }
        if (b & BUTTON_UP) {
            float thrust = 70.0f; // pixels/sec^2
            ship_vx += cosf(ship_heading) * thrust * dt;
            ship_vy += sinf(ship_heading) * thrust * dt;
        }

        // fire bullets on press events
        if (events & BUTTON_PRESS || events & BUTTON_A) {
            // spawn bullet at ship tip
            float tipx = ship_x + cosf(ship_heading) * 12.0f;
            float tipy = ship_y + sinf(ship_heading) * 12.0f;
            float bvx = ship_vx + cosf(ship_heading) * BULLET_SPEED;
            float bvy = ship_vy + sinf(ship_heading) * BULLET_SPEED;
            spawn_bullet(tipx, tipy, bvx, bvy);
        }
        

        // simple damping
        ship_vx *= (1.0f - 0.5f * dt);
        ship_vy *= (1.0f - 0.5f * dt);

        // update ship
        ship_x += ship_vx * dt;
        ship_y += ship_vy * dt;
        circular_wrap(&ship_x, &ship_y);

        // update asteroids
        for (int i = 0; i < asteroid_count; i++) {
            asteroid_t *a = &asteroids[i];
            a->x += a->vx * dt * 20.0f;   // balance: slower drift, was 30
            a->y += a->vy * dt * 20.0f;
            a->ang_offset += 0.2f * dt;
            circular_wrap(&a->x, &a->y);
            // collision simple: distance between centers
            float dx = a->x - ship_x;
            float dy = a->y - ship_y;
            float dist2 = dx*dx + dy*dy;
            float ship_radius = 6.0f;
            float r = a->radius + ship_radius;
            if (dist2 <= r * r && now > grace_until) {
                collided = true;
            }
        }

        // update bullets and bullet collisions
        for (int bi = 0; bi < BULLET_MAX; bi++) {
            if (!bullets[bi].active) continue;
            bullet_t *bb = &bullets[bi];
            bb->x += bb->vx * dt;
            bb->y += bb->vy * dt;
            bb->life_ms -= (int)(dt * 1000.0f);
            circular_wrap(&bb->x, &bb->y);
            if (bb->life_ms <= 0) { bb->active = 0; continue; }

            // check collision with asteroids
            for (int i = 0; i < asteroid_count; i++) {
                asteroid_t *a = &asteroids[i];
                float dx = a->x - bb->x;
                float dy = a->y - bb->y;
                float dist2 = dx*dx + dy*dy;
                if (dist2 <= (a->radius * a->radius)) {
                    // hit: respawn asteroid, remove bullet, increase score
                    bb->active = 0;
                    score += 15;
                    spawn_asteroid(i);
                    break;
                }
            }
        }

        // render
        frame_clear();
        astroids_draw();
        astroids_draw_ship();

        // draw timer and score
        uint64_t rem = (end > now) ? (end - now) : 0;
        int secs = (int)(rem / 1000000ULL);
        char buf[32];
        snprintf(buf, sizeof(buf), "%2d", secs);
        frame_draw_string(170, 14, buf, COL_FG);

        // draw bullets
        for (int bi = 0; bi < BULLET_MAX; bi++) {
            if (!bullets[bi].active) continue;
            int bx = (int)roundf(bullets[bi].x);
            int by = (int)roundf(bullets[bi].y);
            frame_draw_pixel(bx, by, COL_FG);
        }

        frame_refresh();

        sleep_ms(33);
    }

    ui_set_scene(SCENE_UI);
    // show result
    char buf2[64];
    if (collided) {
        snprintf(buf2, sizeof(buf2), "You hit an asteroid!\n -1 Hull, credits kept: +%d", score);
        score = -1 - score;   // negative: hit, but what was mined is kept
    } else {
        char buf2[64];
        snprintf(buf2, sizeof(buf2), "Asteroids cleared!\ncredits: +%d", score);
    }
    ui_draw_popup(buf2);
    sleep_ms(1000);
    buttons_clear();
    while (buttons_get_and_clear() != BUTTON_A) {
        sleep_ms(10);
    }
    return score;
}