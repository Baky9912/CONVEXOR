#ifndef CONVEXHULL_H_INCLUDED
#define CONVEXHULL_H_INCLUDED

#include "geometry.h"
#include <rafgl.h>

typedef struct {
    vec2_t* v;
    rect2_t* bounding_box;
    int n;
    vec2_t* offset;
} convex_hull2_t;

convex_hull2_t* ch_make_random(float exsize);
convex_hull2_t* ch_make_regular(int vertex, float size, float rotation);
void ch_calculate_bounding_box(convex_hull2_t* ch);
int ch_point_inside(convex_hull2_t* ch, vec2_t* p);
void scale_convex_hull(convex_hull2_t* ch, float scale);
//void draw_convex_polygon_over_raster(convex_hull2_t* ch, vec2_t* ch_offset, rafgl_raster_t* raster);
void draw_raster_inside_convex(rafgl_raster_t* raster, convex_hull2_t* ch, rafgl_raster_t* img, vec2_t* cam_offset);
void draw_convex_hull(rafgl_raster_t* raster, convex_hull2_t* ch, rafgl_pixel_rgb_t dot_style, int dot_width, rafgl_pixel_rgb_t* style,
int style_sz, int width, int angle_width, vec2_t* offset);
void print_convex_hull(convex_hull2_t* ch);
void fill_inside_convex(rafgl_raster_t* raster, convex_hull2_t* ch, rafgl_pixel_rgb_t color, vec2_t* cam_offset);
void free_ch(convex_hull2_t* ch);

#endif