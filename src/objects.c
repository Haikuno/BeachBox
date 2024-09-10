#define MAX_OBJECTS 16

#define MAX_OBJECT_SPEED 13

#define COIN_SIZE \
    (Vector2) { .x = 10, .y = 10 }

struct Objects {
    Vector2 size[MAX_OBJECTS];
    Vector2 pos[MAX_OBJECTS];
    bool is_shifted[MAX_OBJECTS];
} objects = {0};

// Bitfield that keeps track of which objects are active, 0 being inactive and 1 active
uint16_t objects_bitfield;

// We split the bitfield in half, the first 8 bits being the pillars and the last 8 bits being the coins
#define PILLARS_FIRST_BIT 0
#define PILLARS_LAST_BIT 7
#define COINS_FIRST_BIT 8
#define COINS_LAST_BIT 15
// So for example, a value of 0b00010000'00000001 would mean there's a pillar occupying the first bit, and a coin on the 13th bit

float base_object_speed;
float current_object_speed;

// Cooldown variables
float coin_cooldown;
float pillar_cooldown;
float giant_pillar_cooldown;

float last_coin_spawn_time;
float last_pillar_spawn_time;
float last_giant_pillar_spawn_time;

double now;

void calculate_object_cooldowns() {
    coin_cooldown = 8 / current_object_speed;
    pillar_cooldown = 15 / current_object_speed;
    giant_pillar_cooldown = 70 / current_object_speed;
}

void init_objects() {
    objects_bitfield = 0;
    current_object_speed = base_object_speed = 5;
    calculate_object_cooldowns();
    last_coin_spawn_time = last_pillar_spawn_time = GetTime();
    last_giant_pillar_spawn_time = GetTime() + 28;  // Make the first giant pillar spawn later in the run
}

void spawn_pillar() {
    bool should_spawn_pillar = now - last_pillar_spawn_time > pillar_cooldown;
    if (!should_spawn_pillar) return;

    uint16_t index;
    for (index = PILLARS_FIRST_BIT; index <= PILLARS_LAST_BIT; index++) {
        if (!(objects_bitfield & (1 << index))) {
            break;
        }
    }

    objects_bitfield |= (1 << index);

    bool should_spawn_giant_pillar = should_spawn_pillar && now - last_giant_pillar_spawn_time > giant_pillar_cooldown;

    if (should_spawn_giant_pillar) {
        objects.size[index] = (Vector2){.x = 200, .y = FLOOR_HEIGHT};
        objects.pos[index] = (Vector2){.x = SCREEN_WIDTH + 100, .y = FLOOR_HEIGHT - objects.size[index].y};
        objects.is_shifted[index] = false;

        // We set both times to avoid spawning two pillars on top of each other
        last_giant_pillar_spawn_time = now;
        last_pillar_spawn_time = now;
        return;
    }

    // Spawn a normal pillar
    objects.size[index] = (Vector2){.x = GetRandomValue(30, 50), .y = GetRandomValue(80, 120)};
    objects.pos[index] = (Vector2){.x = SCREEN_WIDTH + 100, .y = FLOOR_HEIGHT - objects.size[index].y};

    bool is_upside_down = GetRandomValue(0, 1);
    if (is_upside_down) {
        objects.size[index].y = GetRandomValue(270, 300);
        objects.pos[index].y = -5;
    }

    objects.is_shifted[index] = GetRandomValue(0, 1);
    last_pillar_spawn_time = now;
    return;
}

// This function is also responsible of increasing the object speed each time a coin is spawned
void spawn_coin() {
    bool should_spawn_coin = now - last_coin_spawn_time > coin_cooldown;
    if (!should_spawn_coin) return;

    uint16_t index;
    for (index = COINS_FIRST_BIT; index <= COINS_LAST_BIT; index++) {
        if (!(objects_bitfield & (1 << index))) {
            break;
        }
    }

    objects_bitfield |= (1 << index);

    objects.size[index] = COIN_SIZE;
    objects.pos[index] = (Vector2){.x = SCREEN_WIDTH + 100, .y = GetRandomValue(190, FLOOR_HEIGHT - 100)};
    objects.is_shifted[index] = GetRandomValue(0, 1);

    last_coin_spawn_time = now;
    base_object_speed = MIN(base_object_speed + 0.09, MAX_OBJECT_SPEED);
    calculate_object_cooldowns();
}

void add_objects() {
    if (objects_bitfield == MAX_OBJECTS) return;
    now = GetTime();
    spawn_pillar();
    spawn_coin();
}

inline bool is_giant_pillar(Vector2 size) {
    return size.y >= FLOOR_HEIGHT;
}

void update_objects() {
    current_object_speed = is_slowing_down ? base_object_speed / 1.5 : base_object_speed;
    add_objects();

    for (uint8_t index = 0; index < MAX_OBJECTS; index++) {
        if (!(objects_bitfield & (1 << index))) continue;  // Skip if the object is not active
        objects.pos[index].x -= current_object_speed;      // Move objects

        // Remove objects that are off the screen
        if (objects.pos[index].x < -objects.size[index].x) {
            objects_bitfield &= ~(1 << index);
            continue;
        }

        // Check for collisions with the player

        // Coins
        if (index >= COINS_FIRST_BIT && index <= COINS_LAST_BIT) {
            if (objects.is_shifted[index] != player.is_shifted && !is_teleporting) continue;  // If the player and the coin's "plane" do not match, skip
                                                                                              // If the player is teleporting, we grab the coin no matter what

            if (CheckCollisionCircleRec(objects.pos[index], objects.size[index].x, (Rectangle){.x = player.pos.x, .y = player.pos.y, .width = player.size.x, .height = player.size.y})) {
                objects_bitfield &= ~(1 << index);
                current_coins++;
                player.meter = MIN(player.meter + 20, player.max_meter);
            }
        }

        // Pillars
        if (index >= PILLARS_FIRST_BIT && index <= PILLARS_LAST_BIT) {
            if (is_teleporting || (!is_giant_pillar(objects.size[index]) && objects.is_shifted[index] != player.is_shifted)) continue;  // If the player and the pillar's "plane" do not match, skip
                                                                                                                                        // If the player is teleporting, we do not check for collisions

            if (CheckCollisionRectangleV(player.pos, player.size, objects.pos[index], objects.size[index])) {
                lose_game();
                break;
            }
        }
    }
}

void invert_color(Color* color) {
    color->r = 255 - color->r;
    color->g = 255 - color->g;
    color->b = 255 - color->b;
}

void draw_objects() {
    // Pillars
    for (uint16_t i = PILLARS_FIRST_BIT; i <= PILLARS_LAST_BIT; i++) {
        if (!(objects_bitfield & (1 << i))) continue;

        Color color = is_giant_pillar(objects.size[i]) ? (Color){136, 216, 176, 255} : (Color){255, 154, 49, 255};

        if (is_slowing_down) invert_color(&color);
        if (objects.is_shifted[i]) {
            color.a = 50;
        }

        DrawRectangleV(objects.pos[i], objects.size[i], color);
        DrawRectangleLinesExV(objects.pos[i], objects.size[i], 2, BLACK);
    }

    // Coins
    for (uint16_t i = COINS_FIRST_BIT; i <= COINS_LAST_BIT; i++) {
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