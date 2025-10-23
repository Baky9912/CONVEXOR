#include "protagonist.h"
#include "convexhull.h"
#include "food_shards.h"
#include "polygon.h"
#include "hostile_mob.h"
#include "camera.h"
#include "globals.h"


protagonist_t* prot;

void setup_protagonist(){
    prot = (protagonist_t*)malloc(sizeof(protagonist_t));
    prot->ch = ch_make_random(120);
    set_vec2(prot->ch->offset, WIDTH/2, HEIGHT/2);
    prot->min_size = rafgl_distance2D(0, 0, prot->ch->v[0].x, prot->ch->v[0].y);
    prot->max_size = 250;
    prot->dir = make_vec2(0, 0);
    prot->running = 0;
    prot->speed = 300;
    prot->scale_down_factor = 0.99;
    prot->rotating = 0;
}


float protagonist_get_size(){
    return rafgl_distance2D(0, 0, prot->ch->v[0].x, prot->ch->v[0].y);
}


float protagonist_get_area(){
    return polygon_area(prot->ch->v, prot->ch->n, prot->ch->bounding_box);
}


void scale_protagonist(float scale){
    scale_convex_hull(prot->ch, scale);
    if(protagonist_get_size() > prot->max_size){
        scale_convex_hull(prot->ch, prot->max_size / protagonist_get_size());
    }
    if(protagonist_get_size() < prot->min_size){
        scale_convex_hull(prot->ch, prot->min_size / protagonist_get_size());
    }
}

void protagonist_eat_food(){
    for(int i=0; i < SHARD_CNT; ++i){
        float intr = intersection_area_ch_ch(prot->ch, shards[i]);
        if(intr > shard_area[i] * 0.5){
            float original_area = protagonist_get_area(prot);
            float new_area = original_area + shard_gets_eaten(i, prot->ch->offset);
            scale_protagonist(new_area / original_area);
            break; // samo jedan po frameu
        }
    }
}

void protagonist_fight_enemies(){
    hostile_mob_t* hm;
    float prot_area = protagonist_get_area();
    for(int i=0; i < MAX_HMOBS; ++i){
        hm = hmobs[i];
        float intr = intersection_area_ch_nch(prot->ch, hm->nch);
        float hmob_area = 0;
        if(intr != 0.0f) hmob_area = hmob_get_area(hm);

        // printf("prot_area = %.2f\n", prot_area);
        // printf("intr = %.2f\n", intr);
        // printf("hmob_area = %.2f\n", hmob_area);
        // printf("----------------------\n");

        if(intr != 0.0f && prot_area < hmob_area && intr >= prot_area * KILL_OVERLAP){
            printf("protagonist died\n");
            clean_game();
            setup_game();
            break;
        }
        else if(intr != 0.0f && prot_area > hmob_area && intr >= hmob_area * KILL_OVERLAP){
            printf("protagonist killed\n");
            hmob_die(i);
            float original_area = protagonist_get_area(prot);
            float new_area = original_area + hmob_area * KILL_OVERLAP * 0.5;
            scale_convex_hull(prot->ch, new_area/original_area);
            break;
        }
    }
}

void take_input(rafgl_game_data_t *game_data){
    protagonist_t* p = prot;
    p->dir->x = p->dir->y = 0;
    if(game_data->keys_down['w'] || game_data->keys_down['W']) p->dir->y -= 1;
    if(game_data->keys_down['s'] || game_data->keys_down['S']) p->dir->y += 1;
    if(game_data->keys_down['a'] || game_data->keys_down['A']) p->dir->x -= 1;
    if(game_data->keys_down['d'] || game_data->keys_down['D']) p->dir->x += 1;
    normalize_vec2(p->dir);

    if(game_data->keys_down['k'] || game_data->keys_down['K']) p->rotating = 1;
    else p->rotating = 0;

    if(game_data->keys_down[' ']) p->running = 1;
    else p->running = 0;

    if(game_data->keys_down['j'] || game_data->keys_down['J']){
        scale_convex_hull(p->ch, 1.001);
    }
}


void move_protagonist(float delta_time){
    vec2_t tmp;
    cpy_vec2(&tmp, prot->dir);
    scale_vec2(&tmp, delta_time * prot->speed);
    if(prot->running){
        if(fabs(protagonist_get_size() - prot->min_size) < EPS_G){
            scale_vec2(&tmp, 1.2);
        }
        else{
            scale_vec2(&tmp, 1.6);
        }
    }
    translate_vec2(prot->ch->offset, &tmp);
    if(protagonist_get_size() > prot->min_size){
        float added_scale_down_factor = 0;
        if(prot->running) added_scale_down_factor = 0.06;
        float s = powf(prot->scale_down_factor - added_scale_down_factor, delta_time);

        if(protagonist_get_size() * s < prot->min_size){
            s = prot->min_size / protagonist_get_size();
        }
        scale_protagonist(s);
    }

    if(prot->rotating){
        for(int i=0; i < prot->ch->n; ++i){
            rotate_vec2(&prot->ch->v[i], 1 * delta_time);
        }
        ch_calculate_bounding_box(prot->ch);
    }
    else{
        for(int i=0; i < prot->ch->n; ++i){
            rotate_vec2(&prot->ch->v[i], -1 * delta_time);
        }
        ch_calculate_bounding_box(prot->ch);
    }
}

rafgl_pixel_rgb_t outline_colors[6];
rafgl_pixel_rgb_t red;

void draw_protagonist(rafgl_raster_t* raster, rafgl_raster_t* img){
    red.rgba = rafgl_RGB(255, 0, 0);
    for(int i=0; i<3; ++i){
        outline_colors[i].rgba = rafgl_RGB(20, 20, 255);
        outline_colors[i+3].rgba = rafgl_RGB(0, 0, 0);
    }

    // TODO zameni ovaj fill
    // fill_inside_convex(raster, prot->ch, red, cam_offset);
    draw_raster_inside_convex(raster, prot->ch, img, cam_offset);
    draw_convex_hull(raster, prot->ch, outline_colors[0], 4, outline_colors, 6, 6, 1, cam_offset);
    
}

void free_protagonist(){
    free_ch(prot->ch);
    free(prot->dir);
    free(prot);
}