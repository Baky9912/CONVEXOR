#include "hostile_mob.h"
#include "rafgl.h"
#include "polygon.h"
#include "protagonist.h"
#include "myrandom.h"
#include "geometry.h"
#include "globals.h"
#include "food_shards.h"
#include <time.h>


hostile_mob_t* hmobs[MAX_HMOBS];
int reserved_food[SHARD_CNT];
int reserved_mob[MAX_HMOBS];

typedef struct {
    int hmob;
    int food;
    float dist;
} tuple_iif_t;

tuple_iif_t t[MAX_HMOBS * SHARD_CNT];


hostile_mob_t* make_hostile_mob(){
    hostile_mob_t* hm = (hostile_mob_t*)malloc(sizeof(hostile_mob_t));
    hm->nch = nch_make_random(135);
    float dist = (float)rand_int(65, 100)/100.0 * WIDTH/2;
    float angle = rand_float() * FULL_CIRCLE;
    //printf("angle=%.2f\n", angle);
    polar_vec2(hm->nch->offset, angle, dist);
    translate_vec2(hm->nch->offset, prot->ch->offset);

    hm->min_size = rafgl_distance2D(0, 0, hm->nch->v[0].x, hm->nch->v[0].y);
    hm->max_size = hm->min_size * 1.5;
    hm->dir = make_vec2(0, 0);
    hm->running = 0;
    hm->speed = 300;
    hm->scale_down_factor = 0.99;
    hm->delusion_factor = 0;
    hm->rotation = (rand_float() > 0.5 ? -1 : 1);
    return hm;
}


void setup_hmobs(){
    for(int i=0; i < MAX_HMOBS; ++i){
        srand((unsigned int)time(NULL) + i);
        hmobs[i] = make_hostile_mob();
    }
}

float hmob_get_size(hostile_mob_t* hm){
    return rafgl_distance2D(0, 0, hm->nch->v[0].x, hm->nch->v[0].y);
}


float hmob_get_area(hostile_mob_t* hm){
    return polygon_area(hm->nch->v, hm->nch->n, hm->nch->bounding_box);
}


int lower_dist_comp(const void *a, const void *b) {
    const tuple_iif_t *ta = (const tuple_iif_t *)a;
    const tuple_iif_t *tb = (const tuple_iif_t *)b;

    if (ta->dist < tb->dist) {
        return -1;
    } else if (ta->dist > tb->dist) {
        return 1;
    } else {
        return 0;
    }
}


int debug_fps = 0;

void hmobs_calculate_dirs(){
    for(int i=0; i < SHARD_CNT; ++i) reserved_food[i] = 0;
    for(int i=0; i < MAX_HMOBS; ++i) reserved_mob[i] = 0;

    float prot_area = protagonist_get_area();
    int danger_i=-1;
    //float dst = rafgl_distance2D(hmobs[0]->nch->offset->x, hmobs[0]->nch->offset->y, prot->ch->offset->x, prot->ch->offset->y);
    //float mx_danger = hmob_get_area(hmobs[0]) / (dst * prot_area);
    float dst, mx_danger=-1;
    float tmp_danger;
    hostile_mob_t* hm;
    vec2_t dir;
    for(int i=0; i < MAX_HMOBS; ++i){
        float hmob_area = hmob_get_area(hmobs[i]) * (1+hmobs[i]->delusion_factor);
        if(hmob_area > prot_area * 1.03){
            dst = rafgl_distance2D(hmobs[i]->nch->offset->x, hmobs[i]->nch->offset->y,
            prot->ch->offset->x, prot->ch->offset->y);
            tmp_danger = hmob_area / (dst * prot_area);
            if(danger_i == -1 || tmp_danger > mx_danger){
                mx_danger = tmp_danger;
                danger_i = i;
            }
        }
    }
    if(danger_i != -1){
        reserved_mob[danger_i] = 1;
        cpy_vec2(&dir, hmobs[danger_i]->nch->offset);
        negate_vec2(&dir);
        translate_vec2(&dir, prot->ch->offset);
        normalize_vec2(&dir);
        cpy_vec2(hmobs[danger_i]->dir, &dir);
    }
    
    int k=0;
    float dist;
    float prot_dist;
    for(int i=0; i < MAX_HMOBS; ++i){
        if(reserved_mob[i]) continue;
        float hmob_area = hmob_get_area(hmobs[i]) * (1+hmobs[i]->delusion_factor);
        for(int j=0; j < SHARD_CNT; ++j){
            cpy_vec2(&dir, hmobs[i]->nch->offset);
            negate_vec2(&dir);
            translate_vec2(&dir, shards[j]->offset);
            dist = intensity_vec2(&dir);
            //prot_dist = rafgl_distance2D(shards[i]->offset->x, shards[i]->offset->y,
               //prot->ch->offset->x, prot->ch->offset->y);
            prot_dist = bounding_box_distance(prot->ch->bounding_box, prot->ch->offset, hmobs[i]->nch->bounding_box, shards[j]->offset);
            
            if(prot_dist < FEAR_RANGE && hmob_area > prot_area){
                continue;
            }
            // TODO ADD FEAR
            t[k].dist = dist;
            t[k].hmob = i;
            t[k].food = j;
            k++;
        }
    }

    

    convex_hull2_t* sh;
    qsort(t, k, sizeof(tuple_iif_t), lower_dist_comp);

    for(int i=0; i<k; ++i){
        if(reserved_food[t[i].food] || reserved_mob[t[i].hmob]) continue;
        reserved_food[t[i].food] = 1;
        reserved_mob[t[i].hmob] = 1;

        hm = hmobs[t[i].hmob];
        sh = shards[t[i].food];

        cpy_vec2(&dir, hm->nch->offset);
        negate_vec2(&dir);
        translate_vec2(&dir, sh->offset);
        normalize_vec2(&dir);
        cpy_vec2(hm->dir, &dir);
    }


    int goalless = 0;

    for(int i=0; i < MAX_HMOBS; ++i){
        if(reserved_mob[i]) continue;
        // nemas cilj samo bezi
        goalless++;
        cpy_vec2(&dir, prot->ch->offset);
        negate_vec2(&dir);
        translate_vec2(&dir, hmobs[i]->nch->offset);
        normalize_vec2(&dir);
        cpy_vec2(hmobs[i]->dir, &dir);
    }
}

void generate_hmob_delusions(){
    for(int i=0; i < MAX_HMOBS; ++i){
        hmobs[i]->delusion_factor = rand_float() / 5 - 0.05;
    }
}


void rotate_hmob(hostile_mob_t* hm, float delta_time){
    for(int i=0; i < hm->nch->n; ++i){
        rotate_vec2(&hm->nch->v[i], hm->rotation * delta_time);
    }
    nch_calculate_bounding_box(hm->nch);
}


void rotate_hmobs(float delta_time){
    for(int i=0; i < MAX_HMOBS; ++i){
        rotate_hmob(hmobs[i], delta_time);
    }
}

float time_since_change = 0;

void move_hmobs(float delta_time){
    time_since_change += delta_time;

    for(int i=0; i < MAX_HMOBS; ++i){
        scale_hmob(hmobs[i], powf(hmobs[i]->scale_down_factor, delta_time));
    }

    if(time_since_change > 0.6){
        generate_hmob_delusions();
        time_since_change = 0;
        hmobs_calculate_dirs();
        // mobovi imaju reflex 600ms
        // TODO dodaj delusion factor
    }
    vec2_t tmp;
    for(int i=0; i < MAX_HMOBS; ++i){
        cpy_vec2(&tmp, hmobs[i]->dir);
        scale_vec2(&tmp, delta_time * hmobs[i]->speed);
        translate_vec2(hmobs[i]->nch->offset, &tmp);
        scale_hmob(hmobs[i], powf(hmobs[i]->scale_down_factor, delta_time));
        //if(frame_count % 60 && i<3){
            //print_vec2(hmobs[i]->dir);
        //}
    }
    
    rotate_hmobs(delta_time);
}


void hmobs_eat_food(){
    for(int i=0; i < MAX_HMOBS; ++i){
        float original_area = hmob_get_area(hmobs[i]);
        for(int j=0; j < SHARD_CNT; ++j){
            float intr = intersection_area_ch_nch(shards[j], hmobs[i]->nch);
            if(intr > shard_area[j] * 0.5){
                float new_area = original_area + shard_gets_eaten(j, prot->ch->offset);
                scale_hmob(hmobs[i], new_area/original_area);
                break; // samo jedan po frameu
            }
        }
    }
}


void scale_hmob(hostile_mob_t* hm, float scale){
    float hmsz = hmob_get_size(hm);
    if(hmsz * scale < hm->min_size){
        scale = hm->min_size / hmsz;
    }
    else if(hmsz * scale > hm->max_size){
        scale = hm->max_size / hmsz; 
    }

    for(int i=0; i < hm->nch->n; ++i){
        scale_vec2(&hm->nch->v[i], scale);
    }
    nch_calculate_bounding_box(hm->nch);
}


void free_hmob(hostile_mob_t* hm){
    free_nch(hm->nch);
    free(hm->dir);
    free(hm);
}


void free_hmobs(){
    for(int i=0; i < MAX_HMOBS; ++i){
        free_hmob(hmobs[i]);
    }
}


void hmob_die(int ind){
    free_hmob(hmobs[ind]);
    hmobs[ind] = make_hostile_mob();
}


void draw_hostile_mobs(rafgl_raster_t* raster, vec2_t* cam_offset){
    for(int i=0; i < MAX_HMOBS; ++i){
        // mozda bolji draw
        fill_inside_concave(raster, hmobs[i]->nch, cam_offset);
    }
}
        


        // hm = hmobs[i];
        // float hmob_area = hmob_get_area(hm) * (1 + hm->delusion_factor);
        // if(hmob_area > prot_area){
        //     cpy_vec2(&dir, hm->nch->offset);
        //     negate_vec2(&dir);
        //     translate_vec2(&dir, prot->ch->offset);
        //     normalize_vec2(&dir);
        //     cpy_vec2(hm->dir, &dir);

        //     reserved_mob[i] = 1;
        // }

        // 1 / (distance / (hmob_area / prot_area))
        // 1 / (distance * prot_are / hmob_area)
        // hmob_area / (distance * prot_area)

