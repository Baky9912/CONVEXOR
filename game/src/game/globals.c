#include "geometry.h"
#include "globals.h"
#include "myrandom.h"
#include <time.h>

int WIDTH = 1400;
int HEIGHT = 1400 * 9/16;
// int WIDTH = 1600;
// int HEIGHT = 900;
char* GAME_NAME = "CONVEXOR";
float KILL_OVERLAP = 0.3;
vec2_t ZERO_VEC2;
float EPS_G = 1e-5;
vec2_t* cam_offset;
rect2_t* cam_trigger;
unsigned int seed = 0;
unsigned long long frame_count = 0;  // TODO preimenuj u frame_counter
float FEAR_RANGE = 100;

void set_globals(){
    set_vec2(&ZERO_VEC2, 0, 0);
    cam_offset = make_vec2(0, 0);
    seed = (unsigned int)time(NULL);
    srand(seed);
}

