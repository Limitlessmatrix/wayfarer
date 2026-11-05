#include "framebuffer.h"
#include "buttons.h"
#include "pico/rand.h"
#include "pico/stdlib.h"
#include "touch.h"
#include "ui.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define CLOUD_MIN_RADIUS 20
#define CLOUD_MAX_RADIUS 36
#define CLOUD_MAX_VERT   7
#define CLOUD_MAX       36
#define MAX_RANGE       500

#define rand32  get_rand_32

#define OFF_X   (LCD_WIDTH/2)
#define OFF_Y   (LCD_HEIGHT/2)

static float ship_x, ship_y;
static float ship_vx, ship_vy;
static float ship_heading; // radians
static float radar_heading; //radians
static const uint16_t COL_BG = RGB565(0, 0, 0);
static const uint16_t COL_FG = RGB565(127, 255, 127);

typedef struct {
    float x, y;
    int verts;
    float radius;
    float ang_offset;
    int vis;
    float verts_r[CLOUD_MAX_VERT];
} cloud_t;

static cloud_t clouds[CLOUD_MAX];
static int cloud_count = 0;

void nebula_draw_ship() {
    // draw a simple triangle ship using current heading
    float size = 8.0f;
    float hx = cosf(ship_heading);
    float hy = sinf(ship_heading);
    // tip
    int tx = (int)roundf(hx * (size * 1.5f));
    int ty = (int)roundf(hy * (size * 1.5f));
    // rear left
    float ldx = cosf(ship_heading + 2.4f);
    float ldy = sinf(ship_heading + 2.4f);
    int lx = (int)roundf(ldx * size);
    int ly = (int)roundf(ldy * size);
    // rear right
    float rdx = cosf(ship_heading - 2.4f);
    float rdy = sinf(ship_heading - 2.4f);
    int rx = (int)roundf(rdx * size);
    int ry = (int)roundf(rdy * size);

    frame_draw_line(OFF_X + tx, OFF_Y + ty, OFF_X + lx, OFF_Y + ly, COL_FG);
    frame_draw_line(OFF_X + tx, OFF_Y + ty, OFF_X + rx, OFF_Y + ry, COL_FG);
    frame_draw_line(OFF_X + lx, OFF_Y + ly, OFF_X + rx, OFF_Y + ry, COL_FG);
}

void nebulas_draw() {
    // draw each nebulas as star
    for (int i = 0; i < cloud_count; i++) {
        cloud_t *a = &clouds[i];
        int px[CLOUD_MAX_VERT];
        int py[CLOUD_MAX_VERT];
        uint16_t color = RGB565(a->vis/2, a->vis, a->vis/2);
        for (int v = 0; v < a->verts; v++) {
            float ang = a->ang_offset + ((float)v / (float)a->verts) * (2.0f * M_PI);
            float r = a->radius * a->verts_r[v];
            px[v] = (int)roundf(a->x + cosf(ang) * r);
            py[v] = (int)roundf(a->y + sinf(ang) * r);
        }
        for (int v = 0; v < a->verts; v++) {
            frame_draw_line(OFF_X + px[v] - ship_x, OFF_Y + py[v] - ship_y, OFF_X + a->x - ship_x, OFF_Y + a->y - ship_y, color);
        }
    }
}

static void spawn_nebula(int i) {
    cloud_t *a = &clouds[i];

    const float min_r = (float)CLOUD_MAX_RADIUS + 10.0f;
    const float max_r = (float)MAX_RANGE - 10.0f;
    const float u = (float)(rand32() & 0xFFFFu) / 65535.0f;
    const float r = sqrtf(u) * (max_r - min_r) + min_r;
    float ang = (float)(rand32() % 360) * M_PI / 180.0f;

    a->x = cosf(ang) * r;
    a->y = sinf(ang) * r;

    a->vis = 255;
    a->verts = CLOUD_MAX_VERT;
    a->radius = CLOUD_MAX_RADIUS;
    a->ang_offset = (float)(rand32() % 360) * M_PI / 180.0f;
    for (int v = 0; v < a->verts; v++) {
        a->verts_r[v] = 0.7f + ((float)(rand32() % 5000) / 10000.0f);
    }
}

void draw_radar() {
    int ex = OFF_X + (int)roundf(cosf(radar_heading) * 200);
    int ey = OFF_Y + (int)roundf(sinf(radar_heading) * 200);
    frame_draw_line(OFF_X, OFF_Y, ex, ey, RGB565(0,125,0));
}

float calc_dist2_from_ray(float x0, float y0, float rx, float ry, float ray_heading) {
    const float dx = x0 - rx;
    const float dy = y0 - ry;

    const float ux = cosf(ray_heading);
    const float uy = sinf(ray_heading);

    const float t = dx * ux + dy * uy;
    if (t <= 0.0f) {
        return dx * dx + dy * dy;
    }

    const float px = rx + ux * t;
    const float py = ry + uy * t;
    const float pdx = x0 - px;
    const float pdy = y0 - py;
    return pdx * pdx + pdy * pdy;
}

int nebula_play() {
    frame_clear();
    ui_draw_popup("Escape the nebula");
    sleep_ms(500);
    buttons_clear();
    while (buttons_get_and_clear() != BUTTON_A) {
        sleep_ms(10);
    }
    // initialize physics and nebula
    ship_x = 0;
    ship_y = 0;
    ship_vx = ship_vy = 0.0f;
    ship_heading = -M_PI_2; // point up
    int score = 300;

    cloud_count = CLOUD_MAX;
    for (int i = 0; i < cloud_count; i++) {
        spawn_nebula(i);
    }

    ui_set_scene(SCENE_NEBULA);
    uint64_t start = time_us_64();
    uint64_t end = start + (18ULL * 1000000ULL);   // balance: 18 s, was 12
    uint64_t last = start;

    bool collided = false;
    float d2 = 0;
    const int d2_max = MAX_RANGE * MAX_RANGE;
    const float cloud_speed_cap = 32.0f;

    while ((time_us_64() < end) && d2 < d2_max) {
        uint64_t now = time_us_64();
        float dt = (now - last) / 1000000.0f;
        if (dt <= 0) dt = 0.001f;
        last = now;

        // input
        int b = buttons_get_raw();
        int events = buttons_get_and_clear();
        {
            int px, py;
            if (ui_pointer(&px, &py)) {
                float dx = (float)px - OFF_X, dy = (float)py - OFF_Y;
                if (dx * dx + dy * dy > 14.0f * 14.0f) {      // small dead zone around the ship
                    float want = atan2f(dy, dx);
                    float diff = want - ship_heading;
                    while (diff > M_PI) diff -= 2.0f * M_PI;
                    while (diff < -M_PI) diff += 2.0f * M_PI;
                    if (diff > 0.05f) b |= BUTTON_RIGHT;
                    else if (diff < -0.05f) b |= BUTTON_LEFT;
                    if (diff < 0.6f && diff > -0.6f) b |= BUTTON_UP;
                }
            }
        }
        if (b & BUTTON_LEFT) {
            ship_heading -= 3.0f * dt; // radians per second
        }
        if (b & BUTTON_RIGHT) {
            ship_heading += 3.0f * dt;
        }
        if (b & BUTTON_UP) {
            float thrust = 60.0f; // pixels/sec^2
            if (collided) thrust = 20.0f; // weak thrust while already in a cloud
            ship_vx += cosf(ship_heading) * thrust * dt;
            ship_vy += sinf(ship_heading) * thrust * dt;
        }

        // simple damping
        ship_vx *= (1.0f - 0.5f * dt);
        ship_vy *= (1.0f - 0.5f * dt);

        // update ship
        ship_x += ship_vx * dt;
        ship_y += ship_vy * dt;
        d2 = ship_x * ship_x + ship_y * ship_y;

        // update nebulas
        collided = false;
        for (int i = 0; i < cloud_count; i++) {
            cloud_t *a = &clouds[i];
            a->ang_offset += 0.2f * dt;

            a->vis *= 0.9f;
            float rd2 = calc_dist2_from_ray(a->x, a->y, ship_x, ship_y, radar_heading);
            float radar_width = 10.0f;
            if (rd2 < (a->radius + radar_width) * (a->radius + radar_width)) {
                a->vis = 255;
            }
            // collision simple: distance between centers
            float dx = a->x - ship_x;
            float dy = a->y - ship_y;
            float dist2 = dx*dx + dy*dy;
            const float ship_radius = 2.0f;
            float r = a->radius + ship_radius;
            if (dist2 <= r * r) {
                collided = true;
            }
        }

        radar_heading += 2.0f * dt;

        if (collided) {
            float speed = hypotf(ship_vx, ship_vy);
            if (speed > cloud_speed_cap) {
                float scale = cloud_speed_cap / speed;
                ship_vx *= scale;
                ship_vy *= scale;
            }
            ship_vx *= 0.93f;
            ship_vy *= 0.93f;
        }

        // render
        frame_clear();
        frame_draw_circle(OFF_X-ship_x, OFF_Y-ship_y, MAX_RANGE, RGB565(80,0,80));
        draw_radar();
        nebulas_draw();
        nebula_draw_ship();

        // draw timer and score
        uint64_t rem = (end > now) ? (end - now) : 0;
        int secs = (int)(rem / 1000000ULL);
        char buf[32];
        snprintf(buf, sizeof(buf), "%2d", secs);
        frame_draw_string(170, 14, buf, COL_FG);

        frame_refresh();

        sleep_ms(33);
    }

    ui_set_scene(SCENE_UI);
    // show result
    char buf2[64];
    if (d2 < d2_max) {
        snprintf(buf2, sizeof(buf2), "You got lost in the nebula!\n -2 Fuel");
        score = -1;
    } else {
        char buf2[64];
        snprintf(buf2, sizeof(buf2), "Nebula cleared!\ncredits: +%d", score);
    }
    ui_draw_popup(buf2);
    sleep_ms(1000);
    buttons_clear();
    while (buttons_get_and_clear() != BUTTON_A) {
        sleep_ms(10);
    }
    return score;
}