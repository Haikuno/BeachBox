#define MAX_SAND_PARTICLES 20
#define SAND_SPAWN_RATE 0.15 // in seconds, lower is faster
#define SLOW_WAVE_HEIGHT 12
#define SLOW_WAVE_WIDTH 8
#define FAST_WAVE_HEIGHT 20
#define FAST_WAVE_WIDTH 15

struct SandParticle {
    Vector2 pos;
    bool active;
} sand_particles[MAX_SAND_PARTICLES] = {0};

void draw_ocean(void) {
    Color ocean_color = {66, 147, 255, 255};
    if (is_slowing_down) invert_color(&ocean_color);
    DrawRectangle(0, 250, SCREEN_WIDTH, FLOOR_HEIGHT - 250, ocean_color);

    Color colors[4] = {
        {173, 216, 230, 255},
        {135, 206, 250, 255},
        {135, 206, 235, 255},
        {70, 130, 180, 255},
    };

    if (is_slowing_down) {
        for (int i = 0; i < 4; i++) {
            invert_color(&colors[i]);
        }
    }

    static float slowWaveStep = 0;
    static float fastWaveStep = 0;

    if (!is_game_paused) {
        slowWaveStep += current_object_speed != 0 ? 0.002f * current_object_speed : 0.01f;
        fastWaveStep += current_object_speed != 0 ? 0.004f * current_object_speed : 0.02f;
    }

    if (fastWaveStep > 100) fastWaveStep = 0;
    if (slowWaveStep > 100) slowWaveStep = 0;

    int centerY = SCREEN_HEIGHT / 2;
    for (int row = 0; row < 2; row++) {
        int y = centerY + (row * SLOW_WAVE_HEIGHT);
        for (int x = 0; x < SCREEN_WIDTH; x += SLOW_WAVE_WIDTH) {
            float offsetY = sinf((x + slowWaveStep * 100) / 50.0f) * SLOW_WAVE_HEIGHT / 2;
            DrawRectangle(x, y + (int)offsetY, SLOW_WAVE_WIDTH, SLOW_WAVE_HEIGHT, colors[row]);
        }
    }

    for (int row = 0; row < 2; row++) {
        int y = centerY + (row * FAST_WAVE_HEIGHT);
        for (int x = 0; x < SCREEN_WIDTH; x += FAST_WAVE_WIDTH) {
            float offsetY = sinf((x + fastWaveStep * 100) / 50.0f) * FAST_WAVE_HEIGHT / 2 + 45;
            DrawRectangle(x, y + (int)offsetY, FAST_WAVE_WIDTH, FAST_WAVE_HEIGHT, colors[row + 2]);
        }
    }
}

void draw_sand_particles(void) {
    static struct Timer sand_particle_spawn_timer = {.is_done = true};
    update_timer(&sand_particle_spawn_timer);
    Color sand_particle_color = BROWN;
    if (is_slowing_down) invert_color(&sand_particle_color);

    for (uint16_t i = 0; i < MAX_SAND_PARTICLES; i++) {
        if (sand_particles[i].active) {
            DrawRectangleV(sand_particles[i].pos, (Vector2){7, 7}, sand_particle_color);

            if (is_game_paused) continue;

            sand_particles[i].pos.x -= current_object_speed != 0 ? current_object_speed : 5;

            if (sand_particles[i].pos.x < 0) {
                sand_particles[i].active = false;
            }
        } else {
            if (is_game_paused) continue;
            if (sand_particle_spawn_timer.is_done && GetRandomValue(0, 300) == 0) {
                start_timer(&sand_particle_spawn_timer, 0.15);
                sand_particles[i].pos = (Vector2){SCREEN_WIDTH, GetRandomValue(FLOOR_HEIGHT + 22, SCREEN_HEIGHT - 22)};
                sand_particles[i].active = true;
            }
        }
    }
}

void draw_background(void) {
    Color sky_color = {135, 206, 250, 255};
    if (is_slowing_down) invert_color(&sky_color);
    Color sand_color = {242, 195, 68, 255};
    if (is_slowing_down) invert_color(&sand_color);
    DrawRectangle(0, 0, SCREEN_WIDTH, FLOOR_HEIGHT, sky_color);  // Draw sky
    draw_ocean();
    DrawRectangle(0, FLOOR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - FLOOR_HEIGHT, sand_color);  // Draw sand
    draw_sand_particles();
}