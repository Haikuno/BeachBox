struct UiButton shop_buttons[] = {
    {.pos = {30, 50}, .size = {200, 40}, .column = 0, .row = 0, .layer = 0, .text = "Movement Speed"},
    {.pos = {30, 110}, .size = {200, 40}, .column = 0, .row = 1, .layer = 0, .text = "Max Meter"},
    {.pos = {30, 170}, .size = {200, 40}, .column = 0, .row = 2, .layer = 0, .text = "Unlock Teleport"},
    {.pos = {30, 230}, .size = {200, 40}, .column = 0, .row = 3, .layer = 0, .text = "Teleport Cooldown"},
    {.pos = {30, 290}, .size = {200, 40}, .column = 0, .row = 4, .layer = 0, .text = "Teleport Distance"},
    {.pos = {30, 350}, .size = {200, 40}, .column = 0, .row = 5, .layer = 0, .text = "Unlock Slowdown"},
    {.pos = {30, 410}, .size = {200, 40}, .column = 0, .row = 6, .layer = 0, .text = "Slowdown Cost"},
    {.pos = {340, 400}, .size = {100, 40}, .column = 1, .row = 0, .layer = 0, .text = "Buy"},
    {.pos = {460, 400}, .size = {100, 40}, .column = 2, .row = 0, .layer = 0, .text = "Exit"},
};

enum ShopOptions {
    SPEED,
    MAX_METER,
    TELEPORT_UNLOCK,
    TELEPORT_COOLDOWN,
    TELEPORT_DISTANCE,
    SLOWDOWN_UNLOCK,
    SLOWDOWN_COST,
} selected_shop_option = SPEED;

const uint8_t max_upgrade_levels[7] = {
    5,  // SPEED
    5,  // MAX_METER
    1,  // TELEPORT_UNLOCK
    5,  // TELEPORT_COOLDOWN
    5,  // TELEPORT_DISTANCE
    1,  // SLOWDOWN_UNLOCK
    5   // SLOWDOWN_COST
};

const uint8_t costs[7] = {
    20,  // SPEED
    20,  // MAX_METER
    50,  // TELEPORT_UNLOCK
    30,  // TELEPORT_COOLDOWN
    30,  // TELEPORT_DISTANCE
    50,  // SLOWDOWN_UNLOCK
    30,  // SLOWDOWN_COST
};

inline bool can_afford(enum ShopOptions option) {
    return costs[option] <= save.total_coins;
}

inline uint8_t get_upgrade_level(enum ShopOptions option) {
    switch (option) {
        case SPEED:
            return save.player_upgrade_levels.player_speed_level;
        case MAX_METER:
            return save.player_upgrade_levels.meter_level;
        case TELEPORT_UNLOCK:
            return save.player_upgrade_levels.teleport_unlocked;
        case TELEPORT_COOLDOWN:
            return save.player_upgrade_levels.teleport_cooldown_level;
        case TELEPORT_DISTANCE:
            return save.player_upgrade_levels.teleport_distance_level;
        case SLOWDOWN_UNLOCK:
            return save.player_upgrade_levels.slowdown_unlocked;
        case SLOWDOWN_COST:
            return save.player_upgrade_levels.slowdown_cost_level;
        default:
            return false;
    }
    return false;
}

inline bool can_upgrade(enum ShopOptions option) {
    switch (option) {
        case TELEPORT_COOLDOWN:
        case TELEPORT_DISTANCE:
            return save.player_upgrade_levels.teleport_unlocked && get_upgrade_level(option) < max_upgrade_levels[option];
        case SLOWDOWN_COST:
            return save.player_upgrade_levels.slowdown_unlocked && get_upgrade_level(option) < max_upgrade_levels[option];
        default:
            return get_upgrade_level(option) < max_upgrade_levels[option];
    }
}

void purchase() {
    switch (selected_shop_option) {
        case SPEED:
            save.player_upgrade_levels.player_speed_level++;
            save.total_coins -= costs[SPEED];
            break;
        case MAX_METER:
            save.player_upgrade_levels.meter_level++;
            save.total_coins -= costs[MAX_METER];
            break;
        case TELEPORT_UNLOCK:
            save.player_upgrade_levels.teleport_unlocked = true;
            save.total_coins -= costs[TELEPORT_UNLOCK];
            break;
        case TELEPORT_COOLDOWN:
            save.player_upgrade_levels.teleport_cooldown_level++;
            save.total_coins -= costs[TELEPORT_COOLDOWN];
            break;
        case TELEPORT_DISTANCE:
            save.player_upgrade_levels.teleport_distance_level++;
            save.total_coins -= costs[TELEPORT_DISTANCE];
            break;
        case SLOWDOWN_UNLOCK:
            save.player_upgrade_levels.slowdown_unlocked = true;
            save.total_coins -= costs[SLOWDOWN_UNLOCK];
            break;
        case SLOWDOWN_COST:
            save.player_upgrade_levels.slowdown_cost_level++;
            save.total_coins -= costs[SLOWDOWN_COST];
            break;
        default:
            break;
    }
}

void update_shop_scene() {
    memset(column_count, 0, sizeof(column_count));
    memset(row_count, 0, sizeof(row_count));

    column_count[0] = 3;
    column_count[1] = column_count[2] = column_count[3] = column_count[4] = column_count[5] = column_count[6] = 1;

    row_count[0] = 7;
    row_count[1] = row_count[2] = 1;

    if (selected_column == 0) {
        selected_shop_option = selected_row;
    }
}

void draw_shop_description() {
    static const char *descriptions[] = {
        "Increases your movement speed\n\n\"For those who want to go fast\"",
        "Increases your max meter\n\nHelps you stay alive for longer\n\n\"I need more!\"",
        "Unlocks the teleport power\n\nTeleport forwards by pressing R\n\nHint:\nYou can teleport through\nthe big wall",
        "Dereases the teleport's cooldown\n\nAllows you to teleport more often",
        "Increases the teleport's distance\n\nAllows you to teleport further",
        "Unlocks the slowdown power\n\nSlow time down by pressing L",
        "Decreases the cost of slowdown\n\n\"Take it slow\"",
    };

    const char *description = descriptions[selected_shop_option];
    Color cost_color = can_afford(selected_shop_option) ? BLACK : RED;

    DrawText(description, 270, 50, 20, BLACK);
    DrawText(TextFormat("Cost: %d coins", costs[selected_shop_option]), 270, 250, 20, cost_color);
    DrawText(TextFormat("Level: %d/%d", get_upgrade_level(selected_shop_option), max_upgrade_levels[selected_shop_option]), 270, 300, 20, BLACK);
}

void jump_to_buy_button() {
    selected_row = 0;
    selected_column = 1;
}

// TODO: confirmation window

void draw_shop_scene() {
    draw_background();
    DrawRectangle(SCREEN_WIDTH * 0.4, 0, SCREEN_WIDTH * 0.6, SCREEN_HEIGHT, (Color){200, 200, 200, 200});
    draw_shop_description();

    // Draw total coins
    const char *coins_text = TextFormat("Coins: %d", save.total_coins);

    DrawText(coins_text, 10 + MeasureText(coins_text, 20) / 2, 10, 20, BLACK);

    DrawLine(SCREEN_WIDTH * 0.4, 0, SCREEN_WIDTH * 0.4, SCREEN_HEIGHT, BLACK);

    bool can_buy = can_afford(selected_shop_option) && can_upgrade(selected_shop_option);
    bool button_pressed = false;

    for (int i = 0; i < 7; i++) {
        Color color = can_afford(i) && can_upgrade(i) ? GRAY : DARKGRAY;
        if (do_button(shop_buttons[i], color)) {
            if (can_buy) jump_to_buy_button();
            button_pressed = true;
        }
    }

    if (do_button(shop_buttons[7], can_buy ? GRAY : DARKGRAY) && !button_pressed && can_buy) {  // Buy
        purchase();
    }

    if (do_button(shop_buttons[8], GRAY)) {  // Exit
        save_game();
        change_scene(MAINMENU);
    }
}