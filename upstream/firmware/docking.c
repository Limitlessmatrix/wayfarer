#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include "bsp/framebuffer.h"
#include "game.h"
#include "bsp/buttons.h"
#include "bsp/ws2812.h"
#include "bsp/terminal.h"
#include "ui.h"
#include "img/achiv_icons.h"
#include "bsp/config.h"
#include "img/titlescreen.h"
#include "img/title_ani.h"
#include "bsp/eeprom.h"
#include "img/crew.h"
#include "img/captinlog.h"
#include "pico/rand.h"
#include "img/stations.h"
#include "img/voting.h"
#include "multiplayer.h"
#include "frontier.h"
#include "img/frontier_img.h"

extern game_character_t crew[5];
extern int crew_count;
extern char game_stats[80];
extern bool offline_mode;

extern const unsigned short project_icons[50][4096];

int stationId;
char station_name[32];
char topplayer[16];
char topclass;
int topscore;
char *title_buffer = "Unknown Station";
char message_buffer[300];
char vote_options[4][32];
int vote_count;
int vote_percent[4];
int vote_img[4];
int etotal, egames, epeers;

enum docking_state {
    none,
    welcome,
    syncing,
    voting,
    complete,
    manifest
};

enum docking_state dock_state;
absolute_time_t dock_time;

void parse_crew_string(char* str) {
    if (dock_state == none) {
        return; //This is to avoid dropping crew as people undock
    }
    crew_count = 0;
    char* ptr = str;
    for (int i=0; i<5; ++i) {
        while(*ptr == ' ') ++ptr;
        if (strnlen(ptr, 21) < 20) {
            break;
        }
        if ((*ptr < 'A') || (*ptr > 'E')) {
            break;
        }
        crew[i].level=1;
        crew[i].specialty=*ptr++;
        memcpy(crew[i].id, ptr, 5);
        crew[i].id[5] = '\0';
        ptr+=5;
        memcpy(crew[i].name, ptr, 14);
        crew[i].name[14] = '\0';
        crew[i].skin_tone = *(uint16_t*)crew[i].id;
        ptr+=14;
        crew_count++;
    }
    if (crew_count == 5) {
        config_achievement(fully_crew);
        int unique=true;
        for (int i=0; i<4; ++i) {
            for (int j=i+1; j<5; ++j) {
                if (crew[i].specialty == crew[j].specialty){
                    unique=false;
                }
            }
        }
        if (unique) {
            config_achievement(unique_crew);
        }
    }
    if (crew_count == 0) {
        crew[0].level=1;
        crew[0].specialty = config_get_class();
        memcpy(crew[0].id, config_get_id(), 5);
        crew[0].id[5] = '\0';
        snprintf(crew[0].name, sizeof(crew[0].name), "%s", config_get_handle());
        crew_count = 1;
    }
    //printf("Crew updated: %d members\n", crew_count);
}

void replace_char(char *str, char old_char, char new_char) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == old_char) {
            str[i] = new_char;
        }
    }
}

int process_docking_commands() {
    char command[512];
    int processed = 0;

    terminal_update();

    while (terminal_pop_docking_command(command, sizeof(command))) {
        if (strncmp(command, "$STATION", 8) == 0) {
            vote_count = 0;
            if (dock_state != syncing) {
                dock_state = syncing;
                dock_time = get_absolute_time();
            }
            sscanf(command, "$STATION %d|%c%d|%15[^|]|%299[^\\0]", &stationId, &topclass, &topscore, topplayer, message_buffer);
            //printf("station command recieved %d %c %d %s \"%10s\"\n", stationId, topclass, topscore, topplayer, message_buffer);
            if (stationId == 1) {
                title_buffer = "HORIZON STATION";
            } else if (stationId == 2) {
                title_buffer = "MERIDIAN STATION";
            } else if (stationId == 3) {
                title_buffer = "NEXUS STATION";
            } else {
                stationId = 0;
                title_buffer = "FRONTIER STATION";
            }
            snprintf(command, sizeof(command), "$ID %c%5.5s%-14.14s%08X ", config_get_class(), config_get_id(), config_get_handle(), config_get_achievements());
            sign_message(command, sizeof(command));
            terminal_send(command);
            for (int i=0;i<EEPROM_SLOTS;++i) {
                char line[65]={0};
                eeprom_readbytes(64*(i+1),line, 64);
                if (line[0] == '$') {
                    //printf("Sending game data from slot %d\r\n", i);
                    terminal_send(line);
                }
            }
            //printf("Docking complete, sending EOL\n");
            terminal_send("$EOL \r\n");
        } else if (strncmp(command, "$GAME", 5) == 0) {
            for (int i=0;i<EEPROM_SLOTS;++i) {
                char line[65]={0};
                eeprom_readbytes(64*(i+1),line, 64);
                if (strncmp(line, command, 16)==0) {
                    //printf("Clearing game in slot %d\n", i);
                    memset(line, 0, 64);
                    eeprom_writebytes(64*(i+1),line, 64);
                }
            }
        } else if (strncmp(command, "$OPTIONS", 8) == 0) {
            //printf("recieved OPTIONS\n");
            char* at = command + 9;
            int nchars, i=0;
            
            while (sscanf(at, "%02d%03d%32[^|]|%n", 
                &vote_img[i], &vote_percent[i], vote_options[i], &nchars) == 3) {
                at += nchars;
                if ((++i >= 4) || (*at == '\0')) break;
            }
            vote_count = i;
            //should we confirm reception?
        } else if (strncmp(command, "$EOL", 4) == 0) {
            sscanf(command, "$EOL %d %d %d", &etotal, &egames, &epeers);
            if (epeers>=10) {
                config_achievement(social_2);
            }
            else if (epeers>0) {
                config_achievement(social_1);
            }
            if (etotal >= 9000) {
                config_achievement(game_quota);
            }
            //printf("Docking complete\n");
            dock_state = welcome;
        } else if (strncmp(command, "$CREW", 5) == 0) {
            const char *payload = command + 5;
            parse_crew_string((char *)payload);
        }
        else if (strncmp(command, "$MP", 3) == 0) {
            mp_handle_command(command);
        }
        else {
            printf("Bad uart command: %s\r\n", command);
        }
        processed++;
    }

    return processed;
}

void run_docking_welcome() {
    buttons_clear();
    while (dock_state == welcome) {
        frame_clear();
        frame_draw_image(155, 5, docked_image_data, 49, 8);
        frame_draw_text(title_buffer, title_color, 30);
        if (stationId) frame_draw_image(80, 30, stations_image_data[stationId-1], 200, 128);
        frame_draw_text(message_buffer, description_color, 180);
        frame_refresh();
        int buttons = buttons_get_and_clear();
        if ((buttons & BUTTON_A) || (buttons & BUTTON_B)){
            dock_state = vote_count?voting:complete;
        }
        process_docking_commands();
    }
}

void run_docking_sync() {
    buttons_clear();
    int dots = 0;
    char msg[64];
    while (dock_state == syncing) {
        snprintf(msg, sizeof(msg), "Docking.%c%c", dots>0?'.':' ', dots>1?'.':' ');
        dots = ++dots %3;
        frame_clear();
        frame_draw_text(msg, RGB565(0,200,0), 180);
        frame_refresh();
        process_docking_commands();
        sleep_ms(10);
        absolute_time_t end_time = get_absolute_time();
        int64_t elapsed_us = absolute_time_diff_us(dock_time, end_time);
        if (elapsed_us > (1000000 * 5)) { //pairing went wrong
            dock_state = none;
            printf("Timeout during docking\n");
        }
    }
}

void draw_project_icon(int x, int y, int icon_idx) {
    if (icon_idx >=0 && icon_idx < 48)
        frame_draw_sprite(x, y, project_icons_image_data, 64, 64, (icon_idx %8)*64, (icon_idx / 8)*64, 64*8);
}

void run_docking_vote() {
    buttons_clear();
    int selected_option = 0;

    while (dock_state == voting) {
        frame_clear();

        frame_draw_image(97, 6, voteheader_image_data, 166, 48);
        int y_offset = 60; // Initial y position
        //y_offset += frame_draw_text(title_buffer, title_color, y_offset); // Yellow for POI name

        frame_fill_rect(62, y_offset+selected_option*80+2, 236, 66, sbackground_color);

        for (int i=0; i<vote_count; ++i) {
            char vline[12];
            frame_draw_rounded_rect(60, y_offset + (80 * i), 240, 70, border_color);
            draw_project_icon(63, y_offset + (80 * i)+3, vote_img[i]);
            int w = frame_calculate_text_width(vote_options[i]);
            frame_draw_string(212-(w/2), y_offset + (80 * i) + 30, vote_options[i], selected_color);
            snprintf(vline, sizeof(vline), "%5d votes", vote_percent[i]);
            frame_draw_string(200, y_offset + (80 * i) + 55, vline, option_color);
        }
    
        frame_refresh();
        uint32_t keys = buttons_get_and_clear();

        if (keys & BUTTON_UP) {
            if (selected_option > 0) selected_option--;
        }
        else if (keys & BUTTON_DOWN) {
            if (selected_option < (vote_count-1)) selected_option++;
        }
        if (keys & BUTTON_A) {
            char resp[64];
            snprintf(resp, sizeof(resp), "$VOTE %5.5s %d ", config_get_id(), selected_option);
            sign_message(resp, sizeof(resp));
            terminal_send(resp);
            config_add_vote();
            break; 
        }
        if (keys & BUTTON_B) {
            break; 
        }
        process_docking_commands();
    }
    dock_state = complete;
}

void run_docking_complete() {
    buttons_clear();
    const char* ranks[6] = {"Cadet", "Deckhand", "Spacer", "Veteran", "Ace", "Legend"};
    int rl=0;
    if (etotal >= 81000) {rl = 5;} 
    else if (etotal >= 27000) {rl = 4;} 
    else if (etotal >= 9000) {rl = 3;} 
    else if (etotal >= 3000) {rl = 2;} 
    else if (etotal >= 1000) {rl = 1;}

    while (dock_state == complete) {
        char buff[16];
        int xw;
        frame_clear();
        frame_draw_image(98, 7, log1_image_data, 166, 34);
        frame_draw_image(58, 106, log2_image_data, 243, 130);
        for (int i=0; i< rl; ++i) {
            frame_draw_image(129 + i * 19, 131, star_image_data, 18, 18);
        }
        frame_draw_text(title_buffer, RGB565(175, 137, 78), 65);
        frame_draw_text(config_get_handle(), description_color, 90);
        frame_draw_image(145, 168, rank_image_data[rl], 70, 66);
        frame_draw_string_centered(167, ranks[rl], description_color);
        snprintf(buff, sizeof(buff), "%d", etotal);
        xw = frame_calculate_text_width(buff);
        frame_draw_string(88 - xw/2, 205, buff, description_color);
        snprintf(buff, sizeof(buff), "%d", egames);
        xw = frame_calculate_text_width(buff);
        frame_draw_string(280 - xw/2, 155, buff, description_color);
        snprintf(buff, sizeof(buff), "%d", epeers);
        xw = frame_calculate_text_width(buff);
        frame_draw_string(280 - xw/2, 224, buff, description_color);
        int y = 260;
        frame_draw_rounded_rect(80, y, 200, 72, border_color);
        frame_draw_image(84, y+4, leader_image_data, 100, 8);
        const char* pclass = get_class_name(topclass);
        frame_draw_text(topplayer, description_color, y + 26);
        int xoff = frame_calculate_text_width(pclass) + 20;
        frame_draw_string((LCD_WIDTH - xoff)/2 + 20, y + 46 , pclass, get_crew_color(topclass));
        frame_draw_image((LCD_WIDTH - xoff)/2, y + 30, player_img_data[topclass - 'A'], 15, 16);
        snprintf(buff, sizeof(buff), "Score: %d", topscore);
        frame_draw_text(buff, RGB565(244, 173, 44), y + 66);
        frame_refresh();
        int buttons = buttons_get_and_clear();
        if ((buttons & BUTTON_A) || (buttons & BUTTON_B)){
            dock_state = manifest;
        }
        process_docking_commands();
    }
}

void run_docking_manifest() {
    buttons_clear();
    while (dock_state == manifest) {
        char crew_info[64];
        snprintf(crew_info, sizeof(crew_info), "%d crew aboard", crew_count);
        frame_clear();
        frame_draw_image(89, 7, crew_image_data, 178, 35);
        frame_draw_text(crew_info, description_color, 62);
        int yoff = 74;
        for (int i=0; i< 5; ++i) {
            if (i < crew_count) {
                gui_show_crew(crew[i].name, crew[i].specialty, yoff + i *50);
            } else {
                gui_show_crew(NULL, 0, yoff + i *50);
            }
        }
        frame_refresh();
        int buttons = buttons_get_and_clear();
        if ((buttons & BUTTON_A) || (buttons & BUTTON_B)){
            dock_state = none;
        }
        process_docking_commands();
    }
}

void run_achievement_screen() {
    buttons_clear();
    int selected_index = 0;
    while (dock_state == none) {
        uint32_t unlocks = config_get_achievements();
        frame_clear();
        frame_draw_image(95, 30, achive_image_data, 169, 12);
        frame_draw_image(172, 50, for_image_data, 24, 8);
        gui_show_crew(config_get_handle(), config_get_class(), 64);
        frame_draw_rounded_rect(32, 114, 294, 149, border_color);
        frame_draw_rounded_rect(62, 270, 234, 30, border_color);
        frame_draw_rounded_rect(140, 306, 80, 44, border_color);

        uint16_t color = RGB565(80,80,80);
        if (unlocks & (1<<selected_index)) {
            color = RGB565(180,180,180);
        }
        frame_draw_text(config_get_achievement_name(selected_index), color, 290); // White color
        int sx = selected_index % 8;
        int sy = selected_index / 8;
        frame_fill_rect(35 + sx * 36, 117 + sy * 36, 36, 36, selected_color); // Highlight selected character
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 8; col++) {
                const uint16_t* data = (unlocks & (1<<(row*8+col))) ? medal_image_data : locked_image_data;
                frame_draw_image(43 + col * 36, 118 + row * 36, data, 20, 32);
            }
        }

        frame_draw_image(158, 312, bonus_image_data, 43, 8);
        char buffer[64];
        int count = config_get_achievement_count();
        snprintf(buffer, sizeof(buffer), "+$%d", count * 150);
        frame_draw_text(buffer, RGB565(0, 0, 200), 340);
        frame_refresh();

        uint32_t keys = buttons_get_and_clear();
        if (keys & BUTTON_UP) {
            selected_index = (selected_index - 8 + 32) % 32;
        }
        if (keys & BUTTON_DOWN) {
            selected_index = (selected_index + 8) % 32;
        }
        if (keys & BUTTON_LEFT) {
            selected_index = (selected_index - 1 + 32) % 32;
        }
        if (keys & BUTTON_RIGHT) {
            selected_index = (selected_index + 1) % 32;
        }
        if (keys & BUTTON_B) {
            break;
        }
        process_docking_commands();
    }
}

void run_title_screen() {
    buttons_clear();
    uint8_t frame=0;
    uint8_t star_nxt=0;
    int star_idx , star_idx2 = 0;
    const uint16_t star_locs[24][2] = {
        {181,6}, {193,25}, {184,75}, {216,7}, {209,79}, {233,65}, {251,37}, {249,65}, 
        {251,37}, {295,39}, {288,75}, {307,90}, {324,71}, {22,137}, {85,19}, {102,29},
        {117,59}, {90,48}, {62,99}, {70,53}, {96,76}, {139,23}, {153,61}, {158,81}
    };
    while (dock_state == none) {
        frame_clear();
        frame_draw_image(0,0,title_image_data,360,360);
        if (frame >= 10 && frame < 14) {
            frame_draw_image(108,8,comet_image_data[frame-10],127,68);
        }
        if (frame >= 110 && frame < 114) {
            frame_draw_image(48,-10,comet_image_data[frame-110],127,68);
        }
        if (frame % 10 == 0) {
            star_idx = get_rand_32()%24;
        }
        if (frame % 20 < 4) {
            int ii = (frame % 20);
            const uint16_t *loc = star_locs[star_idx];
            if (ii = 0) {
                frame_draw_image(loc[0]-5,loc[1]-5,st1_image_data,10,10);
            }
            else if (ii == 2) {
                frame_draw_image(loc[0]-17,loc[1]-16,st3_image_data,35,33);
            }
            else {
                frame_draw_image(loc[0]-9,loc[1]-8,st2_image_data,18,17);
            }
        }
        if ((frame >= star_nxt) && (((frame-star_nxt)%256) < 5)) {
            int ii = (frame - star_nxt)%256;
            const uint16_t *loc = star_locs[star_idx2];
            if (ii == 0) {
                frame_draw_image(loc[0]-5,loc[1]-5,st1_image_data,10,10);
            }
            else if (ii == 2) {
                frame_draw_image(loc[0]-17,loc[1]-16,st3_image_data,35,33);
            }
            else if (ii == 4) {
                star_nxt += 5 + (get_rand_32()%15);
                star_idx2 = get_rand_32()%24;
            } 
            else {
                frame_draw_image(loc[0]-9,loc[1]-8,st2_image_data,18,17);
            }
        }

        {
            // frontier expansion dressing: a wreck drifting across the sky, tagline, mission tally
            int wx = 400 - ((frame * 2) % 520);
            frame_draw_image(wx, 52, wreck_image_data, WRECK_W, WRECK_H);
            frame_draw_string_centered(283, "FRONTIER EXPANSION", RGB565(242, 150, 48));
            int m = config_get_missions();
            if (m > 0) {
                char tally[32];
                snprintf(tally, sizeof(tally), "%d mission%s logged", m, m == 1 ? "" : "s");
                frame_draw_string_centered(318, tally, RGB565(150, 150, 150));
            }
        }
        if (offline_mode) {
            frame_draw_text("OFFLINE MODE", RGB565(63, 204, 172), 300);
        }
        frame_refresh();
        frame++;
        sleep_ms(10);
        int buttons = buttons_get_and_clear();
        if (buttons & BUTTON_A) {
            game_run();
        }
        else if (buttons & BUTTON_PRESS) {
            run_achievement_screen();
        }
        process_docking_commands();
    }
}

void run_main_loop() {
    switch (dock_state) {
    case none:
        run_title_screen();
        break;
    case welcome:
        run_docking_welcome();
        break;
    case syncing:
        run_docking_sync();
        break;
    case voting:
        run_docking_vote();
        break;
    case complete:
        run_docking_complete();
        break;
    case manifest:
        run_docking_manifest();
        break;
    default:
        printf("Unknown docking state: %d\n", dock_state);
        dock_state = none;
        break;
    }
}