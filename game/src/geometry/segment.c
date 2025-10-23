#include <rafgl.h>
#include <stdlib.h>
#include "segment.h"
#include "globals.h"

static const int INSIDE_CC = 0; // 0000
static const int LEFT_CC = 1; // 0001
static const int RIGHT_CC = 2; // 0010
static const int BOTTOM_CC = 4; // 0100
static const int TOP_CC = 8; // 1000

int compute_code(vec2_t* p, rect2_t* r){
    int code = INSIDE_CC;
    if (p->x < r->min_x)
        code |= LEFT_CC;
    else if (p->x > r->max_x) 
        code |= RIGHT_CC;
    if (p->y < r->min_y) 
        code |= BOTTOM_CC;
    else if (p->y > r->max_y) 
        code |= TOP_CC;
    return code;
}

static int fps_cnt1 = 0;

seg2_t* clip_seg_rect(seg2_t* seg, rect2_t* r){
    fps_cnt1++;

    int outcode[2];
    outcode[0] = compute_code(seg->p[0], r);
    outcode[1] = compute_code(seg->p[1], r);
    line2_eq_t* line_eq = calc_line2_eq(seg);
    // prava se nikad ne menja, samo se ogranicava

    if(outcode[0] & outcode[1]){
        // na istoj strani su obe, ne mogu da seku viewport
        return NULL;
    }
    seg2_t* res = clone_seg2(seg);
    int i;
    for(;;){
        outcode[0] = compute_code(res->p[0], r);
        outcode[1] = compute_code(res->p[1], r);
        if(outcode[0]==INSIDE_CC && outcode[1]==INSIDE_CC){
            res->p[0]->x -= r->min_x;
            res->p[1]->x -= r->min_x;
            res->p[0]->y -= r->min_y;
            res->p[1]->y -= r->min_y;
            
            //printf("********************\nreturning seg2?\n");
            //print_seg2(res);
            //printf("****************\n");
            return res;  // vraca spreman za crtanje bez offseta
        }
        else if(outcode[0] & outcode[1]){
            // ne moze da se udje u ovo? za svaki slucaj je kodirano, zaokruzivanja isl
            free_seg(res);
            return NULL;
        }

        i=0;
        if(outcode[i]==0) i=1;
        
        if(outcode[i] & TOP_CC){
            // zavisi kako gledano top i bottom bi bili naopaki
            // presek sa p[i]-p[1-i] sa r.max_y horizontalom
            // p[1-i] je ispod
            // y = kx + n
            // x = (y-n)/k
            res->p[i]->x = (r->max_y - line_eq->n)/line_eq->k;
            res->p[i]->y = r->max_y;
        }
        else if(outcode[i] & BOTTOM_CC){
            res->p[i]->x = (r->min_y - line_eq->n)/line_eq->k;
            res->p[i]->y = r->min_y;
        }
        else if(outcode[i] & LEFT_CC){
            res->p[i]->y = line_eq->k * r->min_x + line_eq->n;
            res->p[i]->x = r->min_x;
        }
        else if(outcode[i] & RIGHT_CC){
            res->p[i]->y = line_eq->k * r->max_x + line_eq->n;
            res->p[i]->x = r->max_x;
        }
    }
}

rafgl_pixel_rgb_t black;
rafgl_pixel_rgb_t default_style[1];

void draw_seg(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width, int angle_width, vec2_t* offset)
{
    // angle width, da li je width po stvarnoj debljini (1) ili po sirini / visini (0)
    black.rgba = rafgl_RGB(0, 0, 0);
    default_style[0] = black;

    
    // style je niz
    if(style==NULL){
        style = default_style;
        style_sz = 1;
    }
    rect2_t* rect = make_rect2(offset->x, offset->y, offset->x + raster->width-1, offset->y + raster->height-1);
    //printf("clippin against\n((%.2f %.2f) (%.2f %.2f))\n-------------\n",
        //rect->min_x, rect->min_y, rect->max_x, rect->max_y);

    seg2_t* rdy_seg = clip_seg_rect(seg, rect);
    if(rdy_seg==NULL){
        free(rect);
        return;
    }

    if(rdy_seg->p[0]->x > rdy_seg->p[1]->x){
        vec2_t* tmp = rdy_seg->p[0];
        rdy_seg->p[0] = rdy_seg->p[1];
        rdy_seg->p[1] = tmp;
    }

    //print_seg2(seg);
    //printf("-----------\n clipped\n");
    //print_seg2(rdy_seg);
    //printf("-----------");

    if(angle_width && abs(roundf(rdy_seg->p[0]->x) - roundf(rdy_seg->p[1]->x)) >= 1){
        // ovo je unistavalo width vertikalama, hvala trigonometrijo

        float alpha = atanf(fabsf(rdy_seg->p[1]->y - rdy_seg->p[0]->y) / (rdy_seg->p[1]->x - rdy_seg->p[0]->x));
        float wf = width;
        //printf("BEFORE: %d\n", width);
        //printf("cos(alpha)=%.2f, alpha=%.2f\n", cosf(alpha), alpha);
        width = roundf(fminf(fabsf(wf/cosf(alpha)), fabsf(wf/sinf(alpha)))+0.1);
        if(width>20){
            printf("WIDTH=%d\n", width);
        }
        //printf("AFTER: %d\n", width);
        //printf("------------------\n");
    }

    if(roundf(rdy_seg->p[0]->x) == roundf(rdy_seg->p[1]->x)){
        draw_vertical(raster, rdy_seg, style, style_sz, width);
    }
    else if(fabs(rdy_seg->p[1]->y - rdy_seg->p[0]->y) <= rdy_seg->p[1]->x - rdy_seg->p[0]->x){
        if(rdy_seg->p[1]->y > rdy_seg->p[0]->y){
            // printf("draw east up\n");
            draw_east_up(raster, rdy_seg, style, style_sz, width);
        }
        else{
            // printf("draw east up\n");
            draw_east_down(raster, rdy_seg, style, style_sz, width);
        }
    }
    else if(rdy_seg->p[1]->y > rdy_seg->p[0]->y){
        // printf("draw north\n");
        draw_north(raster, rdy_seg, style, style_sz, width);
    }
    else{
        // printf("draw south\n");
        draw_south(raster, rdy_seg, style, style_sz, width);
    }
    free_seg(rdy_seg);
    free(rect);
}


// TODO inline
static void draw_point_horizontal(rafgl_raster_t* raster, rafgl_pixel_rgb_t color, int width, int x, int y){
    int up=1;
    int fail_up=0;
    int fail_down=0;
    // da veliki width ne bi zaglavio (tipa 100000) ovo clampuje width na raster

    int up_y = y;
    int down_y = y+1;
    for(int i=0; i<width && (!fail_up || !fail_down); ++i){
        if(up){
            if (fail_up || up_y < 0){
                fail_up = 1;
            }
            else{
                pixel_at_m((*raster), x, up_y--) = color;
                up=0;
            }
        }
        else{
            if(fail_down || down_y >= raster->height){
                fail_down = 1;
            }
            else{
                pixel_at_m((*raster), x, down_y++) = color;
                up=1;
            }
        }
    }
}

static void draw_point_vertical(rafgl_raster_t* raster, rafgl_pixel_rgb_t color, int width, int x, int y){
    int up=1;
    int fail_left=0;
    int fail_right=0;
    // da veliki width ne bi zaglavio (tipa 100000) ovo clampuje width na raster

    int left_x = x;
    int right_x = x+1;
    for(int i=0; i<width && (!fail_left || !fail_right); ++i){
        if(up){
            if (fail_left || left_x < 0){
                fail_left = 1;
            }
            else{
                pixel_at_m((*raster), left_x--, y) = color;
                up=0;
            }
        }
        else{
            if(fail_right || right_x >= raster->width){
                fail_right = 1;
            }
            else{
                pixel_at_m((*raster), right_x++, y) = color;
                up=1;
            }
        }
    }
}

void draw_east_up(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width){
    int x1 = roundf(seg->p[0]->x);
    int x2 = roundf(seg->p[1]->x);
    int y1 = roundf(seg->p[0]->y);
    int y2 = roundf(seg->p[1]->y);
    
    int dx = x2-x1;
    int dy = y2-y1;
    int lambda = 2*dy-dx;
    int incr1 = 2*dy;
    int incr2 = 2*(dy-dx);

    int style_i=0;

    int x = x1;
    int y = y1;
    draw_point_horizontal(raster, style[0], width, x, y);
    while(x < x2){
        x++;
        if(lambda <= 0){
            lambda += incr1;
        }
        else{
            lambda += incr2;
            y++;
        }
        style_i++;
        draw_point_horizontal(raster, style[style_i % style_sz], width, x, y);
    }
}

void draw_east_down(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width){
    int x1 = roundf(seg->p[0]->x);
    int x2 = roundf(seg->p[1]->x);
    int y1 = roundf(seg->p[0]->y);
    int y2 = roundf(seg->p[1]->y);
    
    int dx = x2-x1;
    int dy = y1-y2;
    int lambda = 2*dy-dx;
    int incr1 = 2*dy;
    int incr2 = 2*(dy-dx);

    int style_i=0;

    int x = x1;
    int y = y1;
    draw_point_horizontal(raster, style[0], width, x, y);
    while(x < x2){
        x++;
        if(lambda <= 0){
            lambda += incr1;
        }
        else{
            lambda += incr2;
            y--;
        }
        style_i++;
        draw_point_horizontal(raster, style[style_i % style_sz], width, x, y);
    }
}

void draw_north(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width){
    int x1 = roundf(seg->p[0]->x);
    int x2 = roundf(seg->p[1]->x);
    int y1 = roundf(seg->p[0]->y);
    int y2 = roundf(seg->p[1]->y);
    
    int dx = x2-x1;
    int dy = y2-y1;
    int lambda = 2*dx-dy;
    int incr1 = 2*dx;
    int incr2 = 2*(dx-dy);

    int style_i=0;

    int x = x1;
    int y = y1;
    draw_point_vertical(raster, style[0], width, x, y);
    while(y < y2){
        y++;
        if(lambda <= 0){
            lambda += incr1;
        }
        else{
            lambda += incr2;
            x++;
        }
        style_i++;
        draw_point_vertical(raster, style[style_i % style_sz], width, x, y);
    }
}

void draw_south(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width){
    int x1 = roundf(seg->p[0]->x);
    int x2 = roundf(seg->p[1]->x);
    int y1 = roundf(seg->p[0]->y);
    int y2 = roundf(seg->p[1]->y);
    
    int dx = x2-x1;
    int dy = y1-y2;  // naopako
    int lambda = 2*dx-dy;
    int incr1 = 2*dx;
    int incr2 = 2*(dx-dy);

    int style_i=0;

    int x = x1;
    int y = y1;
    draw_point_vertical(raster, style[0], width, x, y);
    while(y > y2){
        y--;
        if(lambda <= 0){
            lambda += incr1;
        }
        else{
            lambda += incr2;
            x++;
        }
        style_i++;
        draw_point_vertical(raster, style[style_i % style_sz], width, x, y);
    }
}

void draw_vertical(rafgl_raster_t* raster, seg2_t* seg, rafgl_pixel_rgb_t* style, int style_sz, int width){
    int x = roundf(seg->p[0]->x);
    int ymin = roundf(seg->p[0]->y);
    int ymax = roundf(seg->p[1]->y);
    int tmp;
    if(ymin > ymax){
        tmp = ymin;
        ymin = ymax;
        ymax = tmp;
    }

    int style_i = 0;
    //printf("Draw vertical\n");
    //printf("x=%d, ymin=%d, ymax=%d, width=%d\n", x, ymin, ymax, width);
    for(int y=ymin; y<=ymax; ++y){
        draw_point_vertical(raster, style[style_i % style_sz], width, x, y);
        //pixel_at_m((*raster), x, y) = style[0];
        style_i++;
    }
}
