#include <raylib.h>
#include <math.h>
#include "background.h"
#include "helper_functions.h"
#include "config.h"
#include "timer.h"
#include "scenes/game.h"

#define MAX_SAND_PARTICLES 20
#define SAND_SPAWN_RATE 0.15 // in seconds, lower is faster
#define SLOW_WAVE_HEIGHT 12
#define SLOW_WAVE_WIDTH 8
#define FAST_WAVE_HEIGHT 20
#define FAST_WAVE_WIDTH 15

extern bool  is_game_paused;

struct SandParticle {
        Vector2 pos;
        bool    active;
} sand_particles[MAX_SAND_PARTICLES] = { 0 };

void draw_ocean(void) {
    Color ocean_color = { 66, 147, 255, 255 };
    if (get_is_slowing_down()) invert_color(&ocean_color);
    DrawRectangle(0, 250, SCREEN_WIDTH, FLOOR_HEIGHT - 250, ocean_color);

    Color colors[4] = {
        { 173, 216, 230, 255 },
        { 135, 206, 250, 255 },
        { 135, 206, 235, 255 },
        { 70,  130, 180, 255 },
    };

    if (get_is_slowing_down()) {
        for (int i = 0; i < 4; i++) {
            invert_color(&colors[i]);
        }
    }

    static float slow_wave_step = 0;
    static float fast_wave_step = 0;

    if (!is_game_paused) {
        slow_wave_step += get_current_object_speed() != 0 ? 0.002f * get_current_object_speed() : 0.01f;
        fast_wave_step += get_current_object_speed() != 0 ? 0.004f * get_current_object_speed() : 0.02f;
    }

    const int center_y = SCREEN_HEIGHT / 2;
    for (int row = 0; row < 2; row++) {
        int y = center_y + (row * SLOW_WAVE_HEIGHT);
        for (int x = 0; x < SCREEN_WIDTH; x += SLOW_WAVE_WIDTH) {
            const float offsetY = sinf((x + slow_wave_step * 100) / 50.0f) * SLOW_WAVE_HEIGHT / 2;
            DrawRectangle(x, y + (int)offsetY, SLOW_WAVE_WIDTH, SLOW_WAVE_HEIGHT, colors[row]);
        }
    }

    for (int row = 0; row < 2; row++) {
        int y = center_y + (row * FAST_WAVE_HEIGHT);
        for (int x = 0; x < SCREEN_WIDTH; x += FAST_WAVE_WIDTH) {
            const float offsetY = sinf((x + fast_wave_step * 100) / 50.0f) * FAST_WAVE_HEIGHT / 2 + 45;
            DrawRectangle(x, y + (int)offsetY, FAST_WAVE_WIDTH, FAST_WAVE_HEIGHT, colors[row + 2]);
        }
    }
}

void draw_sand_particles(void) {
    static bbox_timer_t sand_particle_spawn_timer;
    update_timer(&sand_particle_spawn_timer);
    Color sand_particle_color = BROWN;
    if (get_is_slowing_down()) invert_color(&sand_particle_color);

    for (int i = 0; i < MAX_SAND_PARTICLES; i++) {
        if (sand_particles[i].active) {
            DrawRectangleV(sand_particles[i].pos, (Vector2){ 7, 7 }, sand_particle_color);

            if (is_game_paused) continue;

            sand_particles[i].pos.x -= get_current_object_speed() != 0 ? get_current_object_speed() : 5;

            if (sand_particles[i].pos.x < 0) {
                sand_particles[i].active = false;
            }

        } else {
            if (is_game_paused) continue;
            if (!sand_particle_spawn_timer.is_running && GetRandomValue(0, 300) == 0) {
                start_timer(&sand_particle_spawn_timer, 0.15);
                sand_particles[i].pos    = (Vector2){ SCREEN_WIDTH, GetRandomValue(FLOOR_HEIGHT + 22, SCREEN_HEIGHT - 22) };
                sand_particles[i].active = true;
            }
        }
    }
}

void draw_background(void) {
    Color sky_color = { 135, 206, 250, 255 };
    if (get_is_slowing_down()) invert_color(&sky_color);
    Color sand_color = { 242, 195, 68, 255 };
    if (get_is_slowing_down()) invert_color(&sand_color);
    DrawRectangle(0, 0, SCREEN_WIDTH, FLOOR_HEIGHT, sky_color); // Draw sky
    draw_ocean();
    DrawRectangle(0, FLOOR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - FLOOR_HEIGHT, sand_color); // Draw sand
    draw_sand_particles();
}