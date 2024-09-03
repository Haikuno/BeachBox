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

bool is_slowing_down;  // Is the player using the slowdown power
bool is_teleporting;   // Is the player using the teleport power

bool is_game_over;  // If the player lost

bool has_player_released_A = false;  // This bool is used to check if it's the first time the A button is pressed
                                     // This is used because to get to the game scene you need to press A
                                     // And that would also make the player jump

void init_player() {
    player.size = (Vector2){.x = 32, .y = 32};
    player.pos = (Vector2){.x = 100, .y = FLOOR_HEIGHT - player.size.y};
    player.velocity = (Vector2){.x = 0, .y = 0};
    player.speed = 6 + 0.5 * save.player_powerups.player_speed_level;
    player.max_meter = 100 + 10 * save.player_powerups.meter_level;
    player.meter = player.max_meter;
    player.is_shifted = false;

    is_slowing_down = false;
    is_teleporting = false;
}

void move_player(Vector2 direction) {
    player.velocity.x += direction.x * player.speed;
}

void toggle_player_shift() {
    player.is_shifted = !player.is_shifted;
}

void update_player_pos() {
    player.pos.x += player.velocity.x;
    if (player.pos.x < 0) player.pos.x = 0;
    if (player.pos.x > SCREEN_WIDTH - player.size.x) player.pos.x = SCREEN_WIDTH - player.size.x;
    player.velocity.x = 0;

    player.pos.y += player.velocity.y;

    if (player.velocity.y < 0) {
        player.velocity.y += GRAVITY;
    } else {  // we increase the gravity when falling to create a jump that feels better
        player.velocity.y += GRAVITY * 1.5;
    }

    if (player.pos.y > FLOOR_HEIGHT - player.size.y) {
        player.pos.y = FLOOR_HEIGHT - player.size.y;
        player.velocity.y = 0;
    }
}

void update_player() {
    update_player_pos();
    player.meter -= 0.2;
    if (player.meter <= 0) {
        #ifndef DEBUG_GODMODE
        is_game_over = true;
        #endif
    }
}

void jump() {
    if (!has_player_released_A) return;

    bool is_player_on_ground = player.pos.y == FLOOR_HEIGHT - player.size.y;

    int jump_force = 15;

    if (is_player_on_ground) {
        player.velocity.y = -jump_force;
    }
}

// this functions gets called when the player lets go of the jump button, cutting the jump short
void cut_jump() {
    if (player.velocity.y < 0) {
        player.velocity.y *= 0.7;
    }
}

void draw_player() {
    Color color = RED;

    if (player.is_shifted) {
        color.a = 140;
    }

    DrawRectangleV(player.pos, player.size, color);
}