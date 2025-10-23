#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <math.h>
#include <stdlib.h>

typedef struct {float x; float y;} vec2_t;
typedef struct {double k; double n;} line2_eq_t;
typedef struct {vec2_t* p[2];} seg2_t;
typedef struct {seg2_t* seg; line2_eq_t* line_eq;} seg2_eq_t;
typedef struct {float min_y; float max_y; float min_x; float max_x;} rect2_t;
typedef struct {vec2_t* p[3];} tri2_t;

static const float PI = 3.1415926535;
static const float FULL_CIRCLE = 3.1415926535 + 3.1415926535;
static const int ALIGNED = 0;
static const int LEFT = 1;
static const int RIGHT = 2;

void set_tri2(tri2_t* dst, vec2_t* a, vec2_t* b, vec2_t* c);
void cpy_tri2(tri2_t* dst, tri2_t* src);
rect2_t* make_rect2(float min_x, float min_y, float max_x, float max_y);
void set_rect2(rect2_t *r, float min_x, float min_y, float max_x, float max_y);
void print_rect2(rect2_t* r);
vec2_t* make_vec2(float x, float y);
void set_vec2(vec2_t* v, float x, float y);
void cpy_vec2(vec2_t* dst, vec2_t* src);
void cpy_seg2(seg2_t* dst, seg2_t* src);
void negate_vec2(vec2_t* p);
void normalize_vec2(vec2_t* v);
float intensity_vec2(vec2_t* v);
float dot_vec2(vec2_t* a, vec2_t* b);
void rotate_vec2(vec2_t* a, float angle);
void print_vec2(vec2_t* v);
void print_seg2(seg2_t* s);
void set_seg2(seg2_t* s, vec2_t* p0, vec2_t* p1);

vec2_t* add_vec2(vec2_t* a, vec2_t* b);
vec2_t* mult_vec2(vec2_t* a, float mp);
vec2_t* subtract_vec2(vec2_t* a, vec2_t* b);
vec2_t* clone_vec2(const vec2_t* v);
seg2_t* clone_seg2(const seg2_t* s);
float cross_lines_from_same_point(vec2_t* p, vec2_t* q, vec2_t* r);
int orientate(vec2_t* p, vec2_t* q, vec2_t* r);

seg2_t* make_seg2(vec2_t* a, vec2_t* b);
line2_eq_t* make_line2_eq_kn(double k, double n);
line2_eq_t* calc_line2_eq(const seg2_t* s);
float solve_y(line2_eq_t* leq, float x);
float solve_x(line2_eq_t* leq, float y);
float solve_x_nofloaterror(seg2_eq_t* seq, float y);
float solve_y_nofloaterror(seg2_eq_t* seq, float x);
seg2_eq_t* extend_seg2(seg2_t* s);
void free_seg(seg2_t* s);
void free_seq2_eq(seg2_eq_t* s);
vec2_t* intersection(line2_eq_t* l1, line2_eq_t* l2);
vec2_t* intersects(const seg2_t* a, const seg2_t* b);

void scale_vec2_from_point(vec2_t* p, const vec2_t* from, float scale);
void translate_vec2(vec2_t* p, const vec2_t* tr);
void translate_seg2(seg2_t* s, const vec2_t* tr);
void polar_vec2(vec2_t* v, float rad, float intensity);
void scale_vec2(vec2_t* v, float scale);

#endif