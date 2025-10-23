#ifndef PROTAGONIST_H_INCLUDED
#define PROTAGONIST_H_INCLUDED

#include "convexhull.h"
#include "globals.h"

typedef struct{
    convex_hull2_t* ch;
    vec2_t* dir;
    float min_size;
    float max_size;
    int running;
    int rotating;
    float speed;
    float scale_down_factor;
} protagonist_t;


extern protagonist_t* prot;


void setup_protagonist();
void protagonist_eat_food();
void take_input(rafgl_game_data_t *game_data);
float protagonist_get_area();
float protagonist_get_size();
// void draw_protagonist(rafgl_raster_t* raster);
void draw_protagonist(rafgl_raster_t* raster, rafgl_raster_t* img);
void move_protagonist(float delta_time);
void protagonist_fight_enemies();
void free_protagonist();

#endif