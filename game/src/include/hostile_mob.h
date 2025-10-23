#ifndef HOSTILE_MOB_H_INCLUDED
#define HOSTILE_MOB_H_INCLUDED

#include "concavehull.h"

#define MAX_HMOBS 8

typedef struct{
    concave_hull2_t* nch;
    vec2_t* dir;
    float min_size;
    float max_size;
    int running;
    float speed;
    float scale_down_factor;
    float delusion_factor;
    int rotation;
} hostile_mob_t;


extern hostile_mob_t* hmobs[MAX_HMOBS];

hostile_mob_t* make_hostile_mob();
void setup_hmobs();
float hmob_get_size(hostile_mob_t* hm);
float hmob_get_area(hostile_mob_t* hm);
void hmobs_eat_food();
void scale_hmob(hostile_mob_t* hm, float scale);
void free_hmobs();
void hmob_die(int ind);
void move_hmobs(float delta_time);
void draw_hostile_mobs(rafgl_raster_t* raster, vec2_t* cam_offset);


#endif