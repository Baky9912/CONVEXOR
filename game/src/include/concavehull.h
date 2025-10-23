#ifndef CONCAVEHULL_H_INCLUDED
#define CONCAVEHULL_H_INCLUDED

#include "geometry.h"
#include <rafgl.h>

typedef struct {
    vec2_t* v;
    rect2_t* bounding_box;
    int n;
    vec2_t* offset;
    int tris_n;
    tri2_t* tris;
    int tri_lines_n;
    seg2_t** tri_lines;
} concave_hull2_t;

concave_hull2_t* nch_make_random(float exsize);
int nch_triangulate(concave_hull2_t* nch);
void nch_calculate_bounding_box(concave_hull2_t* nch);
int nch_point_inside(concave_hull2_t* nch, vec2_t* p);
void draw_concave_hull(rafgl_raster_t* raster, concave_hull2_t* nch, rafgl_pixel_rgb_t dot_style,
int dot_width, rafgl_pixel_rgb_t* style, int style_sz, int width, int angle_width, vec2_t* offset);
void print_concave_hull(concave_hull2_t* nch);
void fill_inside_concave(rafgl_raster_t* raster, concave_hull2_t* nch, vec2_t* cam_offset);
void setup_nch();
void free_nch(concave_hull2_t* nch);

#endif