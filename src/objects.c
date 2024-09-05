#define MAX_OBJECTS 16

#define MAX_OBJECT_SPEED 10

#define COIN_SIZE \
    (Vector2) { .x = 10, .y = 10 }

struct Objects {
    Vector2 size[MAX_OBJECTS];
    Vector2 pos[MAX_OBJECTS];
    bool is_shifted[MAX_OBJECTS];
} objects = {0};

// 0 represents a free slot
uint16_t objects_bitfield;
// bits 0-7 represent coints, 8-15 represent pillars
#define COINS_LAST_BIT 7
#define PILLARS_LAST_BIT 15

float base_object_speed;
float current_object_speed;

// Cooldown variables
float coin_cooldown;
float pillar_cooldown;
float giant_pillar_cooldown;

float last_coin_spawn_time;
float last_pillar_spawn_time;
float last_giant_pillar_spawn_time;

void calculate_object_cooldowns() {
    coin_cooldown = 10 / current_object_speed;
    pillar_cooldown = 16 / current_object_speed;
    giant_pillar_cooldown = 70 / current_object_speed;
}

void init_objects() {
    objects_bitfield = 0;
    current_object_speed = base_object_speed = 5;
    calculate_object_cooldowns();
    last_coin_spawn_time = last_pillar_spawn_time = GetTime();
    last_giant_pillar_spawn_time = GetTime() + 20;  // Make the first giant pillar spawn later in the run
}

void spawn_coin() {
    uint16_t i;
    for (i = 0; i < COINS_LAST_BIT; i++) {
        if (!(objects_bitfield & (1 << i))) {
            break;
        }
    }

    double now = GetTime();
    if (i >= COINS_LAST_BIT || now - last_coin_spawn_time < coin_cooldown) return;

    objects_bitfield |= (1 << i);

    objects.size[i] = COIN_SIZE;
    objects.pos[i] = (Vector2){.x = SCREEN_WIDTH + 100, .y = GetRandomValue(190, FLOOR_HEIGHT - 100)};
    objects.is_shifted[i] = GetRandomValue(0, 1);

    last_coin_spawn_time = now;
    base_object_speed = MIN(base_object_speed + 0.1, MAX_OBJECT_SPEED);
    calculate_object_cooldowns();
}

void spawn_pillar() {
    uint16_t i;
    for (i = COINS_LAST_BIT; i < PILLARS_LAST_BIT; i++) {
        if (!(objects_bitfield & (1 << i))) {
            break;
        }
    }

    double now = GetTime();
    bool should_spawn_pillar = now - last_pillar_spawn_time > pillar_cooldown;

    if (i >= PILLARS_LAST_BIT || !should_spawn_pillar) return;

    objects_bitfield |= (1 << i);

    bool should_spawn_giant_pillar = should_spawn_pillar && now - last_giant_pillar_spawn_time > giant_pillar_cooldown;

    if (should_spawn_giant_pillar) {
        objects.size[i] = (Vector2){.x = 200, .y = FLOOR_HEIGHT};
        objects.pos[i] = (Vector2){.x = SCREEN_WIDTH + 100, .y = FLOOR_HEIGHT - objects.size[i].y};

        // We set both times to avoid spawning two pillars on top of each other
        last_giant_pillar_spawn_time = now;
        last_pillar_spawn_time = now;
        return;
    }

    // Spawn a normal pillar
    objects.size[i] = (Vector2){.x = GetRandomValue(30, 50), .y = GetRandomValue(80, 120)};
    objects.pos[i] = (Vector2){.x = SCREEN_WIDTH + 100, .y = FLOOR_HEIGHT - objects.size[i].y};

    bool is_upside_down = GetRandomValue(0, 1);
    if (is_upside_down) {
        objects.size[i].y = GetRandomValue(270, 300);
        objects.pos[i].y = -5;
    }

    objects.is_shifted[i] = GetRandomValue(0, 1);
    last_pillar_spawn_time = now;
    return;
}

void add_object() {
    if (objects_bitfield == MAX_OBJECTS) return;
    spawn_coin();
    spawn_pillar();
}

inline bool is_giant_pillar(Vector2 size) {
    return size.y >= FLOOR_HEIGHT;
}

void update_objects() {
    current_object_speed = is_slowing_down ? base_object_speed / 2 : base_object_speed;
    add_object();

    for (uint16_t i = 0; i < MAX_OBJECTS; i++) {
        if (!(objects_bitfield & (1 << i))) continue;
        objects.pos[i].x -= current_object_speed;
    }

    // Check for collisions with the player

    // Coins
    for (uint16_t i = 0; i < COINS_LAST_BIT; i++) {
        if (!(objects_bitfield & (1 << i))) continue;
        if (objects.is_shifted[i] != player.is_shifted && !is_teleporting) continue;

        if (CheckCollisionCircleRec(objects.pos[i], objects.size[i].x, (Rectangle){.x = player.pos.x, .y = player.pos.y, .width = player.size.x, .height = player.size.y})) {
            objects_bitfield &= ~(1 << i);
            current_coins++;
            player.meter = MIN(player.meter + 20, player.max_meter);
        }
    }

    // Pillars
    for (uint16_t i = COINS_LAST_BIT; i < PILLARS_LAST_BIT; i++) {
        if (!(objects_bitfield & (1 << i))) continue;
        if (is_teleporting || (!is_giant_pillar(objects.size[i]) && objects.is_shifted[i] != player.is_shifted)) continue;

        if (CheckCollisionRectangleV(player.pos, player.size, objects.pos[i], objects.size[i])) {
            lose_game();
            break;
        }
    }

    // Remove objects that are off the screen
    for (uint16_t i = 0; i < MAX_OBJECTS; i++) {
        if (objects.pos[i].x < -objects.size[i].x) {
            objects_bitfield &= ~(1 << i);
        }
    }
}

void invert_color(Color *color) {
    color->r = 255 - color->r;
    color->g = 255 - color->g;
    color->b = 255 - color->b;
}

void draw_objects() {
    // Pillars
    for (uint16_t i = COINS_LAST_BIT; i < PILLARS_LAST_BIT; i++) {
        if (!(objects_bitfield & (1 << i))) continue;

        Color color;

        if (is_giant_pillar(objects.size[i])) {
            color = (Color){136, 216, 176, 255};
        } else {
            color = (Color){255, 154, 49, 255};
            if (is_slowing_down) invert_color(&color);
            if (objects.is_shifted[i]) {
                color.a = 50;
            }
        }

        DrawRectangleV(objects.pos[i], objects.size[i], color);
        DrawRectangleLinesExV(objects.pos[i], objects.size[i], 2, BLACK);
    }

    // Coins
    for (uint16_t i = 0; i < COINS_LAST_BIT; i++) {
        Color color = {224, 212, 0, 255};

        if (is_slowing_down) invert_color(&color);
        if (objects.is_shifted[i]) {
            color.a = 100;
        }

        if (objects_bitfield & (1 << i)) {
            DrawCircleV(objects.pos[i], objects.size[i].x, color);
            DrawCircleLinesExV(objects.pos[i], objects.size[i].x, 1.25, BLACK);
        }
    }
}