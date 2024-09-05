#define FLOOR_HEIGHT 350

struct Character {
    Vector2 size;
    Vector2 pos;
    Vector2 velocity;
    float speed;
    float max_meter;
    float meter;
    bool is_shifted;
} player;

struct Timer teleport_duration_timer;
struct Timer teleport_cooldown_timer;

bool is_slowing_down;  // Is the player using the slowdown power
bool is_teleporting;   // Is the player using the teleport power

bool is_game_over;  // If the player lost

uint16_t current_coins;       // The amount of coins collected in this run
bool new_high_score = false;  // If the player got a new high score this run

void init_player() {
    player.size = (Vector2){.x = 32, .y = 32};
    player.pos = (Vector2){.x = 100, .y = FLOOR_HEIGHT - player.size.y};
    player.velocity = (Vector2){.x = 0, .y = 0};
    player.speed = 6.5 + 0.5 * save.player_upgrade_levels.player_speed_level;
    player.max_meter = 100 + 10 * save.player_upgrade_levels.meter_level;
    player.meter = player.max_meter;
    player.is_shifted = false;

    is_slowing_down = false;
    is_teleporting = false;

    teleport_duration_timer.is_done = true;
    teleport_duration_timer.time_started = 0;
    teleport_cooldown_timer.is_done = true;
    teleport_cooldown_timer.time_started = 0;
}

inline void lose_game() {
#ifndef DEBUG_GODMODE
    is_game_over = true;
    save.total_runs++;
    save.total_coins += current_coins;
    new_high_score = save.high_score < current_coins;
#endif
}

inline void move_player(Vector2 direction) {
    if (is_teleporting) return;  // If the player is teleporting, we don't let the player move
    player.velocity.x += direction.x * player.speed;
}

inline void shift_player(bool is_holding_down_x) {
    player.is_shifted = is_holding_down_x;
}

void update_player_pos() {
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

void update_player() {
    if (is_teleporting) {
        player.velocity.y = MIN(player.velocity.y, 0);  // If the player was falling down before teleporting, we set it back to 0
        update_timer(&teleport_duration_timer);
        if (teleport_duration_timer.is_done) is_teleporting = false;
    }
    update_timer(&teleport_cooldown_timer);

    update_player_pos();
    player.meter -= 0.13;
    if (is_slowing_down) player.meter -= 0.13 / (1 + save.player_upgrade_levels.slowdown_cost_level) * 0.75;

    if (player.meter <= 0) {
        lose_game();
    }
}

void jump() {
    bool is_player_on_ground = player.pos.y == FLOOR_HEIGHT - player.size.y;

    int jump_force = 15;

    if (is_player_on_ground) {
        player.velocity.y = -jump_force;
    }
}

// This function gets called when the player lets go of the jump button, cutting the jump short
inline void cut_jump() {
    if (player.velocity.y < 0) {
        player.velocity.y *= 0.7;
    }
}

inline void slow_down() {
    if (!save.player_upgrade_levels.slowdown_unlocked) return;
    is_slowing_down = !is_slowing_down;
}

inline void teleport() {
    if (!save.player_upgrade_levels.teleport_unlocked) return;
    if (!teleport_cooldown_timer.is_done) return;
    if (teleport_duration_timer.is_done) {  // If the player can teleport
        is_teleporting = true;
        start_timer(&teleport_duration_timer, 0.4);
        start_timer(&teleport_cooldown_timer, 5 - 0.5 * save.player_upgrade_levels.teleport_cooldown_level);
    }
}

void draw_player() {
    Color color = is_teleporting ? GREEN : RED;
    color.a = player.is_shifted ? 140 : color.a;

    DrawRectangleV(player.pos, player.size, color);
    DrawRectangleLinesExV(player.pos, player.size, 2, BLACK);
}