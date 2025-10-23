
#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include "geometry.h"

void set_globals();

extern int WIDTH;
extern int HEIGHT;
extern char* GAME_NAME;
extern float KILL_OVERLAP;
extern float FEAR_RANGE;
extern vec2_t ZERO_VEC2;
extern float EPS_G;
extern vec2_t* cam_offset;
extern rect2_t* cam_trigger;
extern unsigned int seed;
extern unsigned long long frame_count;

#endif