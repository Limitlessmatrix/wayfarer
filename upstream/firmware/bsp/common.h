#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>

void pseudo_random_init(int seed);
int psuedo_random();
void sign_message(char* message, size_t msg_size);

typedef struct {
    uint8_t specialty;
    uint8_t level;
    uint16_t skin_tone; // RGB565 color for character skin tone
    unsigned char id[6];
    char name[15];
} game_character_t;

#define CREW_SPECIALTY_PILOT 'A'
#define CREW_SPECIALTY_ENGINEER 'B'
#define CREW_SPECIALTY_SCIENTIST 'C'
#define CREW_SPECIALTY_SOLDIER 'D'
#define CREW_SPECIALTY_MEDIC 'E'

#endif //COMMON_H
