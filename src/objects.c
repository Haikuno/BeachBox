#include <raylib.h>
#include <stdint.h>
#include "timer.h"
#include "config.h"
#include "objects.h"
#include "audio.h"
#include "player.h"
#include "helper_functions.h"
#include "scenes/game.h"

constexpr uint8_t max_objects      = 16; // 16 is the limit since we are using a 16-bit bitfield
constexpr float   max_object_speed = 13;
constexpr float   coin_size        = 10.0f;

struct Objects {
        Vector2 size[max_objects / 2]; // half because all coins are the same size
        Vector2 pos[max_objects];
} static objects = { 0 };

// TODO: none of this should be externed
extern character_t player;
extern bool        is_slowing_down;
extern bool        is_teleporting;
extern uint16_t    current_coins;

// Bitfield that keeps track of which objects are active, 0 being inactive and 1 active
static uint16_t objects_bitfield;

// We split the bitfield in half, the first 8 bits being the pillars and the last 8 bits being the coins
constexpr uint8_t pillars_fist_bit = 0;
constexpr uint8_t pillars_last_bit = 7;
constexpr uint8_t coins_first_bit  = 8;
constexpr uint8_t coins_last_bit   = 15;

// So for example, a value of 0b00010000'00000001 would mean there's a pillar occupying the first bit,
// and a coin on the 13th bit

// Bitfield that keeps track of which objects are shifted, 0 being not shifted and 1 shifted
static uint16_t objects_shifted_bitfield;

static float base_object_speed;
float        current_object_speed; // TODO: getter for this

// Cooldown variables
static float        coin_cooldown;
static float        pillar_cooldown;
static float        giant_pillar_cooldown;
static bbox_timer_t coin_spawn_timer;
static bbox_timer_t pillar_spawn_timer;
static bbox_timer_t giant_pillar_spawn_timer;

static void calculate_object_cooldowns(void) {
    coin_cooldown         = 8 / current_object_speed;
    pillar_cooldown       = 15 / current_object_speed;
    giant_pillar_cooldown = 70 / current_object_speed;
}

void init_objects(void) {
    objects_bitfield     = 0;
    current_object_speed = base_object_speed = 5;
    start_timer(&coin_spawn_timer, 1);
    start_timer(&pillar_spawn_timer, 2);
    start_timer(&giant_pillar_spawn_timer, 30); // The first giant pillar spawns later in the run
}

static void spawn_pillar(void) {
    if (pillar_spawn_timer.is_running) return;

    uint16_t index;
    for (index = pillars_fist_bit; index <= pillars_last_bit; index++) {
        if (!(objects_bitfield & (1 << index))) {
            break;
        }
    }

    objects_bitfield |= (1 << index);

    // Spawn a giant pillar
    if (!giant_pillar_spawn_timer.is_running) {
        objects.size[index]       = (Vector2){ .x = 200, .y = FLOOR_HEIGHT };
        objects.pos[index]        = (Vector2){ .x = SCREEN_WIDTH + 100, .y = FLOOR_HEIGHT - objects.size[index].y };
        objects_shifted_bitfield &= ~(1 << index);

        // We start both timers to avoid spawning two pillars on top of each other
        start_timer(&giant_pillar_spawn_timer, giant_pillar_cooldown);
        start_timer(&pillar_spawn_timer, pillar_cooldown);
        return;
    }

    // If couldn't spawn a giant pillar, spawn a normal pillar
    objects.size[index] = (Vector2){ .x = GetRandomValue(30, 50), .y = GetRandomValue(80, 120) };
    objects.pos[index]  = (Vector2){ .x = SCREEN_WIDTH + 100, .y = FLOOR_HEIGHT - objects.size[index].y };

    bool is_upside_down = GetRandomValue(0, 1);
    if (is_upside_down) {
        objects.size[index].y = GetRandomValue(270, 300);
        objects.pos[index].y  = -5;
    }

    objects_shifted_bitfield |= (1 << index) & GetRandomValue(0, 1);
    start_timer(&pillar_spawn_timer, pillar_cooldown);
    return;
}

// This function is also responsible of increasing the object speed each time a coin is spawned
static void spawn_coin(void) {
    if (coin_spawn_timer.is_running) return;
    uint16_t index;
    for (index = coins_first_bit; index <= coins_last_bit; index++) {
        if (!(objects_bitfield & (1 << index))) {
            break;
        }
    }

    objects_bitfield |= (1 << index);

    objects.pos[index]        = (Vector2){ .x = SCREEN_WIDTH + 100, .y = GetRandomValue(190, FLOOR_HEIGHT - 100) };
    objects_shifted_bitfield |= (1 << index) & GetRandomValue(0, 1);

    base_object_speed = BBOX_MIN(base_object_speed + 0.09, max_object_speed);
    calculate_object_cooldowns();
    start_timer(&coin_spawn_timer, coin_cooldown);
}

static void add_objects(void) {
    if (objects_bitfield == max_objects) return;
    spawn_pillar();
    spawn_coin();
}

static inline bool is_giant_pillar(Vector2 size) {
    return size.y >= FLOOR_HEIGHT;
}

void update_objects(void) {
    update_timer(&coin_spawn_timer);
    update_timer(&pillar_spawn_timer);
    update_timer(&giant_pillar_spawn_timer);
    current_object_speed = is_slowing_down ? base_object_speed / 1.5 : base_object_speed;
    add_objects();

    for (int index = 0; index < max_objects; index++) {
        if (!(objects_bitfield & (1 << index))) continue; // Skip if the object is not active
        objects.pos[index].x -= current_object_speed;     // Move objects

        // Remove objects that are off the screen
        if (index >= coins_first_bit && index <= coins_last_bit) {
            if (objects.pos[index].x < -coin_size) {
                objects_bitfield &= ~(1 << index);
                continue;
            }
        } else {
            if (objects.pos[index].x < -objects.size[index].x) {
                objects_bitfield &= ~(1 << index);
                continue;
            }
        }

        // Check for collisions with the player

        // Coins
        if (index >= coins_first_bit && index <= coins_last_bit) {
            if ((objects_shifted_bitfield & (1 << index)) != player.is_shifted && !is_teleporting)
                continue; // If the player and the coin's "dimension" do not match, skip
                          // If the player is teleporting, we grab the coin no matter what

            if (CheckCollisionCircleRec(objects.pos[index], coin_size, (Rectangle){ .x = player.pos.x, .y = player.pos.y, .width = player.size.x, .height = player.size.y })) {
                objects_bitfield &= ~(1 << index);
                play_sfx_coin();
                current_coins++;
                player.meter = BBOX_MIN(player.meter + 20, player.max_meter);
            }
        }

        // Pillars
        if (index >= pillars_fist_bit && index <= pillars_last_bit) {
            if (is_teleporting || (!is_giant_pillar(objects.size[index]) && (objects_shifted_bitfield & (1 << index)) != player.is_shifted))
                continue; // If the player and the pillar's "dimension" do not match, skip
                          // If the player is teleporting, we do not check for collisions

            if ((CheckCollisionRectangleV(player.pos, player.size, objects.pos[index], objects.size[index]))) {
                lose_game();
                break;
            }
        }
    }
}

void draw_objects(void) {
    // Pillars
    for (int index = pillars_fist_bit; index <= pillars_last_bit; index++) {
        if (!(objects_bitfield & (1 << index))) continue;

        Color color = is_giant_pillar(objects.size[index]) ? (Color){ 136, 216, 176, 255 } : (Color){ 255, 154, 49, 255 };

        if (is_slowing_down) invert_color(&color);
        if (objects_shifted_bitfield & (1 << index)) {
            color.a = 50;
        }

        DrawRectangleV(objects.pos[index], objects.size[index], color);
        DrawRectangleLinesExV(objects.pos[index], objects.size[index], 2, BLACK);
    }

    // Coins
    for (int index = coins_first_bit; index <= coins_last_bit; index++) {
        Color color = { 224, 212, 0, 255 };

        if (is_slowing_down) invert_color(&color);
        if (objects_shifted_bitfield & (1 << index)) {
            color.a = 100;
        }

        if (objects_bitfield & (1 << index)) {
            DrawCircleV(objects.pos[index], coin_size, color);
            DrawCircleLinesExV(objects.pos[index], coin_size, 1.25, BLACK);
        }
    }
}