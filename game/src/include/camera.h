#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "convexhull.h"
#include "geometry.h"

void move_camera(convex_hull2_t* ch, vec2_t* cam_offset, rect2_t* cam_trigger);
void draw_background(rafgl_raster_t* raster);
void clean_game();
void setup_game();
void postfx(rafgl_raster_t* raster);

#endif