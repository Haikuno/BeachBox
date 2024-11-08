#include "player.h"
#include "timer.h"
#include "controller.h"
#include "config.h"
#include "hats.h"
#include "helper_functions.h"
#include "save.h"
#include "audio.h"
#include "scenes/game.h"

extern save_t    save;
extern Texture2D hats[];

const Color player_colors[PLAYER_COLORS_COUNT] = {
    RED,
    ORANGE,
    GOLD,
    YELLOW,
    DARKGREEN,
    LIME,
    GREEN,
    BLUE,
    (Color){ 137, 207, 240, 255 }, // Light blue
    (Color){ 75,  54,  157, 255 }, // Indigo
    (Color){ 112, 54,  157, 255 }, // Violet
    MAGENTA,
    PINK,
    WHITE,
    GRAY,
    DARKBROWN,
    DARKGRAY,
};

character_t  player                  = { 0 };
bbox_timer_t teleport_duration_timer = { 0 };
bbox_timer_t teleport_cooldown_timer = { 0 };
bool         is_slowing_down         = false;
bool         is_teleporting          = false;
bool         held_a_during_death     = false;

void init_player(void) {
    player.size       = (Vector2){ 32.0f, 32.0f };
    player.pos        = (Vector2){ .x = 100, .y = FLOOR_HEIGHT - player.size.y };
    player.velocity   = (Vector2){ 0.0f, 0.0f };
    player.speed      = 6.5 + 0.5 * save.player_upgrade_levels.player_speed_level;
    player.max_meter  = 100 + 10 * save.player_upgrade_levels.meter_level;
    player.meter      = player.max_meter;
    player.is_shifted = false;
    player.color      = player_colors[save.color_index];

    is_slowing_down = false;
    is_teleporting  = false;
}

void move_player(Vector2 direction) {
    if (is_teleporting) return; // If the player is teleporting, we don't let the player move
    player.velocity.x += direction.x * player.speed;
}

void shift_player(bool is_holding_down_x) {
    player.is_shifted = is_holding_down_x;
}

void update_player_pos(void) {
    player.pos.x += is_teleporting ? 8.5 + 0.5 * save.player_upgrade_levels.teleport_distance_level : player.velocity.x;
    if (player.pos.x < 0) player.pos.x = 0;
    if (player.pos.x > SCREEN_WIDTH - player.size.x) player.pos.x = SCREEN_WIDTH - player.size.x;
    player.velocity.x = 0;

    if (is_teleporting) return; // If the player is teleporting, we skip falling down
    player.velocity.y += player.velocity.y < 0 ? GRAVITY : GRAVITY * 1.5;
    player.pos.y      += player.velocity.y;

    player.pos.y = BBOX_MIN(player.pos.y, FLOOR_HEIGHT - player.size.y);

    if (player.pos.y > FLOOR_HEIGHT - player.size.y) {
        player.pos.y      = FLOOR_HEIGHT - player.size.y;
        player.velocity.y = 0;
    }
}

void jump(void) {
    bool is_player_on_ground = player.pos.y == FLOOR_HEIGHT - player.size.y;

    int jump_force = 15;

    if (is_player_on_ground) {
        player.velocity.y = -jump_force;
    }
}

void cut_jump(void) {
    if (player.velocity.y < 0) {
        player.velocity.y *= 0.7;
    }
}

// The slowdown power
void slow_down(void) {
    if (!save.player_upgrade_levels.slowdown_unlocked) return;

    if (!is_slowing_down) play_sfx_slowdown();
    else play_sfx_slowdown_back();

    is_slowing_down = !is_slowing_down;
}

// The teleport power
void teleport(void) {
    if (!save.player_upgrade_levels.teleport_unlocked) return;

    if (teleport_cooldown_timer.is_running) return;
    if (!teleport_duration_timer.is_running) { // If the player can teleport
        play_sfx_teleport();
        is_teleporting = true;
        start_timer(&teleport_duration_timer, 0.4);
        start_timer(&teleport_cooldown_timer, 5 - 0.5 * save.player_upgrade_levels.teleport_cooldown_level);
    }
}

void update_player(void) {
    if (is_teleporting) {
        player.velocity.y = BBOX_MIN(player.velocity.y, 0); // If the player was falling down before teleporting, we set it back to 0
        update_timer(&teleport_duration_timer);
        if (!teleport_duration_timer.is_running) is_teleporting = false;
    }
    update_timer(&teleport_cooldown_timer);

    update_player_pos();
    player.meter -= 0.16;
    if (is_slowing_down) player.meter -= 0.22 / (1 + save.player_upgrade_levels.slowdown_cost_level / 4);

    if (player.meter <= 0) {
        lose_game();
    }
}

void draw_player(void) {
    const Color transparent        = { 0, 0, 0, 0 };
    const Color hat_teleport_color = { 0, 0, 0, 120 };

    Color current_player_color = is_teleporting ? transparent : player.color;
    current_player_color.a     = player.is_shifted ? 140 : current_player_color.a;

    Color hat_color = is_teleporting ? hat_teleport_color : WHITE;
    hat_color.a     = player.is_shifted ? 140 : hat_color.a;

    DrawRectangleV(player.pos, player.size, current_player_color);
    DrawRectangleLinesExV(player.pos, player.size, 2, BLACK);

    if (save.hat_index != HAT_NIL) {
        float x_pos = player.pos.x + player.size.x / 4;

        // F and Murph need +2 X alignment
        if (save.hat_index == HAT_F || save.hat_index == HAT_MUPRH) {
            x_pos += 2;
        } else if (save.hat_index == HAT_CROWN) {
            x_pos -= 2; // Crown needs -2
        }

        float y_pos = player.pos.y - 16;

        // M and L need +6 Y alignment, Z needs +3, Crown needs +2
        if (save.hat_index == HAT_M || save.hat_index == HAT_L) {
            y_pos += 6;
        } else if (save.hat_index == HAT_Z) {
            y_pos += 3;
        } else if (save.hat_index == HAT_CROWN) {
            y_pos += 2;
        }

        DrawTextureV(hats[save.hat_index], (Vector2){ x_pos, y_pos }, hat_color);
    }
}