#ifndef POLYGON_H_INCLUDED
#define POLYGON_H_INCLUDED

#include <rafgl.h>
#include "geometry.h"
#include "segment.h"
#include "convexhull.h"
#include "concavehull.h"


void draw_polygon_hull(rafgl_raster_t* raster, vec2_t* v, int pn, rafgl_pixel_rgb_t dot_style,
int dot_width, rafgl_pixel_rgb_t* style, int style_sz, int width, int angle_width, vec2_t* offset);
float polygon_area(vec2_t* v, int n, rect2_t* bb);
float intersection_area_ch_ch(convex_hull2_t* ch1, convex_hull2_t* ch2);
float intersection_area_ch_nch(convex_hull2_t* ch, concave_hull2_t* nch);
float intersection_area_ch_tri(convex_hull2_t* ch, tri2_t* tri, vec2_t* tri_off);
int bounding_boxes_intersect(convex_hull2_t* ch1, convex_hull2_t* ch2);
float bounding_box_distance(rect2_t* r1, vec2_t* off1, rect2_t* r2, vec2_t* off2);
void points_in_ch(convex_hull2_t* ch1, convex_hull2_t* ch2, vec2_t** out, int* outn);
void segment_intersections(vec2_t* a, int n, vec2_t* off_a, vec2_t* b, int m, vec2_t* off_b, vec2_t** out, int* outn);

#endif