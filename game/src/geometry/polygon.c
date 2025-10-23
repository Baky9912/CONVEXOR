#include "polygon.h"


void draw_polygon_hull(rafgl_raster_t* raster, vec2_t* v, int pn, rafgl_pixel_rgb_t dot_style,
int dot_width, rafgl_pixel_rgb_t* style, int style_sz, int width, int angle_width, vec2_t* offset){
    seg2_t s;
    vec2_t v0, v1;
    s.p[0] = &v0;
    s.p[1] = &v1;
    // da ne bi unistavao datu koju imam sad
    for(int i=0; i<pn; ++i){
        set_vec2(&v0, v[i].x, v[i].y);
        set_vec2(&v1, v[(i+1) % pn].x, v[(i+1)% pn].y);
        draw_seg(raster, &s, style, style_sz, width, angle_width, offset);
    }
    // TODO unreturn

    if(dot_width>300) dot_width=300;
    if(dot_width==0) return;
    int ny, nx ;
    int dot_off = (dot_width+1)/2-1;
    cpy_vec2(&v1, offset);
    negate_vec2(&v1);
    for(int i=0; i<pn; ++i){
        set_vec2(&v0, v[i].x, v[i].y);
        translate_vec2(&v0, &v1);
        for(int ix=-dot_off; ix<=dot_off; ix++){
            nx = v0.x + ix;
            if(nx<0 || nx>=raster->width) continue;
            for(int iy=-dot_off; iy<=dot_off; iy++){
                ny = v0.y + iy;
                if(ny<0 || ny>=raster->height) continue;
                pixel_at_m((*raster), nx, ny) = dot_style;
            }
        }
    }
}


float polygon_area(vec2_t* v, int n, rect2_t* bb){
    // shoelace
    // bounding box
    // ccw
    float area = 0;
    float m, h;
    vec2_t a, b;
    vec2_t adjust;
    set_vec2(&adjust, -bb->min_x+10, -bb->min_y+10);
    //printf("adjust\n");
    //print_vec2(&adjust);

    for(int i=0; i<n; ++i){
        // adjust da ne bi bilo pola ispod x/y ose
        cpy_vec2(&a, &v[i]);
        cpy_vec2(&b, &v[(i+1)%n]);
        translate_vec2(&a, &adjust);
        translate_vec2(&b, &adjust);
        // b posle a
        m = (a.x - b.x)*(a.y + b.y);
        //printf("i=%d, m=%.1f\n", i, m);
        area += m;
    }
    return area/2.0;
}


void jarvis_ch(vec2_t* points, int n, vec2_t** out) {
    *out = (vec2_t*)malloc(sizeof(vec2_t)*n);

    int leftmost = 0;
    for (int i = 1; i < n; i++) {
        if (points[i].x < points[leftmost].x)
            leftmost = i;
    }

    int p = leftmost, q;
    int k=0;

    for(int i=0; i<n; ++i){
        cpy_vec2(&((*out)[i]), &points[p]);
        // nadji novo p-q
        int q=0;
        for(int j=1; j<n; ++j){
            if(j==p) continue;
            // svi su levo od p q 
            int ort = orientate(&points[p], &points[q], &points[j]);
            if(ort!=LEFT){
                q=j;
            }
        }
        p=q;
    }
}


int bounding_boxes_intersect(convex_hull2_t* ch1, convex_hull2_t* ch2){
    float x_min1 = ch1->bounding_box->min_x + ch1->offset->x;
    float x_max1 = ch1->bounding_box->max_x + ch1->offset->x;
    float y_min1 = ch1->bounding_box->min_y + ch1->offset->y;
    float y_max1 = ch1->bounding_box->max_y + ch1->offset->y;

    float x_min2 = ch2->bounding_box->min_x + ch2->offset->x;
    float x_max2 = ch2->bounding_box->max_x + ch2->offset->x;
    float y_min2 = ch2->bounding_box->min_y + ch2->offset->y;
    float y_max2 = ch2->bounding_box->max_y + ch2->offset->y;

    int ch2x_in_ch1x = (x_min1 <= x_min2 && x_min2 <= x_max1)
    || (x_min1 <= x_max2 && x_max2 <= x_max1);

    int ch2y_in_ch1y = (y_min1 <= y_min2 && y_min2 <= y_max1)
    || (y_min1 <= y_max2 && y_max2 <= y_max1);

    if(ch2x_in_ch1x && ch2y_in_ch1y) return 1;

    int ch1x_in_ch2x = (x_min2 <= x_min1 && x_min1 <= x_max2)
    || (x_min2 <= x_max1 && x_max1 <= x_max2);

    int ch1y_in_ch2y = (y_min2 <= y_min1 && y_min1 <= y_max2)
    || (y_min2 <= y_max1 && y_max1 <= y_max2);

    if(ch1x_in_ch2x && ch1y_in_ch2y) return 1;
    if(ch1x_in_ch2x && ch2y_in_ch1y) return 1;
    if(ch2x_in_ch1x && ch1y_in_ch2y) return 1;
    return 0;
}


static float minf(float a, float b){
    if(a<b)return a;
    return b;
}


float bounding_box_distance(rect2_t* r1, vec2_t* off1, rect2_t* r2, vec2_t* off2){
    convex_hull2_t tmp1, tmp2;
    tmp1.offset = off1;
    tmp1.bounding_box = r1;
    tmp2.offset = off2;
    tmp2.bounding_box = r2;
    if(bounding_boxes_intersect(&tmp1, &tmp2)){
        return 0.0f;
    }

    float x_min1 = tmp1.bounding_box->min_x + tmp1.offset->x;
    float x_max1 = tmp1.bounding_box->max_x + tmp1.offset->x;
    float y_min1 = tmp1.bounding_box->min_y + tmp1.offset->y;
    float y_max1 = tmp1.bounding_box->max_y + tmp1.offset->y;

    float x_min2 = tmp2.bounding_box->min_x + tmp2.offset->x;
    float x_max2 = tmp2.bounding_box->max_x + tmp2.offset->x;
    float y_min2 = tmp2.bounding_box->min_y + tmp2.offset->y;
    float y_max2 = tmp2.bounding_box->max_y + tmp2.offset->y;

    float dist_x=1e9, dist_y=1e9;

    if(x_min1 <= x_min2 && x_min2 <= x_max1) dist_x = 0;
    if(x_min1 <= x_max2 && x_max2 <= x_max1) dist_x = 0;
    if(x_min2 <= x_min1 && x_min1 <= x_max2) dist_x = 0;
    if(x_min2 <= x_max1 && x_max1 <= x_max2) dist_x = 0;
    dist_x = minf(dist_x, fabs(x_min1 - x_min2));
    dist_x = minf(dist_x, fabs(x_min1 - x_max2));
    dist_x = minf(dist_x, fabs(x_max1 - x_min2));
    dist_x = minf(dist_x, fabs(x_max1 - x_max2));
    
    if(y_min1 <= y_min2 && y_min2 <= y_max1) dist_y = 0;
    if(y_min1 <= y_max2 && y_max2 <= y_max1) dist_y = 0;
    if(y_min2 <= y_min1 && y_min1 <= y_max2) dist_y = 0;
    if(y_min2 <= y_max1 && y_max1 <= y_max2) dist_y = 0;
    dist_y = minf(dist_y, fabs(y_min1 - y_min2));
    dist_y = minf(dist_y, fabs(y_min1 - y_max2));
    dist_y = minf(dist_y, fabs(y_max1 - y_min2));
    dist_y = minf(dist_y, fabs(y_max1 - y_max2));


    return sqrtf(dist_x*dist_x + dist_y*dist_y);
}


float intersection_area_ch_ch(convex_hull2_t* ch1, convex_hull2_t* ch2){
    if(!bounding_boxes_intersect(ch1, ch2)){
        //printf("BOUNDING BOXES CHECK FAILED\n");
        return 0;
    }

    vec2_t *p1, *p2, *p3;
    int n1, n2, n3;
    //printf("before points in ch\n");
    points_in_ch(ch1, ch2, &p1, &n1);
    points_in_ch(ch2, ch1, &p2, &n2);
    //printf("before segment intersection\n");
    segment_intersections(ch1->v, ch1->n, ch1->offset, ch2->v, ch2->n, ch2->offset, &p3, &n3);
    //printf("after segment intersection\n");

    // printf("n1=%d, n2=%d n3=%d\n", n1, n2, n3);

    //for(int i=0; i<n1; ++i) print_vec2(&p1[i]);
    //for(int i=0; i<n2; ++i) print_vec2(&p2[i]);
    //for(int i=0; i<n3; ++i) print_vec2(&p3[i]);

    //printf("H1\n");

    int n = n1+n2+n3;

    if(n<=1){
        free(p1);
        free(p2);
        free(p3);
        return 0;
    }
    else if(n==2){
        printf("N=2 presek error nemoguce!!!\n");
        printf("n1=%d, n2=%d, n3=%d\n", n1, n2, n3);
        printf("ch1->n = %d, ch2->n = %d\n", ch1->n, ch2->n);
        return 0;
    }

    vec2_t* p = (vec2_t*)malloc(sizeof(vec2_t)*n);

    for(int i=0; i<n1; ++i) cpy_vec2(&p[i], &p1[i]);
    for(int i=0; i<n2; ++i) cpy_vec2(&p[n1+i], &p2[i]);
    for(int i=0; i<n3; ++i) cpy_vec2(&p[n1+n2+i], &p3[i]);

    free(p1);
    free(p2);
    free(p3);

    // napravi CH od p
    vec2_t* all_ch;
    jarvis_ch(p, n, &all_ch);
    convex_hull2_t fch; 
    fch.v = all_ch;
    fch.n = n;
    ch_calculate_bounding_box(&fch);
    //print_concave_hull(&fch);

    float area = polygon_area(fch.v, n, fch.bounding_box);

    free(fch.v);
    free(fch.bounding_box);
    return area;
}


float intersection_area_ch_nch(convex_hull2_t* ch, concave_hull2_t* nch){
    convex_hull2_t tmp;
    tmp.bounding_box = nch->bounding_box;
    tmp.offset = nch->offset;
    if(!bounding_boxes_intersect(ch, &tmp)){
        return 0;
    }

    float area = 0;
    //printf("tris_n=%d\n", nch->tris_n);
    for(int i=0; i<nch->tris_n; ++i){
        //printf("nch->tris[i].p[0]\n");
        //print_vec2(nch->tris[i].p[0]);
        area += intersection_area_ch_tri(ch, &nch->tris[i], nch->offset);
    }
    return area;
}

float intersection_area_ch_tri(convex_hull2_t* ch, tri2_t* tri, vec2_t* tri_off){
    convex_hull2_t ch2;
    ch2.n = 3;
    ch2.v = (vec2_t*)malloc(sizeof(vec2_t)*3);
    for(int i=0; i<3; ++i){
        cpy_vec2(&ch2.v[i], tri->p[i]);
    }
    ch2.offset = tri_off;
    ch_calculate_bounding_box(&ch2);

    float area = intersection_area_ch_ch(ch, &ch2);

    //free(ch2.offset);
    //free(ch2.v);

    return area;
}


void points_in_ch(convex_hull2_t* ch1, convex_hull2_t* ch2, vec2_t** out, int* outn){
    // tacke iz ch2 u ch1
    int mx = ch2->n;
    *out = (vec2_t*)malloc(sizeof(vec2_t)*mx);
    int k=0;
    vec2_t v, v2;
    vec2_t minus_off;
    vec2_t acc_off;
    cpy_vec2(&acc_off, ch1->offset);
    negate_vec2(&acc_off);
    translate_vec2(&acc_off, ch2->offset);

    for(int i=0; i<ch2->n; ++i){
        cpy_vec2(&v, &ch2->v[i]);
        translate_vec2(&v, &acc_off);

        if(ch_point_inside(ch1, &v)){
            //printf("POINT INSIDE CH!\n");
            cpy_vec2(&v2, &ch2->v[i]);
            translate_vec2(&v2, ch2->offset);
            cpy_vec2(&((*out)[k++]), &v2);
        }
    }
    //*out = (vec2_t*)realloc(*out, sizeof(vec2_t)*k);
    *outn = k;
}


void segment_intersections_util(seg2_t* a, int n, vec2_t* off_a, seg2_t* b, int m, vec2_t* off_b, vec2_t** out, int* outn){
    //printf("-");
    int mx = 2*((n > m) ? n : m);
    *out = (vec2_t*)malloc(sizeof(vec2_t)*mx);
    int k=0;

    vec2_t* p;
    seg2_t ai, bj;
    vec2_t v0, v1, v2, v3;
    ai.p[0] = &v0;
    ai.p[1] = &v1;
    bj.p[0] = &v2;
    bj.p[1] = &v3;


    for(int i=0; i<n; ++i){
        //print_seg2(&a[i]);
        cpy_seg2(&ai, &a[i]);
        translate_seg2(&ai, off_a);

        for(int j=0; j<m; ++j){
            cpy_seg2(&bj, &b[j]);
            translate_seg2(&bj, off_b);

            p = intersects(&ai, &bj);
            if(p!=NULL){
                //print_seg2(&ai);
                //print_vec2(p);
                //printf("INTERSECTION FOUND\n");
                //print_vec2(p);
                cpy_vec2(&((*out)[k++]), p);
                free(p);
            }
        }
    }

    //*out = (vec2_t*)realloc(*out, sizeof(vec2_t)*k);
    *outn = k;
}

// moze nlogn

void segment_intersections(vec2_t* a, int n, vec2_t* off_a, vec2_t* b, int m, vec2_t* off_b, vec2_t** out, int* outn){
    // SVI OVI PRINTOVI POSTOJE ZBOG FLOATING POINT PRECIZION ERRORA OD KOG NISAM SPAVAO
    seg2_t* as = (seg2_t*)malloc(sizeof(seg2_t)*n);
    for(int i=0; i<n; ++i){
        as[i].p[0] = &a[i];
        as[i].p[1] = &a[(i+1) % n];
    }

    seg2_t* bs = (seg2_t*)malloc(sizeof(seg2_t)*m);
    for(int i=0; i<m; ++i){
        bs[i].p[0] = &b[i];
        bs[i].p[1] = &b[(i+1) % m];
        //print_vec2(&b[i]);
    }

    //printf("n=%d, m=%d\n", n, m);
    //printf("---------------------\n");
    segment_intersections_util(as, n, off_a, bs, m, off_b, out, outn);

    free(as);
    free(bs);
}
