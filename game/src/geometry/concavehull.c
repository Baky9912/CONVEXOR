#include "concavehull.h"
#include "myrandom.h"
#include "geometry.h"
#include "segment.h"
#include "convexhull.h"
#include "globals.h"
#include <rafgl.h>
#include <math.h>

static int floatcomp(const void *a, const void *b) {
    float x = (*(float*)a - *(float*)b);
    if(x<0) return -1;
    if(x>0) return 1;
    return 0;
}

concave_hull2_t* nch_make_random(float exsize){
    concave_hull2_t* nch;
    while(1){
        nch = (concave_hull2_t*)malloc(sizeof(concave_hull2_t));
        nch->offset = (vec2_t*)malloc(sizeof(vec2_t));
        nch->n = rand_int(6, 14);
        nch->v = (vec2_t*)malloc(sizeof(vec2_t)*nch->n);
        float* rads = (float*)malloc(sizeof(float)*nch->n);
        float* ints = (float*)malloc(sizeof(float)*nch->n);
        // inteziteti ^
        for(int i=0; i<nch->n; ++i){
            rads[i] = rand_float() * FULL_CIRCLE;
            ints[i] = (rand_float()+0.5) * exsize;
        }
        qsort(rads, nch->n, sizeof(float), floatcomp);
        // nema veze sto ne vuce ints svakako su zasebne logike
        for(int i=0; i<nch->n; ++i){
            polar_vec2(&nch->v[i], rads[i], ints[i]);
        }
        free(rads);
        free(ints);
        nch_calculate_bounding_box(nch);
        if(nch_triangulate(nch)){
            break;
        }
        free_nch(nch);
        seed++;
        srand(seed);
        // nekad ne moze da triangulise na ovaj nacin pa samo napravim novi 
    }
    return nch;  // ccw
}

void free_nch(concave_hull2_t* nch){
    free(nch->offset);
    free(nch->bounding_box);
    for(int i=0; i<nch->tri_lines_n;++i){
        free(nch->tri_lines[i]);
        // segmenti koje je on napravio
    }
    free(nch->tri_lines);
    free(nch->tris);
    free(nch->v);
    free(nch);
}

void nch_calculate_bounding_box(concave_hull2_t* nch){
    // moze u O(logn) ali ce samo jednom da se izvrsi tako da nije potebno za ovaj sistem
    nch->bounding_box = (rect2_t*)malloc(sizeof(rect2_t));
    nch->bounding_box->min_x = nch->v[0].x;
    nch->bounding_box->max_x = nch->v[0].x;
    nch->bounding_box->min_y = nch->v[0].y;
    nch->bounding_box->max_y = nch->v[0].y;

    for(int i=1; i<nch->n; ++i){
        nch->bounding_box->min_x = ((nch->v[i].x < nch->bounding_box->min_x) ? nch->v[i].x : nch->bounding_box->min_x);
        nch->bounding_box->max_x = ((nch->v[i].x > nch->bounding_box->max_x) ? nch->v[i].x : nch->bounding_box->max_x);
        nch->bounding_box->min_y = ((nch->v[i].y < nch->bounding_box->min_y) ? nch->v[i].y : nch->bounding_box->min_y);
        nch->bounding_box->max_y = ((nch->v[i].y > nch->bounding_box->max_y) ? nch->v[i].y : nch->bounding_box->max_y);
    }
}

int nch_point_inside(concave_hull2_t* nch, vec2_t* p){
    seg2_t infline;
    seg2_t ei;
    infline.p[0] = p;
    infline.p[1]->y = p->y;
    infline.p[1]->x = 1e10;

    int intr = 0;
    for(int i=0; i<nch->n; ++i){
        if(nch->v[i].y == p->y) continue;
        // specijalni slucaj gde infline sece kroz tacku direktno pa se racuna kao da su secene 2 prave
        set_seg2(&ei, &nch->v[i], &nch->v[(i+1) % nch->n]);
        if(intersects(&infline, &ei)!=NULL){
            intr++;
        }
    }
    return (intr%1);
}

void draw_concave_hull(rafgl_raster_t* raster, concave_hull2_t* nch, rafgl_pixel_rgb_t dot_style,
int dot_width, rafgl_pixel_rgb_t* style, int style_sz, int width, int angle_width, vec2_t* offset){
    // printf("Drawing NCH of size N=%d\n********\n", nch->n);
    vec2_t* real_v = (vec2_t*)malloc(sizeof(vec2_t)*nch->n);
    for(int i=0; i<nch->n; ++i){
        cpy_vec2(&real_v[i], &nch->v[i]);
        translate_vec2(&real_v[i], nch->offset);
    }
    draw_polygon_hull(raster, real_v, nch->n, dot_style, dot_width, style,
    style_sz, width, angle_width, offset);
    free(real_v);
    
    //printf("**********************\n");
    //printf("nch->tri_lines_n=%d\n", nch->tri_lines_n);
    //printf("**********************\n");

    seg2_t ps;
    vec2_t v0, v1;
    ps.p[0] = &v0, ps.p[1] = &v1;
    for(int i=0; i<nch->tri_lines_n; ++i){
        cpy_vec2(&v0, nch->tri_lines[i]->p[0]);
        cpy_vec2(&v1, nch->tri_lines[i]->p[1]);
        translate_vec2(&v0, nch->offset);
        translate_vec2(&v1, nch->offset);

        draw_seg(raster, &ps, style, style_sz, width, angle_width, offset);
    }
}

int nch_triangulate(concave_hull2_t* nch){
    // ovo radi samo zato sto su generisani na nacin na koji su gore generisani
    nch->tris = (tri2_t*)malloc(sizeof(tri2_t) * nch->n); 
    nch->tri_lines = (seg2_t**)malloc(sizeof(seg2_t*) * 2*nch->n + 8); 
    nch->tris_n = 0;
    nch->tri_lines_n = 0;

    vec2_t** ap = (vec2_t**)malloc(sizeof(vec2_t*) * nch->n);
    //vec2_t** nap = (vec2_t**)malloc(sizeof(vec2_t*) * nch->n);
    int an = nch->n;
    //int nan = an;

    //printf("T0\n");

    for(int i=0; i<nch->n; ++i){
        //printf("**\n");
        ap[i] = &nch->v[i];
        //printf("##\n");
        nch->tri_lines[nch->tri_lines_n++] = make_seg2(&nch->v[i], &nch->v[(i+1) % nch->n]);
    }

    //printf("T1\n");

    int found_any = 1;
    int out;
    int rnd;
    tri2_t tr;
    int* orts = (int*)malloc(sizeof(int) * nch->n);
    int all_left;
    while(an >= 3){
        out = -1;
        rnd = rand_int(0, an-1);

        all_left = 1;       
        for(int i=0; i<an; ++i){
            tr.p[0] = ap[(i)%an];
            tr.p[1] = ap[(i+1)%an];
            tr.p[2] = ap[(i+2)%an];
            orts[i] = orientate(tr.p[0], tr.p[1], tr.p[2]);
            if(!(orts[i]!=RIGHT)) all_left=0;
        }

        for(int i=0; i<an; ++i){
            tr.p[0] = ap[(i+rnd)%an];
            tr.p[1] = ap[(i+1+rnd)%an];
            tr.p[2] = ap[(i+2+rnd)%an];
            if((all_left || orts[(i-1+rnd+an)%an]!=LEFT) && orts[(i+rnd)%an]!=RIGHT){
                out = (i+1+rnd)%an;
                cpy_tri2(&nch->tris[nch->tris_n++], &tr);
                nch->tri_lines[nch->tri_lines_n++] = make_seg2(tr.p[0], tr.p[2]);
                break;
            }
        }
        if(out==-1){
            free(orts);
            free(ap);
            return 0;
        }
        for(int i=out; i<an-1; ++i){
            ap[i] = ap[i+1];
        }
        an--;
    }
    
    free(orts);
    free(ap);
    return 1;
}

void print_concave_hull(concave_hull2_t* nch){
    printf("N=%d\n", nch->n);
    printf("offset\n");
    print_vec2(nch->offset);
    printf("bounding_box\n");
    print_rect2(nch->bounding_box);
    printf("tacke\n");
    for(int i=0; i<nch->n; ++i){
        print_vec2(&nch->v[i]);
    }
}

rafgl_pixel_rgb_t NCH_TRI_COLORS[4];

void setup_nch(){
    // #CEF4FF
    // #1A535C
    // #1D1128
    // #3C91E6
    NCH_TRI_COLORS[0].rgba = rafgl_RGB(0xCE, 0xF4, 0xFF);
    NCH_TRI_COLORS[1].rgba = rafgl_RGB(0x1A, 0x53, 0x5C);
    NCH_TRI_COLORS[2].rgba = rafgl_RGB(0x1D, 0x11, 0x28);
    NCH_TRI_COLORS[3].rgba = rafgl_RGB(0x3c, 0x91, 0xE6);
}

void fill_inside_concave(rafgl_raster_t* raster, concave_hull2_t* nch, vec2_t* cam_offset){
    convex_hull2_t ch;
    ch.n = 3;
    ch.v = (vec2_t*)malloc(sizeof(vec2_t)*3);
    ch.offset = nch->offset;
    for(int i=0; i < nch->tris_n; ++i){
        for(int t=0; t<3; ++t){
            cpy_vec2(&ch.v[t], nch->tris[i].p[t]);
        }
        ch_calculate_bounding_box(&ch);

        fill_inside_convex(raster, &ch, NCH_TRI_COLORS[i % 4], cam_offset);
    }
    free(ch.v);
}


void scale_concave_hull(convex_hull2_t* nch, float scale){
    for(int i=0; i < nch->n; ++i){
        scale_vec2(&nch->v[i], scale);
    }
}

/*
    while(nan>=4){
        an = nan;
        for(int i=0; i<an; ++i){
            ap[i] = nap[i];
        }
        nan = 0;

        if(found_any==0){
            printf("DIDNT FIND ANY WITH NAN>3\n");
            break;
        }

        int i;
        int intrs = 0;
        found_any = 0;
        for(i=0; i<an-1; ++i){
            nap[nan++] = ap[i];

            tr.p[0] = ap[i];
            tr.p[1] = ap[i+1];
            tr.p[2] = ap[(i+2) % an];

            if(orientate(tr.p[0], tr.p[1], tr.p[2]) != RIGHT){
                ts = make_seg2(tr.p[0], tr.p[2]);
                intrs = 0;
                for(int j=0; j<nch->tri_lines_n; ++j){
                    if(intersects(ts, nch->tri_lines[j])!=NULL) intrs++;
                }
                printf("INTRS=%d\n", intrs);
                if(intrs > 4) continue;

                cpy_tri2(&nch->tris[nch->tris_n++], &tr);
                nch->tri_lines[nch->tri_lines_n++] = ts;
                ++i;
                found_any=1;
            }
        }
        if(i<an) nap[nan++] = ap[an-1];
    }

*/