#ifndef SEGMENT_H_INCLUDED
#define SEGMENT_H_INCLUDED

#include "geometry.h"
#include <rafgl.h>

int compute_code(vec2_t* p, rect2_t* r);
seg2_t* clip_seg_rect(seg2_t* seg, rect2_t* r);
void draw_seg(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width, int angle_width, vec2_t* offset);
void draw_east_up(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width);
void draw_east_down(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width);
void draw_north(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width);
void draw_south(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width);
// void draw_point(rafgl_raster_t* raster, rafgl_pixel_rgb_t color, int width, int x, int y);

#endif