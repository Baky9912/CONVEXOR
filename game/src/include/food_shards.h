#ifndef FOOD_SHARDS_H
#define FOOD_SHARDS_H

#include "rafgl.h"
#include "myrandom.h"
#include "convexhull.h"
#include "globals.h"

#define SHARD_CNT 30
#define UNIQUE_SHARD_CNT 5

extern convex_hull2_t* shards[SHARD_CNT];
extern int shard_type[SHARD_CNT];
extern float shard_area[SHARD_CNT];
extern int SHARD_VERTEX[];
extern float SHARD_SIZES[];
extern float ROTATION[];
extern rafgl_pixel_rgb_t COLOR[UNIQUE_SHARD_CNT];

void setup_food_shards(vec2_t* pos);
void food_shard_spawn_random_around(int ind, vec2_t* pos);
void fill_shard(rafgl_raster_t* raster, int i, vec2_t* cam_offset);
void draw_all_shards(rafgl_raster_t* raster, vec2_t* cam_offset);
void possibly_relocate_shards(vec2_t* pos);
float shard_gets_eaten(int ind, vec2_t* new_center);
void free_shards();

#endif  // FOOD_SHARDS_H
