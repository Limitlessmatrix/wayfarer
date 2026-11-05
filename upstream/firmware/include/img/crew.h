// crew.h
#ifndef CREW_H
#define CREW_H

#include <stdint.h>

extern const unsigned short player_img_data[5][240];
extern const unsigned short crew_image_data[6230];
extern const unsigned short pressa_image_data[1064];
extern const unsigned short star_image_data[324];
extern const unsigned short wingl_image_data[240];
extern const unsigned short wingr_image_data[240];
extern const unsigned short fuelb_image_data[768];
extern const unsigned short cargo_image_data[918];
extern const unsigned short exit_image_data[864];
extern const unsigned short market_image_data[1295];

unsigned short get_crew_color(const char c); 

#endif