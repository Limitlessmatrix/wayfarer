#include "bsp/framebuffer.h"
#include "bsp/buttons.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/rand.h"
#include "pico/stdlib.h"
#include "touch.h"

#define MAZE_SIZE 21
#define MAZE_CELL 16
#define MAZE_WALL_COLOR RGB565(136, 166, 255)
#define MAZE_BACK_COLOR RGB565(18, 26, 38)
#define MAZE_FLOOR_COLOR RGB565(27, 39, 53)
#define PLAYER_COLOR RGB565(255, 214, 92)
#define GOAL_COLOR RGB565(120, 255, 128)
#define STATUS_COLOR RGB565(255, 255, 255)

static bool maze[MAZE_SIZE][MAZE_SIZE];
static int player_x = 1;
static int player_y = 1;
static int goal_x = MAZE_SIZE - 2;
static int goal_y = MAZE_SIZE - 2;

static void maze_generate(void) {
    for (int y = 0; y < MAZE_SIZE; ++y) {
        for (int x = 0; x < MAZE_SIZE; ++x) {
            maze[y][x] = true;
        }
    }

    int stack_x[MAZE_SIZE * MAZE_SIZE];
    int stack_y[MAZE_SIZE * MAZE_SIZE];
    int stack_depth = 0;

    const int dx[4] = {2, 0, -2, 0};
    const int dy[4] = {0, 2, 0, -2};

    maze[1][1] = false;
    stack_x[stack_depth] = 1;
    stack_y[stack_depth] = 1;
    stack_depth++;

    while (stack_depth > 0) {
        int cx = stack_x[stack_depth - 1];
        int cy = stack_y[stack_depth - 1];

        int options[4];
        int option_count = 0;

        for (int i = 0; i < 4; ++i) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (nx > 0 && nx < MAZE_SIZE - 1 && ny > 0 && ny < MAZE_SIZE - 1 && maze[ny][nx]) {
                options[option_count++] = i;
            }
        }

        if (option_count == 0) {
            stack_depth--;
            continue;
        }

        int dir = options[get_rand_32() % option_count];
        int wall_x = cx + dx[dir] / 2;
        int wall_y = cy + dy[dir] / 2;
        int next_x = cx + dx[dir];
        int next_y = cy + dy[dir];

        maze[wall_y][wall_x] = false;
        maze[next_y][next_x] = false;

        stack_x[stack_depth] = next_x;
        stack_y[stack_depth] = next_y;
        stack_depth++;
    }

    goal_x = MAZE_SIZE - 2;
    goal_y = MAZE_SIZE - 2;
    while (maze[goal_y][goal_x]) {
        goal_x -= 1;
        if (goal_x <= 1) {
            goal_x = MAZE_SIZE - 2;
            goal_y -= 1;
        }
    }

    player_x = 1;
    player_y = 1;
}

static bool can_move_to(int x, int y) {
    if (x <= 0 || x >= MAZE_SIZE - 1 || y <= 0 || y >= MAZE_SIZE - 1) {
        return false;
    }
    return !maze[y][x];
}

static void handle_input(void) {
    int buttons = buttons_get_and_clear();

    if (buttons & BUTTON_UP) {
        if (can_move_to(player_x, player_y - 1)) {
            player_y--;
        }
    }
    if (buttons & BUTTON_DOWN) {
        if (can_move_to(player_x, player_y + 1)) {
            player_y++;
        }
    }
    if (buttons & BUTTON_LEFT) {
        if (can_move_to(player_x - 1, player_y)) {
            player_x--;
        }
    }
    if (buttons & BUTTON_RIGHT) {
        if (can_move_to(player_x + 1, player_y)) {
            player_x++;
        }
    }
    if (buttons & BUTTON_B) {
        maze_generate();
    }
}

static void draw_maze(void) {
    const int camera_x = LCD_WIDTH / 2 - (player_x * MAZE_CELL + MAZE_CELL / 2);
    const int camera_y = LCD_HEIGHT / 2 - (player_y * MAZE_CELL + MAZE_CELL / 2);

    for (int y = 0; y < MAZE_SIZE; ++y) {
        for (int x = 0; x < MAZE_SIZE; ++x) {
            int screen_x = x * MAZE_CELL + camera_x;
            int screen_y = y * MAZE_CELL + camera_y;

            if (maze[y][x]) {
                frame_fill_rect(screen_x + 1, screen_y + 1, MAZE_CELL - 2, MAZE_CELL - 2, MAZE_WALL_COLOR);
            } else {
                frame_fill_rect(screen_x + 1, screen_y + 1, MAZE_CELL - 2, MAZE_CELL - 2, MAZE_FLOOR_COLOR);
            }
        }
    }

    int goal_screen_x = goal_x * MAZE_CELL + camera_x + 3;
    int goal_screen_y = goal_y * MAZE_CELL + camera_y + 3;
    frame_fill_rect(goal_screen_x, goal_screen_y, MAZE_CELL - 6, MAZE_CELL - 6, GOAL_COLOR);

    frame_fill_rect(LCD_WIDTH / 2 - 5, LCD_HEIGHT / 2 - 5, 10, 10, PLAYER_COLOR);
    frame_draw_line(LCD_WIDTH / 2 - 9, LCD_HEIGHT / 2, LCD_WIDTH / 2 + 9, LCD_HEIGHT / 2, STATUS_COLOR);
    frame_draw_line(LCD_WIDTH / 2, LCD_HEIGHT / 2 - 9, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 9, STATUS_COLOR);

}

int maze_game_loop(void) {
    maze_generate();

    while (true) {
        handle_input();

        frame_clear();
        frame_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, MAZE_BACK_COLOR);
        draw_maze();
        frame_draw_string_centered(16, "MAZE", STATUS_COLOR);
        frame_draw_string_centered(LCD_HEIGHT - 22, "B = RESET", STATUS_COLOR);

        if (player_x == goal_x && player_y == goal_y) {
            frame_draw_string_centered(LCD_HEIGHT / 2 - 32, "ESCAPE!", GOAL_COLOR);
            frame_draw_string_centered(LCD_HEIGHT / 2 + 10, "PRESS B", STATUS_COLOR);
        }

        frame_refresh();
        sleep_ms(30);
    }
    return 0;
}

// Frontier: timed wreck crawl. Returns 1 when the core (goal) is reached, 0 on timeout or B.
int maze_play(uint32_t limit_ms) {
    maze_generate();
    ui_set_scene(SCENE_MAZE);
    uint64_t end = time_us_64() + (uint64_t)limit_ms * 1000ULL;
    uint64_t next_hold_step = 0;
    buttons_clear();
    while (true) {
        int buttons = buttons_get_and_clear();
        {
            int tx, ty;
            uint64_t tnow = time_us_64();
            bool held = ui_pointer(&tx, &ty) && tnow >= next_hold_step;
            if (held) next_hold_step = tnow + 140000ULL;
            if (held || ui_take_tap(&tx, &ty)) {
                int dx = tx - LCD_WIDTH / 2, dy = ty - LCD_HEIGHT / 2;
                if (dx * dx > dy * dy) buttons |= dx < 0 ? BUTTON_LEFT : BUTTON_RIGHT;
                else buttons |= dy < 0 ? BUTTON_UP : BUTTON_DOWN;
            }
        }
        if ((buttons & BUTTON_UP) && can_move_to(player_x, player_y - 1)) player_y--;
        if ((buttons & BUTTON_DOWN) && can_move_to(player_x, player_y + 1)) player_y++;
        if ((buttons & BUTTON_LEFT) && can_move_to(player_x - 1, player_y)) player_x--;
        if ((buttons & BUTTON_RIGHT) && can_move_to(player_x + 1, player_y)) player_x++;
        if (buttons & BUTTON_B) { ui_set_scene(SCENE_UI); return 0; }
        frame_clear();
        frame_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, MAZE_BACK_COLOR);
        draw_maze();
        uint64_t now = time_us_64();
        uint64_t rem = (end > now) ? (end - now) : 0;
        char buf[32];
        snprintf(buf, sizeof(buf), "%2d", (int)(rem / 1000000ULL));
        frame_draw_string(170, 14, buf, STATUS_COLOR);
        frame_draw_string_centered(34, "REACH THE CORE", STATUS_COLOR);
        frame_draw_string_centered(312, "B = ABORT", STATUS_COLOR);
        if (player_x == goal_x && player_y == goal_y) {
            frame_draw_string_centered(LCD_HEIGHT / 2 - 32, "CORE REACHED", GOAL_COLOR);
            frame_refresh();
            sleep_ms(800);
            ui_set_scene(SCENE_UI);
            return 1;
        }
        if (rem == 0) {
            frame_draw_string_centered(LCD_HEIGHT / 2 - 32, "OUT OF TIME", STATUS_COLOR);
            frame_refresh();
            sleep_ms(800);
            ui_set_scene(SCENE_UI);
            return 0;
        }
        frame_refresh();
        sleep_ms(30);
    }
}

