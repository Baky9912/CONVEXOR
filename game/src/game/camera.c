#include "camera.h"
#include "convexhull.h"
#include "food_shards.h"
#include "hostile_mob.h"
#include "globals.h"
#include "protagonist.h"
#include "geometry.h"
#include "rafgl.h"


void move_camera(convex_hull2_t* ch, vec2_t* cam_offset, rect2_t* cam_trigger){
    float x_min = ch->bounding_box->min_x + ch->offset->x;
    float x_max = ch->bounding_box->max_x + ch->offset->x;
    float y_min = ch->bounding_box->min_y + ch->offset->y;
    float y_max = ch->bounding_box->max_y + ch->offset->y;

    vec2_t top_left, top_right, bottom_left, bottom_right;
    set_vec2(&top_left, x_min, y_min);
    set_vec2(&top_right, x_max, y_min);
    set_vec2(&bottom_left, x_min, y_max);
    set_vec2(&bottom_right, x_max, y_max);

    float rx_min, rx_max, ry_min, ry_max;
    rx_min = cam_trigger->min_x + cam_offset->x;
    rx_max = cam_trigger->max_x + cam_offset->x;
    ry_min = cam_trigger->min_y + cam_offset->y;
    ry_max = cam_trigger->max_y + cam_offset->y;


    // frame_count++;
    // if(frame_count % 5000 == 0){
    //     printf("%.2f %.2f %.2f %.2f\n", x_min, x_max, y_min, y_max);
    //     printf("%.2f %.2f %.2f %.2f\n", rx_min, rx_max, ry_min, ry_max);

    //     printf("-------------------\ncamera offset\n");
    //     printf("%.2f %.2f\n", cam_offset->x, cam_offset->y);
    //     printf("-------------------\n");
    // }

    if(y_min < ry_min){
        cam_offset->y += y_min - ry_min;
    }
    else if(y_max > ry_max){
        cam_offset->y += y_max - ry_max;
    }

    if(x_min < rx_min){
        cam_offset->x += x_min - rx_min;
    }
    else if(x_max > rx_max){
        cam_offset->x += x_max - rx_max;
    }
}

void draw_background(rafgl_raster_t* raster){
    for(int i=0; i<WIDTH; ++i) for(int j=0;j<HEIGHT;++j){
        pixel_at_m((*raster), i, j).rgba = rafgl_RGB(18, 5, 60);
    }
}

void setup_game(){
    setup_protagonist();
    setup_nch();
    setup_food_shards(prot->ch->offset);
    setup_hmobs();
    int wt = 100*16/9;
    cam_trigger = make_rect2(wt, 100, WIDTH-wt, HEIGHT-100);
}

void clean_game(){
    free_protagonist();
    free_hmobs();
}


void vignette_effect(rafgl_raster_t* raster, vec2_t* center, int starts, float vig_factor, float mx_interp, rafgl_pixel_rgb_t vig_color){
    starts *= starts;
    vig_factor *= vig_factor;
    int dy, dx, dist;
    rafgl_pixel_rgb_t sampled, changed;
    for(int y=0; y < raster->height; ++y){
        int dy = center->y - y;
        for(int x=0; x < raster->width; ++x){
            int dx = center->x - x;
            dist = dy*dy + dx*dx - starts;
            if(dist<=0) continue;
            float t = dist * vig_factor;
            if(t > mx_interp) t = mx_interp;

            sampled = pixel_at_m((*raster), x, y);
            changed.r = (1-t) * sampled.r + t * vig_color.r;
            changed.g = (1-t) * sampled.g + t * vig_color.g;
            changed.b = (1-t) * sampled.b + t * vig_color.b;
            pixel_at_m((*raster), x, y).rgba = changed.rgba;
        }
    }
}

void postfx(rafgl_raster_t* raster){
    rafgl_pixel_rgb_t clr;
    clr.rgba = rafgl_RGB(0, 0, 0);
    vec2_t center;
    cpy_vec2(&center, cam_offset);
    negate_vec2(&center);
    translate_vec2(&center, prot->ch->offset);
    vignette_effect(raster, &center, protagonist_get_size()*1.1, 0.0013 * (WIDTH / 1400.0f), 0.75, clr);
}