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
#include "protagonist.h"
#include "hostile_mob.h"


static rafgl_texture_t tex;
static rafgl_raster_t raster, doge;



void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    rafgl_texture_init(&tex);
    rafgl_raster_init(&raster, WIDTH, HEIGHT);
    draw_background(&raster);
    rafgl_raster_load_from_image(&doge, "res/images/doge.png");

    setup_game();
}

float all_time = 0;

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    take_input(game_data);
    move_protagonist(delta_time);
    move_hmobs(delta_time);

    hmobs_eat_food();
    protagonist_eat_food();
    protagonist_fight_enemies();
    possibly_relocate_shards(prot->ch->offset);
    
    draw_background(&raster);
    draw_all_shards(&raster, cam_offset);
    draw_hostile_mobs(&raster, cam_offset);
    draw_protagonist(&raster, &doge);
    
    move_camera(prot->ch, cam_offset, cam_trigger);

    postfx(&raster);
    frame_count++;
    all_time += delta_time;
    if(frame_count%120==0){
        printf("FPS=%.2f\n", frame_count / all_time);
    }
}



void main_state_render(GLFWwindow *window, void *args)
{
    rafgl_texture_load_from_raster(&tex, &raster);
    rafgl_texture_show(&tex, 0);
}

void main_state_cleanup(GLFWwindow *window, void *args)
{

}
