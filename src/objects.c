#define MAX_OBJECTS 16

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

uint16_t current_coins;  // The amount of coins collected in this run

void init_objects() {
    objects_bitfield = 0;

    base_object_speed = 5;
    current_object_speed = base_object_speed;

    coin_cooldown = current_object_speed / 5;
    pillar_cooldown = current_object_speed / 2;
    giant_pillar_cooldown = current_object_speed * 2;

    last_coin_spawn_time = GetTime();
    last_pillar_spawn_time = GetTime();
    last_giant_pillar_spawn_time = GetTime();
}

void add_object() {
    if (objects_bitfield == MAX_OBJECTS) {  // No free space
        return;
    }

    // Coins
    for (uint16_t i = 0; i < COINS_LAST_BIT; i++) {
        if (objects_bitfield & (1 << i)) {  // This slot is not free
            continue;
        }

        if (GetTime() - last_coin_spawn_time < coin_cooldown) {  // If we shouldn't spawn a coin
            continue;
        }

        // The current index is free
        objects_bitfield |= (1 << i);  // Set the i-th bit to 1 so we know this slot is taken

        objects.size[i] = COIN_SIZE;
        objects.pos[i] = (Vector2){.x = SCREEN_WIDTH + 100, .y = GetRandomValue(100, FLOOR_HEIGHT - 100)};
        objects.is_shifted[i] = GetRandomValue(0, 1);

        // We set both times to avoid spawning two coins at the same time
        last_coin_spawn_time = GetTime();
        break;
    }

    // Pillars
    for (uint16_t i = COINS_LAST_BIT; i < PILLARS_LAST_BIT; i++) {
        if (objects_bitfield & (1 << i)) {  // This slot is not free
            continue;
        }

        if (GetTime() - last_giant_pillar_spawn_time > giant_pillar_cooldown && GetTime() - last_pillar_spawn_time > pillar_cooldown) {  // If we should spawn a giant pillar
            objects_bitfield |= (1 << i);

            objects.size[i] = (Vector2){.x = GetRandomValue(100, 200), .y = FLOOR_HEIGHT};
            objects.pos[i] = (Vector2){.x = SCREEN_WIDTH + 100, .y = FLOOR_HEIGHT - objects.size[i].y};
            objects.is_shifted[i] = GetRandomValue(0, 1);

            // We set both times to avoid spawning two pillars at the same time
            last_giant_pillar_spawn_time = GetTime();
            last_pillar_spawn_time = GetTime();
            break;
        }

        if (GetTime() - last_pillar_spawn_time > pillar_cooldown) {  // If we should spawn a normal pillar
            objects_bitfield |= (1 << i);

            objects.size[i] = (Vector2){.x = GetRandomValue(30, 50), .y = GetRandomValue(80, 120)};
            objects.pos[i] = (Vector2){.x = SCREEN_WIDTH + 100, .y = FLOOR_HEIGHT - objects.size[i].y};
            objects.is_shifted[i] = GetRandomValue(0, 1);

            // We set both times to avoid spawning two pillars at the same time
            last_pillar_spawn_time = GetTime();
            break;
        }
    }
}

void update_objects() {
    add_object();

    for (uint16_t i = 0; i < MAX_OBJECTS; i++) {
        if (!objects_bitfield & (1 << i)) continue;
        objects.pos[i].x -= base_object_speed;
    }

    // Check for collisions with the player

    // Coinsu
    for (uint16_t i = 0; i < COINS_LAST_BIT; i++) {
        if (!objects_bitfield & (1 << i)) continue;
        if (objects.is_shifted[i] != player.is_shifted) continue;

        if (CheckCollisionCircleRec(objects.pos[i], objects.size[i].x, (Rectangle){.x = player.pos.x, .y = player.pos.y, .width = player.size.x, .height = player.size.y})) {
            objects_bitfield &= ~(1 << i);
            current_coins++;
            player.meter += 15;

            if (player.meter > player.max_meter) {
                player.meter = player.max_meter;
            }
        }
    }

    // Pillars
    for (uint16_t i = COINS_LAST_BIT; i < PILLARS_LAST_BIT; i++) {
        if (!objects_bitfield & (1 << i)) continue;
        if (objects.is_shifted[i] != player.is_shifted) continue;

        if (CheckCollisionRectangleV(player.pos, player.size, objects.pos[i], objects.size[i])) {
            #ifndef DEBUG_GODMODE
            is_game_over = true;
            #endif
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

void draw_objects() {
    // Coins
    for (uint16_t i = 0; i < COINS_LAST_BIT; i++) {
        Color color = YELLOW;
        if (objects.is_shifted[i]) {
            color.a = 100;
        }

        if (objects_bitfield & (1 << i)) {
            DrawCircleV(objects.pos[i], objects.size[i].x, color);
            DrawCircleLinesV(objects.pos[i], objects.size[i].x, BLACK);
        }
    }

    // Pillars
    for (uint16_t i = COINS_LAST_BIT; i < PILLARS_LAST_BIT; i++) {
        Color color = DARKGRAY;
        if (objects.is_shifted[i]) {
            color.a = 100;
        }

        if (objects_bitfield & (1 << i)) {
            DrawRectangleV(objects.pos[i], objects.size[i], color);
            DrawRectangleLinesV(objects.pos[i], objects.size[i], BLACK);
        }
    }
}