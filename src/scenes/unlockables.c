const Vector2 buttons_size_unlockables = {.x = 200, .y = 50};
const float buttons_padding_unlockables = 15;

struct UiButton unlockables_confirm_button = {
    .pos = {.x = SCREEN_WIDTH / 2 - buttons_size_unlockables.x / 2, .y = SCREEN_HEIGHT - buttons_size_unlockables.y / 2 - buttons_padding_unlockables * 2},
    .size = buttons_size_unlockables,
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Confirm",
};

void update_unlockables_scene() {
    for (int i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    column_count[0] = 1;
    row_count[0] = 2;
}

void draw_unlockables_scene() {
    draw_background();

    // Draw total coins
    const char *coins_text = TextFormat("Coins: %d", save.total_coins);
    DrawText(coins_text, 10 + MeasureText(coins_text, 20) / 2, 10, 20, BLACK);

    Vector2 player_size_unlockables = (Vector2){.x = player.size.x * 2, .y = player.size.y * 2};
    Vector2 player_pos_unlockables = (Vector2){.x = SCREEN_WIDTH / 2 - player_size_unlockables.x / 2, .y = FLOOR_HEIGHT - player_size_unlockables.y};

    DrawRectangleV(player_pos_unlockables, player_size_unlockables, player.color);
    DrawRectangleLinesExV(player_pos_unlockables, player_size_unlockables, 2, BLACK);

    Vector2 arrows_size = (Vector2){.x = 24, .y = 24};

    struct UiArrows color_arrows = {
        .column = 0,
        .row = 1,
        .pos_left = {.x = player_pos_unlockables.x - arrows_size.x * 2, .y = player_pos_unlockables.y + arrows_size.y * 0.8},
        .pos_right = {.x = player_pos_unlockables.x + player_size_unlockables.x + arrows_size.x, .y = player_pos_unlockables.y + arrows_size.y * 0.8},
        .size = arrows_size,
    };

    if (do_button(unlockables_confirm_button, GRAY)) {
        save_game();
        change_scene(MAINMENU);
    }

    int color_arrows_return_code = do_arrows(color_arrows, GRAY);

    if (color_arrows_return_code != 0 && is_arrow_selected(color_arrows)) {
        save.color_index = (save.color_index + color_arrows_return_code) % PLAYER_COLOR_COUNT;
        if (save.color_index == 255) save.color_index = PLAYER_COLOR_COUNT - 1;  // Wrap around
        player.color = player_colors[save.color_index];
    }
}