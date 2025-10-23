#include "convexhull.h"
#include "myrandom.h"
#include "globals.h"
#include "segment.h"
#include "polygon.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>


int floatcomp(const void *a, const void *b) {
    float x = (*(float*)a - *(float*)b);
    if(x<0) return -1;
    if(x>0) return 1;
    return 0;
}

static void print_floats(float* a, int n){
    for(int i=0; i<n; ++i){
        printf("%.2f, ", a[i]);
    }
    printf("\b\b  \n");
}

void print_convex_hull(convex_hull2_t* ch){
    printf("N=%d\n", ch->n);
    printf("offset\n");
    print_vec2(ch->offset);
    printf("bounding_box\n");
    print_rect2(ch->bounding_box);
    printf("tacke\n");
    for(int i=0; i<ch->n; ++i){
        print_vec2(&ch->v[i]);
    }
}


convex_hull2_t* ch_make_regular(int vertex, float size, float rotation){
    convex_hull2_t* ch = (convex_hull2_t*)malloc(sizeof(convex_hull2_t));
    ch->offset = (vec2_t*)malloc(sizeof(vec2_t));
    ch->offset->x = ch->offset->y = 0;
    ch->n = vertex;

    ch->v = (vec2_t*)malloc(sizeof(vec2_t)*ch->n);

    float rad = rotation - FULL_CIRCLE*floor(rotation/FULL_CIRCLE);
    //float rad = rotation;
    float rad_incr = FULL_CIRCLE / ch->n;
    for(int i=0; i<ch->n; ++i){
        if(rad>=FULL_CIRCLE) rad -= FULL_CIRCLE;
        polar_vec2(&ch->v[i], rad, size);
        rad += rad_incr;
    }
    ch_calculate_bounding_box(ch);
    return ch;
}


convex_hull2_t* ch_make_random(float exsize){
    srand((unsigned int)time(NULL));

    convex_hull2_t* ch = (convex_hull2_t*)malloc(sizeof(convex_hull2_t));
    ch->offset = (vec2_t*)malloc(sizeof(vec2_t));
    ch->offset->x = ch->offset->y = 0;
    ch->n = rand_int(6, 14);
    ch->v = (vec2_t*)malloc(sizeof(vec2_t)*ch->n);
    float* rads = (float*)malloc(sizeof(float)*ch->n);
    for(int i=0; i<ch->n; ++i){
        rads[i] = rand_float() * FULL_CIRCLE;
    }
    qsort(rads, ch->n, sizeof(float), floatcomp);
    print_floats(rads, ch->n);
    for(int i=0; i<ch->n; ++i){
        polar_vec2(&ch->v[i], rads[i], exsize);
    }
    free(rads);
    ch_calculate_bounding_box(ch);
    return ch;  // ccw
}

void free_ch(convex_hull2_t* ch){
    free(ch->offset);
    free(ch->bounding_box);
    free(ch->v);
}

void ch_calculate_bounding_box(convex_hull2_t* ch){
    // moze u O(logn) ali ce samo jednom da se izvrsi tako da nije potebno za ovaj sistem
    ch->bounding_box = (rect2_t*)malloc(sizeof(rect2_t));
    ch->bounding_box->min_x = ch->v[0].x;
    ch->bounding_box->max_x = ch->v[0].x;
    ch->bounding_box->min_y = ch->v[0].y;
    ch->bounding_box->max_y = ch->v[0].y;

    for(int i=1; i<ch->n; ++i){
        ch->bounding_box->min_x = ((ch->v[i].x < ch->bounding_box->min_x) ? ch->v[i].x : ch->bounding_box->min_x);
        ch->bounding_box->max_x = ((ch->v[i].x > ch->bounding_box->max_x) ? ch->v[i].x : ch->bounding_box->max_x);
        ch->bounding_box->min_y = ((ch->v[i].y < ch->bounding_box->min_y) ? ch->v[i].y : ch->bounding_box->min_y);
        ch->bounding_box->max_y = ((ch->v[i].y > ch->bounding_box->max_y) ? ch->v[i].y : ch->bounding_box->max_y);
    }
}

int ch_point_inside(convex_hull2_t* ch, vec2_t* p){
    // TODO 
    // O(logn) moguce, svi ch->n su jako mali tako da nema potrebe ako ne napravim vece konveksne poligone
    for(int i=0; i<ch->n; ++i){
        if(orientate(&ch->v[i], &ch->v[(i+1) % ch->n], p)==RIGHT)
            return 0;
    }
    return 1;
}

void draw_convex_hull(rafgl_raster_t* raster, convex_hull2_t* ch, rafgl_pixel_rgb_t dot_style,
int dot_width, rafgl_pixel_rgb_t* style, int style_sz, int width, int angle_width, vec2_t* offset){
    // printf("Drawing CH of size N=%d\n********\n", ch->n);
    vec2_t* real_v = (vec2_t*)malloc(sizeof(vec2_t)*ch->n);
    //vec2_t real_v[ch->n];
    for(int i=0; i<ch->n; ++i){
        cpy_vec2(&real_v[i], &ch->v[i]);
        translate_vec2(&real_v[i], ch->offset);
    }
    draw_polygon_hull(raster, real_v, ch->n, dot_style, dot_width, style,
    style_sz, width, angle_width, offset);
    free(real_v);
}


void scale_convex_hull(convex_hull2_t* ch, float scale){
    for(int i=0; i<ch->n; ++i){
        scale_vec2(&ch->v[i], scale);
    }
    ch_calculate_bounding_box(ch);
}


static int tallest_point(convex_hull2_t* ch){
    // ako ima vise u liniji too bad!
    float ymin = ch->v[0].y;
    int mni = 0;
    for(int i=1; i<ch->n; ++i){
        if(ch->v[i].y < ymin){
            ymin = ch->v[i].y;
            mni = i;
        }
    }
    return mni;
}


static int leftest_point(convex_hull2_t* ch){
    // ako ima vise u liniji too bad!
    float xmin = ch->v[0].x;
    int mni = 0;
    for(int i=1; i<ch->n; ++i){
        if(ch->v[i].y < xmin){
            xmin = ch->v[i].x;
            mni = i;
        }
    }
    return mni;
}


void fill_inside_convex(rafgl_raster_t* raster, convex_hull2_t* ch, rafgl_pixel_rgb_t color, vec2_t* cam_offset){
    // ccw
    // poznati bug regular 7 convex imao levi outburst za size 28??
    convex_hull2_t tmp_ch;
    tmp_ch.offset = cam_offset;
    tmp_ch.bounding_box = make_rect2(0, 0, WIDTH-1, HEIGHT-1);
    int intr = bounding_boxes_intersect(ch, &tmp_ch);
    free(tmp_ch.bounding_box);
    if(!intr) return;

    int ymax = floorf(ch->bounding_box->max_y);
    int t = tallest_point(ch);
    int n = ch->n;
    seg2_eq_t l, r;
    int lp = (t+1)%n, rp = (t-1+n)%n;


    l.seg = make_seg2(&ch->v[t], &ch->v[lp]);
    l.line_eq = calc_line2_eq(l.seg);
    r.seg = make_seg2(&ch->v[t], &ch->v[rp]);
    r.line_eq = calc_line2_eq(r.seg);
    // moze diskretno kao draw easy is ali nije prevelika optimizacija zato sto cu svakako da prolazim kroz puno pixela u for petljama da iscrtam stvari

    int y = roundf(ch->bounding_box->min_y);
    int xmin, xmax;
    int off_x = roundf(ch->offset->x);
    int off_y = roundf(ch->offset->y);
    int ry;
    int tmp;
    for(y; y<=ymax; ++y){
        if(y > ch->v[lp].y){
            tmp = lp;
            lp = (lp+1)%n;
            free(l.seg);
            free(l.line_eq);
            l.seg = make_seg2(&ch->v[tmp], &ch->v[lp]);
            l.line_eq = calc_line2_eq(l.seg);
        }
        if(y > ch->v[rp].y){
            tmp = rp;
            rp = (rp-1+n)%n;
            free(r.seg);
            free(r.line_eq);
            r.seg = make_seg2(&ch->v[tmp], &ch->v[rp]);
            r.line_eq = calc_line2_eq(r.seg);
        }


        ry = y + off_y - cam_offset->y;
        if(ry<0 || ry >= raster->height) continue;

        //xmax = ceilf(solve_x(l.line_eq, y)); // round?
        //xmin = floorf(solve_x(r.line_eq, y)); // round?
        xmax = ceilf(solve_x_nofloaterror(&l, y));
        xmin = floorf(solve_x_nofloaterror(&r, y));

        xmin += off_x - cam_offset->x;
        xmax += off_x - cam_offset->x;

        if(xmin < 0) xmin=0; 
        if(xmax >= raster->width) xmax = raster->width - 1;

        for(int rx=xmin; rx<=xmax; ++rx){
            pixel_at_m((*raster), rx, ry) = color;
        }
    }
    
    free(l.seg);
    free(l.line_eq);
    free(r.seg);
    free(r.line_eq);
}


static rafgl_pixel_rgb_t interp_color(rafgl_pixel_rgb_t a, rafgl_pixel_rgb_t b, float t){
    rafgl_pixel_rgb_t c;
    c.r = (1-t)*a.r + t*b.r;
    c.g = (1-t)*a.g + t*b.g;
    c.b = (1-t)*a.b + t*b.b;
    return c;
}


void draw_raster_inside_convex(rafgl_raster_t* raster, convex_hull2_t* ch, rafgl_raster_t* img, vec2_t* cam_offset){
    // ccw

    rafgl_raster_t scaled_img;
    int wi, hi;

    convex_hull2_t tmp_ch;
    tmp_ch.offset = cam_offset;
    tmp_ch.bounding_box = make_rect2(0, 0, WIDTH-1, HEIGHT-1);
    int intr = bounding_boxes_intersect(ch, &tmp_ch);
    free(tmp_ch.bounding_box);
    if(!intr) return;

    int ymax = floorf(ch->bounding_box->max_y);
    int t = tallest_point(ch);
    int n = ch->n;
    seg2_eq_t l, r;
    int lp = (t+1)%n, rp = (t-1+n)%n;


    l.seg = make_seg2(&ch->v[t], &ch->v[lp]);
    l.line_eq = calc_line2_eq(l.seg);
    r.seg = make_seg2(&ch->v[t], &ch->v[rp]);
    r.line_eq = calc_line2_eq(r.seg);
    // moze diskretno kao draw easy is ali nije prevelika optimizacija zato sto cu svakako da prolazim kroz puno pixela u for petljama da iscrtam stvari

    int y = roundf(ch->bounding_box->min_y);
    int xmin, xmax;
    int off_x = roundf(ch->offset->x);
    int off_y = roundf(ch->offset->y);
    int ry;
    int tmp;

    hi = ymax - y;
    wi = floorf(ch->bounding_box->max_x) - roundf(ch->bounding_box->min_x);

    // moze da se gurne u svoju funkciju
    rafgl_raster_init(&scaled_img, wi+1, hi+1);
    rafgl_pixel_rgb_t p11, p12, p21, p22;
    rafgl_pixel_rgb_t pup, pdown, pjoint;
    float scale_x = (float)img->width / (float)wi;
    float scale_y = (float)img->height / (float)hi;
    float img_x, img_y;
    int x1, x2, y1, y2;
    float yt, xt;
    for(int yi=0; yi<hi; ++yi){
        img_y = yi * scale_y;
        yt = img_y - (int)img_y;
        y1 = floorf(img_y + EPS_G);
        y2 = ceil(img_y - EPS_G);
        for(int xi=0; xi<wi; ++xi){
            img_x = xi * scale_x;
            xt = img_x - (int)img_x;
            x1 = floorf(img_x + EPS_G);
            x2 = ceil(img_x - EPS_G);
            p11 = pixel_at_m((*img), x1, y1);
            p12 = pixel_at_m((*img), x1, y2);
            p21 = pixel_at_m((*img), x2, y1);
            p22 = pixel_at_m((*img), x2, y2);

            pup = interp_color(p11, p21, xt);
            pdown = interp_color(p12, p22, xt);
            pjoint = interp_color(pup, pdown, yt);
            pixel_at_m(scaled_img, xi, yi) = pjoint;
        }
    }

    // for(int yi=0; yi<hi; ++yi){
    //     for(int xi=0; xi<wi; ++xi){
    //         pixel_at_m((*raster), xi, yi) = pixel_at_m(scaled_img, xi, yi);
    //     }
    // }
    // return;

    int bb_xmin = ch->bounding_box->min_x;
    int bb_ymin = y;
    int img_rx, img_ry;

    for(y; y<=ymax; ++y){
        if(y > ch->v[lp].y){
            tmp = lp;
            lp = (lp+1)%n;
            free(l.seg);
            free(l.line_eq);
            l.seg = make_seg2(&ch->v[tmp], &ch->v[lp]);
            l.line_eq = calc_line2_eq(l.seg);
        }
        if(y > ch->v[rp].y){
            tmp = rp;
            rp = (rp-1+n)%n;
            free(r.seg);
            free(r.line_eq);
            r.seg = make_seg2(&ch->v[tmp], &ch->v[rp]);
            r.line_eq = calc_line2_eq(r.seg);
        }


        ry = y + off_y - cam_offset->y;
        if(ry<0 || ry >= raster->height) continue;

        xmax = floorf(solve_x_nofloaterror(&l, y));  // ceil?
        xmin = floorf(solve_x_nofloaterror(&r, y));       

        img_rx = xmin - bb_xmin;
        img_ry = y - bb_ymin;

        xmin += off_x - cam_offset->x;
        xmax += off_x - cam_offset->x;

        if(xmin < 0) xmin=0; 
        if(xmax >= raster->width) xmax = raster->width - 1;

        for(int rx=xmin; rx<=xmax; ++rx){
            if(img_rx <= 0) img_rx = 0;
            if(img_rx > wi) img_rx = wi-1;
            if(img_ry > hi) img_ry = hi-1;
            if(img_ry <= 0) img_ry = 0;
            pixel_at_m((*raster), rx, ry) = pixel_at_m(scaled_img, img_rx, img_ry);
            img_rx++;
        }
    }
    
    free(l.seg);
    free(l.line_eq);
    free(r.seg);
    free(r.line_eq);
}


/*
seg2_t s;
    vec2_t v0, v1;
    s.p[0] = &v0;
    s.p[1] = &v1;
    // da ne bi unistavao datu koju imam sad
    for(int i=0; i<ch->n; ++i){
        set_vec2(&v0, ch->v[i].x, ch->v[i].y);
        set_vec2(&v1, ch->v[(i+1) % ch->n].x, ch->v[(i+1)% ch->n].y);
        translate_vec2(&v0, ch->offset);
        translate_vec2(&v1, ch->offset);
        draw_seg(raster, &s, style, style_sz, width, angle_width, offset);
    }

    if(dot_width>300) dot_width=300;
    int ny, nx ;
    int dot_off = (dot_width+1)/2-1;
    for(int i=0; i<ch->n; ++i){
        set_vec2(&v0, ch->v[i].x, ch->v[i].y);
        translate_vec2(&v0, ch->offset);
        for(int ix=-dot_off; ix<=dot_off; ix++){
            nx = v0.x + ix;
            for(int iy=-dot_off; iy<=dot_off; iy++){
                ny = v0.y + iy;
                pixel_at_m((*raster), nx, ny) = dot_style;
            }
        }
    }

*/