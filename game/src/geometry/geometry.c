#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "geometry.h"
#include "globals.h"


// TODO make inline

rect2_t* make_rect2(float min_x, float min_y, float max_x, float max_y){
    rect2_t* r = (rect2_t*)malloc(sizeof(rect2_t));
    r->max_x = max_x;
    r->max_y = max_y;
    r->min_x = min_x;
    r->min_y = min_y;
    return r;
}


void set_rect2(rect2_t *r, float min_x, float min_y, float max_x, float max_y){
    r->max_x = max_x;
    r->max_y = max_y;
    r->min_x = min_y;
    r->min_y = min_y;
}


vec2_t* make_vec2(float x, float y){
    vec2_t* p = (vec2_t*)malloc(sizeof(vec2_t));
    p->x = x;
    p->y = y;
}


void set_vec2(vec2_t* v, float x, float y){
    v->x = x, v->y = y;
}


void cpy_vec2(vec2_t* dst, vec2_t* src){
    dst->x = src->x;
    dst->y = src->y;
}

/*
vec2_t make_vec2_s(float x, float y){
    return {x, y};
}

vec2_t add_vec2_s(const vec2_t a, const vec2_t b){
    return {a.x+b.x, a.y+b.y};
}

vec2_t subtract_vec2_s(const vec2_t a, const vec2_t b){
    return {a.x-b.x, a.y-b.y};
}

vec2_t mult_vec2_s(vec2_t a, float mp){
    return {a.x * mp, a.y * mp};
}

vec2_t make_vec2_s(float x1, float y1, float x2, float y2){
    return {x2-x1, y2-y1};
}
*/

vec2_t* add_vec2(vec2_t* a, vec2_t* b){
    vec2_t* c = (vec2_t*)malloc(sizeof(vec2_t));
    c->x = a->x + b->x;
    c->y = a->y + b->y;
    return c;
}

vec2_t* mult_vec2(vec2_t* a, float mp){
    vec2_t* c = (vec2_t*)malloc(sizeof(vec2_t));
    c->x = a->x * mp;
    c->y = a->y * mp;
    return c;
}


vec2_t* subtract_vec2(vec2_t* a, vec2_t* b){
    vec2_t* c = (vec2_t*)malloc(sizeof(vec2_t));
    c->x = a->x - b->x;
    c->y = a->y - b->y;
    return c;
}


vec2_t* clone_vec2(const vec2_t* v){
    vec2_t* nv = (vec2_t*)malloc(sizeof(vec2_t));
    nv->x = v->x;
    nv->y = v->y;
    return nv;
}


seg2_t* clone_seg2(const seg2_t* s){
    seg2_t* ns = (seg2_t*)malloc(sizeof(seg2_t));
    ns->p[0] = clone_vec2(s->p[0]);
    ns->p[1] = clone_vec2(s->p[1]);
    return ns;
}

/*
P, Q, R
| PQx PQy |
| PRx PRy |
R levo desno ili na PQ pravi

(Qx-Px)*(Ry-Py)-(Rx-Px)*(Qy-Py)
Qx*Ry - Qx*Py - Px*Ry + Px*Py - Rx*Qy + Rx*Py + Px*Qy - Px*Py
Qx*Ry - Qx*Py - Px*Ry - Rx*Qy + Rx*Py + Px*Qy

Qx*Ry - Qx*Py - Rx*Qy + Rx*Py ||
Qx*Ry - Rx*Qy |||

Px*(Qy-Ry) + Py(Rx-Qx) + Qx*Ry - Rx*Qy 

manje mnozenja

*/
float cross_lines_from_same_point(vec2_t* p, vec2_t* q, vec2_t* r){
    // return p->x*(q->y - r->y) + p->y*(r->x - q->x) + q->x * r->y - r->x * q->y;

    float a11 = (q->x - p->x);
    float a12 = (r->x - p->x);
    float a21 = (q->y - p->y);
    float a22 = (r->y - p->y);
    return (a11*a22)-(a12*a21);
}


int orientate(vec2_t* p, vec2_t* q, vec2_t* r){
    float tmp = cross_lines_from_same_point(p, q, r);
    // if (fabs(tmp) < EPS_G){
    //     return ALIGNED;
    //     // da float greske ne bi narusile rad programa
    // }
    if(tmp==0) return ALIGNED;
    if(tmp>0) return LEFT;
    return RIGHT;
}


seg2_t* make_seg2(vec2_t* a, vec2_t* b){
    seg2_t* s = (seg2_t*)malloc(sizeof(seg2_t));
    s->p[0] = a;
    s->p[1] = b;
    return s;
}


void print_vec2(vec2_t* v){
    printf("(%.2f %.2f)\n", v->x, v->y);
}


void print_rect2(rect2_t* r){
    printf("min_x=%.1f, max_x=%.1f, min_y=%.1f, max_y=%.1f\n", r->min_x, r->max_x, r->min_y, r->max_y);
}


void print_seg2(seg2_t* s){
    print_vec2(s->p[0]);
    print_vec2(s->p[1]);
}


line2_eq_t* make_line2_eq_kn(double k, double n){
    line2_eq_t* leq = (line2_eq_t*)malloc(sizeof(line2_eq_t));
    leq->k = k, leq->n = n;
    return leq;
}


line2_eq_t* calc_line2_eq(const seg2_t* s){
    double x1 = s->p[0]->x,
    x2 = s->p[1]->x,
    y1 = s->p[0]->y,
    y2 = s->p[1]->y;
    double k = (y2-y1)/(x2-x1);
    /*
    y = kx + n
    E(x1, y1)
    y1 = k*x1 + n
    y1 - k*x1 = n
    */
    double n = y1 - k*x1;
    return make_line2_eq_kn(k, n);
}


seg2_eq_t* extend_seg2(seg2_t* s){
    seg2_eq_t* seq = (seg2_eq_t*)malloc(sizeof(seg2_eq_t));
    seq->line_eq = calc_line2_eq(s);
    return seq;
}


void set_seg2(seg2_t* s, vec2_t* p0, vec2_t* p1){
    s->p[0] = p0;
    s->p[1] = p1;
}


void free_seg(seg2_t* s){
    free(s->p[0]);
    free(s->p[1]);
    free(s);
}


void free_seq2_eq(seg2_eq_t* s){
    free(s->line_eq);
    free_seg(s->seg);
    free(s);
}


vec2_t* intersection(line2_eq_t* l1, line2_eq_t* l2){
    /*
    (yi, xi)
    k1*xi+n1 = k2*xi+n2
    x = (n2-n1)/(k1-k2)
    */
    float x = (l2->n - l1->n)/(l1->k - l2->k);
    float y = l1->k * x + l1->n;
    return make_vec2(x, y);
}


vec2_t* intersects(const seg2_t* a, const seg2_t* b){
    vec2_t* i = intersection(calc_line2_eq(a), calc_line2_eq(b));
    float xa1 = a->p[0]->x, xa2 = a->p[1]->x;
    float xb1 = b->p[0]->x, xb2 = b->p[1]->x;

    float tmp;
    if(xa1 > xa2){
        tmp = xa1;
        xa1 = xa2;
        xa2 = tmp;
    }
    if(xb1 > xb2){
        tmp = xb1;
        xb1 = xb2;
        xb2 = tmp;
    }
    
    if (xa1 - EPS_G <= i->x && i->x <= xa2 + EPS_G && xb1 - EPS_G <= i->x && i->x <= xb2 + EPS_G){
        return i;
    }
    return NULL;
}


void scale_vec2_from_point(vec2_t* p, const vec2_t* from, float scale){
    vec2_t v;
    p->x = from->x + (p->x - from->x)*scale;
    p->y = from->y + (p->y - from->y)*scale;
}


void translate_vec2(vec2_t* p, const vec2_t* tr){
    p->x += tr->x;
    p->y += tr->y;
}


void translate_seg2(seg2_t* s, const vec2_t* tr){
    translate_vec2(s->p[0], tr);
    translate_vec2(s->p[1], tr);
}


void cpy_seg2(seg2_t* dst, seg2_t* src){
    cpy_vec2(dst->p[0], src->p[0]);
    cpy_vec2(dst->p[1], src->p[1]);
}


void negate_vec2(vec2_t* p){
    p->x = -p->x;
    p->y = -p->y;
}


void normalize_vec2(vec2_t* v){
    float mag = intensity_vec2(v);
    if(mag<=EPS_G) return;
    v->x /= mag;
    v->y /= mag;
}


float intensity_vec2(vec2_t* v){
    return sqrtf(v->x*v->x + v->y*v->y);
}


float dot_vec2(vec2_t* a, vec2_t* b){
    return a->x * b->x + a->y * b->y;
}

void rotate_vec2(vec2_t* a, float angle){
    vec2_t b;
    b.x = a->x * cosf(angle) - a->y * sinf(angle);
    b.y = a->x * sinf(angle) + a->y * cosf(angle);
    cpy_vec2(a, &b);
}


void polar_vec2(vec2_t* v, float rad, float intensity){
    v->x = cosf(rad) * intensity;
    v->y = sinf(rad) * intensity;
}


void scale_vec2(vec2_t* v, float scale){
    v->x *= scale;
    v->y *= scale;
}


void set_tri2(tri2_t* dst, vec2_t* a, vec2_t* b, vec2_t* c){
    // ne kopira vec2 val
    dst->p[0] = a;
    dst->p[1] = b;
    dst->p[2] = c;
}


void cpy_tri2(tri2_t* dst, tri2_t* src){
    dst->p[0] = src->p[0];
    dst->p[1] = src->p[1];
    dst->p[2] = src->p[2];
}


float solve_y(line2_eq_t* leq, float x){
    return x*leq->k + leq->n;
}


float solve_x(line2_eq_t* leq, float y){
    return (y - leq->n) / leq->k;
}


float solve_x_nofloaterror(seg2_eq_t* seq, float y){
    if(fabs(seq->line_eq->k) < 0.0005){
        return seq->seg->p[0]->x;
    }
    return solve_x(seq->line_eq, y);
}


float solve_y_nofloaterror(seg2_eq_t* seq, float x){
    return solve_y(seq->line_eq, x);
}