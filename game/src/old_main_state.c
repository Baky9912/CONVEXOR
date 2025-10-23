#include <main_state.h>
#include <glad/glad.h>
#include <math.h>

#include <rafgl.h>
#include <stdlib.h>
#include <time.h>
#include "segment.h"
#include "convexhull.h"
#include "concavehull.h"
#include "polygon.h"
#include "camera.h"
#include "food_shards.h"



static int w, h;
//dodajemo jos jedan raster za doge sliku
static rafgl_raster_t raster, doge;
static rafgl_texture_t tex;
// dimenzije se menjaju u main.c

void test_ch(){
    rafgl_pixel_rgb_t px;
    px.rgba = rafgl_RGB(255, 0, 0);
    printf("Testing CH\n");
    convex_hull2_t* ch = ch_make_random(200);
    printf("Made random CH\n");
    set_vec2(ch->offset, 250, 250);
    draw_convex_hull(&raster, ch, px, 5, NULL, 1, 2, 1, make_vec2(0,0));
    print_convex_hull(ch);
}

void test_nch(){
    rafgl_pixel_rgb_t px;
    px.rgba = rafgl_RGB(255, 0, 0);
    printf("Testing NCH\n");
    convex_hull2_t* nch = nch_make_random(150);
    printf("Made random NCH\n");
    set_vec2(nch->offset, 250, 250);
    draw_concave_hull(&raster, nch, px, 5, NULL, 1, 2, 1, make_vec2(0,0));
}

void test(){
    //float dots[][4] = {{200, 200, 400, 400}, {200, 200, 250, 400}, {200, 200, 500, 300}, {200, 200, 500, 400},
    //{200, 200, 250, 500}, {200, 200, 400, 500}};
    //float dots[][4] = {
        //{100, 100, 120, 400}, {100, 100, 400, 200}, {100, 100, 400, 300}, {100, 100, 400, 400},
        //{100, 100, 400, 500}, {100, 100, 400, 600} 
    //};

     float dots[11][2] = {
        {91, 177},
        {69, 10000},
        {-117, 161},
        {-198, 25},
        {-184, -77},
        {-172, -101},
        {-4, -199},
        {72, -186},
        {88, -179},
        {126, -154},
        {158, -121}};


    int n=11;
    for(int i=0; i<n;++i){
        dots[i][0] += 250;
        dots[i][1] += 250;
    }

    for(int i=0; i<n; ++i){
        seg2_t* seg = make_seg2(make_vec2(dots[i][0], dots[i][1]), make_vec2(dots[(i+1)%n][0], dots[(i+1)%n][1]));
        draw_seg(&raster, seg, NULL, 1, 2, 1, make_vec2(0, 0));
        free_seg(seg);
    }

    for(int i=0; i<n; ++i){
        rafgl_pixel_rgb_t px;
        px.rgba = rafgl_RGB(255, 0, 0);
        for(int ix=-2; ix<=2; ix++){
            for(int iy=-2; iy<=2; iy++){
                pixel_at_m(raster, (int)dots[i][0]+ix, (int)dots[i][1]+iy) = px;
            }
        }
    }

}

convex_hull2_t *ch, *ch2;
concave_hull2_t* nch;
rect2_t* cam_trigger;
vec2_t* cam_offset;

void test_overlap(){
    rafgl_pixel_rgb_t px;
    px.rgba = rafgl_RGB(255, 0, 0);
    printf("Testing CH\n");
    ch = ch_make_random(200);
    // ch = ch_make_regular(16, 200, 0);
    printf("Made random CH\n");
    set_vec2(ch->offset, 450, 450);
    draw_convex_hull(&raster, ch, px, 5, NULL, 1, 2, 1, make_vec2(0,0));

    px.rgba = rafgl_RGB(0 , 0, 255);
    printf("Testing NCH\n");
    nch = nch_make_random(150);
    printf("Made random NCH\n");
    set_vec2(nch->offset, 250, 250);
    draw_concave_hull(&raster, nch, px, 5, NULL, 1, 2, 1, make_vec2(0,0));

    ch2 = ch_make_regular(8, 250, FULL_CIRCLE/16);
    ch2->offset = make_vec2(750, 250);

    printf("a\n");
    float pch = polygon_area(ch->v, ch->n, ch->bounding_box);
    float pnch = polygon_area(nch->v, nch->n, nch->bounding_box);
    float pch2 = polygon_area(ch2->v, ch2->n, ch2->bounding_box);
    printf("b\n");
    //print_convex_hull(ch);
    //print_concave_hull(nch);
    float pintr1 = intersection_area_ch_nch(ch, nch);
    float pintr2 = intersection_area_ch_ch(ch, ch2);
    printf("P(CH)=%.0f\n", pch);
    printf("P(NCH)=%.0f\n", pnch);
    printf("P(PCH2)=%.0f\n", pch2);
    printf("P(CH ^ NCH)=%.0f\n", pintr1);
    printf("P(CH ^ CH2)=%.0f\n", pintr2);
    printf("----------------------\n");

    //ch2 = ch_make_random(250);
}


void main_state_init(GLFWwindow *window, void *args, int width, int height)
{

    cam_offset = make_vec2(0, 0);
    int wt = 100*16/9;
    cam_trigger = make_rect2(wt, 100, width-wt, height-100);
    w = width;
    h = height;
    rafgl_raster_init(&raster, w, h);
    for(int i=0; i<w; ++i) for(int j=0;j<h;++j){
        pixel_at_m(raster, i, j).rgba = rafgl_RGB(255,255,255);
    }
    //test();
    //test_ch();
    // test_nch();
    //test();
    test_overlap();  // sets ch and nch up
    setup_food_shards(ch->offset);
/* raster init nam nije potreban ako radimo load from image */
    rafgl_raster_load_from_image(&doge, "res/images/doge.png");
    rafgl_texture_init(&tex);
}
//pomocni flag, za pocetak setovan na 0
int original = 0;

unsigned int f=300;
int dir_x = 1;
vec2_t dir;
int bleeding=0;
void take_input(rafgl_game_data_t *game_data){
    dir.x = dir.y = 0;
    if(game_data->keys_down['w'] || game_data->keys_down['W']) dir.y--;
    if(game_data->keys_down['s'] || game_data->keys_down['S']) dir.y++;
    if(game_data->keys_down['a'] || game_data->keys_down['A']) dir.x--;
    if(game_data->keys_down['d'] || game_data->keys_down['D']) dir.x++;
    normalize_vec2(&dir);
    if(game_data->keys_down[' ']){
        scale_vec2(&dir, 1.2);
    }
    float MAIN_CH_SPEED = 4;
    scale_vec2(&dir, MAIN_CH_SPEED);

    if(game_data->keys_down['j'] || game_data->keys_down['J']){
        scale_convex_hull(ch, 1.001);
    }
}

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    take_input(game_data);
    translate_vec2(ch->offset, &dir);
    f++;
    for(int i=0; i<w; ++i) for(int j=0;j<h;++j){
        pixel_at_m(raster, i, j).rgba = rafgl_RGB(255,255,255);
    }
    rafgl_pixel_rgb_t px;
    px.rgba = rafgl_RGB(255, 0, 0);
    fill_inside_convex(&raster, ch, px, cam_offset);
    draw_convex_hull(&raster, ch, px, 5, NULL, 1, 2, 1, cam_offset);
    px.rgba = rafgl_RGB(0 , 0, 255);
    draw_concave_hull(&raster, nch, px, 5, NULL, 1, 2, 1, cam_offset);
    fill_inside_concave(&raster, nch, cam_offset);
    px.rgba = rafgl_RGB(0 , 255, 0);
    draw_convex_hull(&raster, ch2, px, 5, NULL, 1, 2, 1, cam_offset);
    
    possibly_relocate_shards(ch->offset);
    draw_all_shards(&raster, cam_offset);

    if(f%600==0){
        float pch = polygon_area(ch->v, ch->n, ch->bounding_box);
        float pnch = polygon_area(nch->v, nch->n, nch->bounding_box);
        float pch2 = polygon_area(ch2->v, ch2->n, ch2->bounding_box);
        //print_convex_hull(ch);
        //print_concave_hull(nch);
        float pintr1 = intersection_area_ch_nch(ch, nch);
        float pintr2 = intersection_area_ch_ch(ch, ch2);
        printf("P(CH)=%.0f\n", pch);
        printf("P(NCH)=%.0f\n", pnch);
        printf("P(PCH2)=%.0f\n", pch2);
        printf("P(CH ^ NCH)=%.0f\n", pintr1);
        printf("P(CH ^ CH2)=%.0f\n", pintr2);
        printf("----------------------\n");
    }

    move_camera(ch, cam_offset, cam_trigger);
}

void main_state_render(GLFWwindow *window, void *args)
{
    rafgl_texture_load_from_raster(&tex, original ? &doge : &raster);
    rafgl_texture_show(&tex, 0);
}

void main_state_cleanup(GLFWwindow *window, void *args)
{

}
