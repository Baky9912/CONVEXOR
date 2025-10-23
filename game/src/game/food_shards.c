#include "rafgl.h"
#include "myrandom.h"
#include "convexhull.h"
#include "globals.h"
#include "food_shards.h"
#include "polygon.h"

/*
const int SHARD_CNT = 30;
convex_hull2_t* shards[SHARD_CNT];

const int UNIQUE_SHARD_CNT = 5;
const int SHARD_VERTEX[] = {3, 4, 5, 7, 16};
const float SHARD_SIZES[] = {30, 40, 60, 80, 100};
const float ROTATION[] = {0, 0, 0, 0, 0};
rafgl_pixel_rgb_t COLOR[UNIQUE_SHARD_CNT];
*/

convex_hull2_t* shards[SHARD_CNT];
int shard_type[SHARD_CNT];
float shard_area[SHARD_CNT];
int SHARD_VERTEX[] = {3, 4, 5, 7, 20};
//float SHARD_SIZES[] = {30, 40, 60, 80, 100};
float SHARD_SIZES[] = {30, 28, 38, 42, 35};
float SHARD_ROTATION[] = {0.1, 1, 0.7, 0.1, 0.1};
rafgl_pixel_rgb_t COLOR[UNIQUE_SHARD_CNT];


void setup_food_shards(vec2_t* pos){
    // around pos
    COLOR[0].rgba = rafgl_RGB(226, 28, 22);
    COLOR[1].rgba = rafgl_RGB(246, 255, 73);
    COLOR[2].rgba = rafgl_RGB(13, 94, 242);
    COLOR[3].rgba = rafgl_RGB(13, 149, 7);
    COLOR[4].rgba = rafgl_RGB(162, 8, 152);

    for(int i=0; i<SHARD_CNT; ++i){
        food_shard_spawn_random_around(i, pos);
    }
}

// Ovde moze da se koristi flyweight ali imam 30 elemenata tkd je nebitno
void food_shard_spawn_random_around(int ind, vec2_t* pos){
    int type = rand_int(0, UNIQUE_SHARD_CNT-1);
    shard_type[ind] = type;
    convex_hull2_t* ch = ch_make_regular(SHARD_VERTEX[type], SHARD_SIZES[type], SHARD_ROTATION[type]);
    float dist = (float)rand_int(65, 100)/100.0 * WIDTH/2;
    float angle = rand_float() * FULL_CIRCLE;
    polar_vec2(ch->offset, angle, dist);
    translate_vec2(ch->offset, pos);
    // free?
    shards[ind] = ch;
    shard_area[ind] = polygon_area(ch->v, ch->n, ch->bounding_box);
}


void fill_shard(rafgl_raster_t* raster, int i, vec2_t* cam_offset){
    rafgl_pixel_rgb_t clr = COLOR[shard_type[i]];
    fill_inside_convex(raster, shards[i], clr, cam_offset);
}


void draw_all_shards(rafgl_raster_t* raster, vec2_t* cam_offset){
    for(int i=0; i<SHARD_CNT; ++i){
        fill_shard(raster, i, cam_offset);
    }
}


void possibly_relocate_shards(vec2_t* pos){
    for(int i=0; i<SHARD_CNT; ++i){
        if(rafgl_distance2D(pos->x, pos->y, shards[i]->offset->x, shards[i]->offset->y) > WIDTH){
            float dist = (float)rand_int(65, 100)/100.0 * WIDTH/2;
            float angle = rand_float() * FULL_CIRCLE;
            polar_vec2(shards[i]->offset, angle, dist);
            translate_vec2(shards[i]->offset, pos);
        }
    }
}

float shard_gets_eaten(int ind, vec2_t* new_center){
    free(shards[ind]);
    food_shard_spawn_random_around(ind, new_center);
    return shard_area[ind] * 0.5;
}

void free_shards(){
    for(int i=0; i < SHARD_CNT; ++i){
        free_ch(shards[i]);
    }
}