#define FLOOR_HEIGHT 350
#define PLAYER_COLOR_COUNT 17

const Color player_colors[PLAYER_COLOR_COUNT] = {
    RED,
    ORANGE,
    GOLD,
    YELLOW,
    DARKGREEN,
    LIME,
    GREEN,
    BLUE,
    (Color){137, 207, 240, 255},  // Light blue
    (Color){75, 54, 157, 255},    // Indigo
    (Color){112, 54, 157, 255},   // Violet
    MAGENTA,
    PINK,
    WHITE,
    GRAY,
    DARKBROWN,
    DARKGRAY,
};

Texture2D hats[HAT_MAX];

struct Character {
    Vector2 size;
    Vector2 pos;
    Vector2 velocity;
    float speed;
    float max_meter;
    float meter;
    bool is_shifted;
    Color color;
} player;

struct Timer teleport_duration_timer;
struct Timer teleport_cooldown_timer;

bool is_slowing_down;  // Is the player using the slowdown power
bool is_teleporting;   // Is the player using the teleport power

bool is_game_over;  // If the player lost

uint16_t current_coins;            // The amount of coins collected in this run
bool new_high_score = false;       // If the player got a new high score this run
bool held_a_during_death = false;  // If the player was holding A during death, we ignore the first release of A to prevent pressing "play again" accidentally

void init_player(void) {
    player.size = (Vector2){.x = 32, .y = 32},
    player.pos = (Vector2){.x = 100, .y = FLOOR_HEIGHT - player.size.y};
    player.velocity = (Vector2){.x = 0, .y = 0};
    player.speed = 6.5 + 0.5 * save.player_upgrade_levels.player_speed_level;
    player.max_meter = 100 + 10 * save.player_upgrade_levels.meter_level;
    player.meter = player.max_meter;
    player.is_shifted = false;
    player.color = player_colors[save.color_index];

    is_slowing_down = false;
    is_teleporting = false;

    teleport_duration_timer.is_done = true;
    teleport_cooldown_timer.is_done = true;
}

void lose_game(void) {
#ifndef DEBUG_GODMODE
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) held_a_during_death = true;
    is_game_over = true;
    save.total_runs++;
    save.total_coins += current_coins;
    new_high_score = save.high_score < current_coins;
    if (current_coins >= 100) {
        save.hats_unlocked[HAT_CROWN] = true;  // Unlock crown
    }
#endif
}

inline void move_player(Vector2 direction) {
    if (is_teleporting) return;  // If the player is teleporting, we don't let the player move
    player.velocity.x += direction.x * player.speed;
}

inline void shift_player(bool is_holding_down_x) {
    player.is_shifted = is_holding_down_x;
}

void update_player_pos(void) {
    player.pos.x += is_teleporting ? 8.5 + 0.5 * save.player_upgrade_levels.teleport_distance_level : player.velocity.x;
    if (player.pos.x < 0) player.pos.x = 0;
    if (player.pos.x > SCREEN_WIDTH - player.size.x) player.pos.x = SCREEN_WIDTH - player.size.x;
    player.velocity.x = 0;

    if (is_teleporting) return;  // If the player is teleporting, we skip falling down
    player.velocity.y += player.velocity.y < 0 ? GRAVITY : GRAVITY * 1.5;
    player.pos.y += player.velocity.y;

    player.pos.y = MIN(player.pos.y, FLOOR_HEIGHT - player.size.y);

    if (player.pos.y > FLOOR_HEIGHT - player.size.y) {
        player.pos.y = FLOOR_HEIGHT - player.size.y;
        player.velocity.y = 0;
    }
}

void update_player(void) {
    if (is_teleporting) {
        player.velocity.y = MIN(player.velocity.y, 0);  // If the player was falling down before teleporting, we set it back to 0
        update_timer(&teleport_duration_timer);
        if (teleport_duration_timer.is_done) is_teleporting = false;
    }
    update_timer(&teleport_cooldown_timer);

    update_player_pos();
    player.meter -= 0.16;
    if (is_slowing_down) player.meter -= 0.22 / (1 + save.player_upgrade_levels.slowdown_cost_level / 4);

    if (player.meter <= 0) {
        lose_game();
    }
}

void jump(void) {
    bool is_player_on_ground = player.pos.y == FLOOR_HEIGHT - player.size.y;

    int jump_force = 15;

    if (is_player_on_ground) {
        player.velocity.y = -jump_force;
    }
}

// This function gets called when the player lets go of the jump button, cutting the jump short
inline void cut_jump(void) {
    if (player.velocity.y < 0) {
        player.velocity.y *= 0.7;
    }
}

// The slowdown power
inline void slow_down(void) {
    if (!save.player_upgrade_levels.slowdown_unlocked) return;
    is_slowing_down = !is_slowing_down;
}

// The teleport power
inline void teleport(void) {
    if (!save.player_upgrade_levels.teleport_unlocked) return;
    if (!teleport_cooldown_timer.is_done) return;
    if (teleport_duration_timer.is_done) {  // If the player can teleport
        is_teleporting = true;
        start_timer(&teleport_duration_timer, 0.4);
        start_timer(&teleport_cooldown_timer, 5 - 0.5 * save.player_upgrade_levels.teleport_cooldown_level);
    }
}

void draw_player(void) {
    const Color transparent = {0, 0, 0, 0};
    const Color hat_teleport_color = {0, 0, 0, 120};

    Color current_player_color = is_teleporting ? transparent : player.color;
    current_player_color.a = player.is_shifted ? 140 : current_player_color.a;

    Color hat_color = is_teleporting ? hat_teleport_color : WHITE;
    hat_color.a = player.is_shifted ? 140 : hat_color.a;

    DrawRectangleV(player.pos, player.size, current_player_color);
    DrawRectangleLinesExV(player.pos, player.size, 2, BLACK);

    // draw hat
    if (save.hat_index != HAT_NIL) {
        uint8_t x_pos = player.pos.x + player.size.x / 4;

        // F and Murph need +2 X alignment
        if (save.hat_index == HAT_F || save.hat_index == HAT_MUPRH) {
            x_pos += 2;
        } else if (save.hat_index == HAT_CROWN) {
            x_pos -= 2;  // Crown needs -2
        }

        uint8_t y_pos = player.pos.y - 16;

        // M and L need +6 Y alignment, Z needs +3, Crown needs +2
        if (save.hat_index == HAT_M || save.hat_index == HAT_L) {
            y_pos += 6;
        } else if (save.hat_index == HAT_Z) {
            y_pos += 3;
        } else if (save.hat_index == HAT_CROWN) {
            y_pos += 2;
        }

        DrawTextureV(hats[save.hat_index], (Vector2){player.pos.x + player.size.x / 4, player.pos.y - 16}, hat_color);
    }
}