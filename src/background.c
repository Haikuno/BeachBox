#define MAX_SAND_PARTICLES 20
#define SLOW_WAVE_HEIGHT 12
#define SLOW_WAVE_WIDTH 8
#define FAST_WAVE_HEIGHT 20
#define FAST_WAVE_WIDTH 15
float last_sand_particle_spawn_time;

struct SandParticle {
    Vector2 pos;
    bool active;
} sand_particles[MAX_SAND_PARTICLES] = {0};

void draw_ocean() {
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

    slowWaveStep += current_object_speed != 0 ? 0.002f * current_object_speed : 0.01f;
    fastWaveStep += current_object_speed != 0 ? 0.004f * current_object_speed : 0.02f;

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

void draw_sand_particles() {
    Color sand_particle_color = BROWN;
    if (is_slowing_down) invert_color(&sand_particle_color);

    for (uint16_t i = 0; i < MAX_SAND_PARTICLES; i++) {
        if (sand_particles[i].active) {
            DrawRectangleV(sand_particles[i].pos, (Vector2){7, 7}, sand_particle_color);
            sand_particles[i].pos.x -= current_object_speed != 0 ? current_object_speed : 5;

            if (sand_particles[i].pos.x < 0) {
                sand_particles[i].active = false;
            }
        } else {
            if (GetTime() - last_sand_particle_spawn_time > 0.15 && GetRandomValue(0, 300) == 0) {
                sand_particles[i].pos = (Vector2){SCREEN_WIDTH, GetRandomValue(FLOOR_HEIGHT + 22, SCREEN_HEIGHT - 22)};
                sand_particles[i].active = true;
                last_sand_particle_spawn_time = GetTime();
            }
        }
    }
}

void draw_background() {
    Color sky_color = {135, 206, 250, 255};
    if (is_slowing_down) invert_color(&sky_color);
    Color sand_color = {242, 195, 68, 255};
    if (is_slowing_down) invert_color(&sand_color);
    DrawRectangle(0, 0, SCREEN_WIDTH, FLOOR_HEIGHT, sky_color);  // Draw sky
    draw_ocean();
    DrawRectangle(0, FLOOR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - FLOOR_HEIGHT, sand_color);  // Draw sand
    draw_sand_particles();
}