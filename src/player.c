#include "player.h"
#include "timer.h"
#include "controller.h"
#include "config.h"
#include "hats.h"
#include "helper_functions.h"
#include "save.h"
#include "audio.h"
#include "scenes/game.h"

// TODO: this is shit
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

character_t         player                  = { 0 }; // TODO: make static
static bbox_timer_t teleport_duration_timer = { 0 };
bbox_timer_t        teleport_cooldown_timer = { 0 }; // TODO: make static
bool                is_slowing_down         = false; // TODO: make static
bool                is_teleporting          = false; // TODO: make static
bool                held_a_during_death     = false; // TODO: make static

void init_player(void) {
    player.size       = (Vector2){ 32.0f, 32.0f };
    player.pos        = (Vector2){ .x = 100, .y = FLOOR_HEIGHT - player.size.y };
    player.velocity   = (Vector2){ 0.0f, 0.0f };
    player.speed      = 6.5 + 0.5 * get_upgrade_level(UPGRADE_SPEED);
    player.max_meter  = 100 + 10 * get_upgrade_level(UPGRADE_METER);
    player.meter      = player.max_meter;
    player.is_shifted = false;
    player.color      = player_colors[get_player_current_color()];

    is_slowing_down = false;
    is_teleporting  = false;
}

void move_player(Vector2 direction) {
    if (get_is_teleporting()) return; // If the player is teleporting, we don't let the player move
    player.velocity.x += direction.x * player.speed;
}

void shift_player(bool is_holding_down_x) {
    player.is_shifted = is_holding_down_x;
}

void update_player_pos(void) {
    player.pos.x += get_is_teleporting() ? 8.5 + 0.5 * get_upgrade_level(UPGRADE_TELEPORT_DISTANCE) : player.velocity.x;
    if (player.pos.x < 0) player.pos.x = 0;
    if (player.pos.x > SCREEN_WIDTH - player.size.x) player.pos.x = SCREEN_WIDTH - player.size.x;
    player.velocity.x = 0;

    if (get_is_teleporting()) return; // If the player is teleporting, we skip falling down
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
    if (!get_upgrade_level(UPGRADE_SLOWDOWN_UNLOCK)) return;

    if (!get_is_slowing_down()) play_sfx_slowdown();
    else play_sfx_slowdown_back();

    set_is_slowing_down(!get_is_slowing_down());
}

// The teleport power
void teleport(void) {
    if (!get_upgrade_level(UPGRADE_TELEPORT_UNLOCK)) return;

    if (teleport_cooldown_timer.is_running) return;
    if (!teleport_duration_timer.is_running) { // If the player can teleport
        play_sfx_teleport();
        set_is_teleporting(true);
        start_timer(&teleport_duration_timer, 0.4);
        start_timer(&teleport_cooldown_timer, 5 - 0.5 * get_upgrade_level(UPGRADE_TELEPORT_COOLDOWN));
    }
}

void update_player(void) {
    if (get_is_teleporting()) {
        player.velocity.y = BBOX_MIN(player.velocity.y, 0); // If the player was falling down before teleporting, we set it back to 0
        update_timer(&teleport_duration_timer);
        if (!teleport_duration_timer.is_running) set_is_teleporting(false);
    }
    update_timer(&teleport_cooldown_timer);

    update_player_pos();
    player.meter -= 0.16;
    if (get_is_slowing_down()) player.meter -= 0.22 / (1 + get_upgrade_level(UPGRADE_SLOWDOWN_DRAIN) / 4);

    if (player.meter <= 0) {
        lose_game();
    }
}

void draw_player(void) {
    const Color transparent        = { 0, 0, 0, 0 };
    const Color hat_teleport_color = { 0, 0, 0, 120 };

    Color current_player_color = get_is_teleporting() ? transparent : player.color;
    current_player_color.a     = player.is_shifted ? 140 : current_player_color.a;

    Color hat_color = get_is_teleporting() ? hat_teleport_color : WHITE;
    hat_color.a     = player.is_shifted ? 140 : hat_color.a;

    DrawRectangleV(player.pos, player.size, current_player_color);
    DrawRectangleLinesExV(player.pos, player.size, 2, BLACK);

    const hat_t current_hat_index = get_current_hat_type();

    if (current_hat_index != HAT_NIL) {
        float x_pos = player.pos.x + player.size.x / 4;

        // F and Murph need +2 X alignment
        if (current_hat_index == HAT_F || current_hat_index == HAT_MUPRH) {
            x_pos += 2;
        } else if (current_hat_index == HAT_CROWN) {
            x_pos -= 2; // Crown needs -2
        }

        float y_pos = player.pos.y - 16;

        // M and L need +6 Y alignment, Z needs +3, Crown needs +2
        if (current_hat_index == HAT_M || current_hat_index == HAT_L) {
            y_pos += 6;
        } else if (current_hat_index == HAT_Z) {
            y_pos += 3;
        } else if (current_hat_index == HAT_CROWN) {
            y_pos += 2;
        }

        const Texture2D *current_hat_texture = get_hat_texture(current_hat_index);
        DrawTextureV(*current_hat_texture, (Vector2){ x_pos, y_pos }, hat_color);
    }
}