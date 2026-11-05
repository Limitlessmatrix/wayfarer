#ifndef UNLOCKS_H
#define UNLOCKS_H
#include <stdint.h>


#define CONFIG_ADDR     16

extern int game_running;

enum achievement {
    konami = 0,         //Obtained by entering the "konami" unlock code (up up down down left right left right) 
    one_game,           //Obtained by completing your first mission
    game_quota,         //Obtained by reaching 9000 total score
    game_event1,        //Obtained by encountering "Relics of the First Builders" random event in the game
    game_event2,        //Obtained by encountering "The Obsidian Monolith" random event in the game
    fully_crew,         //Obtained by having a crew of 5 members
    unique_crew,        //Obtained by having a crew with all different specialties
    social_1,           //Obtained by being a crew member on another player's mission.
    social_2,           //Obtained by being a crew member on 50 other players' missions 
    voter,              //Obtained by voting on at least 3 space station construction polls TODO
    re_flash,           //Obtained by reading the flash memory contents 
    eeprom,             //Obtained by reading the EEPROM contents
    offline,          //Obtained by finding a hardcoded unlock in the firmware TODO
    uart_usb,           //Obtained by finding an unlock code via the USB UART interface
    uart_dock,          //Obtained by finding an unlock code via the docker port UART interface 
    crypto,             //Obtained by solving a terminal cipher
    boot,               //Obtained by analyzing the self test screen and finding a hidden code or message
    art1,               //Obtained by analyzing the badge art circle patterns and finding a hidden code or message  
    art2,               //Obtained by analyzing the badge art constellations and finding a hidden code or message 
    committee,          //Obtained by social engineering an active member of the SAINTCON community 
    communities        // 12 additional achievements obtained by participating in community 
};

int config_load();
void config_clear();
void config_set_handle(const char* name);
void config_generate_handle(void);
int config_submit(const char* key);
int config_achievement(enum achievement id);
int config_get_achievement_count();
int config_get_score();
int config_get_missions();
int config_get_perks();
void config_add_perk(int bit);
void config_add_mission(int score);
uint32_t config_get_achievements();
char* config_get_handle();
char config_get_class();
char* get_class_name(char c);
char* config_get_achievement_name(enum achievement id);
char* config_get_id();
void config_add_vote();

#endif