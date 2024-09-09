const Vector2 buttons_size_unlockables = {.x = 200, .y = 50};
const float buttons_padding_unlockables = 15;

struct UiButton unlockables_confirm_button = {
    .pos = {.x = SCREEN_WIDTH / 2 - buttons_size_unlockables.x / 2, .y = SCREEN_HEIGHT - buttons_size_unlockables.y / 2 - buttons_padding_unlockables * 2},
    .size = buttons_size_unlockables,
    .column = 0,
    .row = 2,
    .layer = 0,
    .text = "Confirm",
};

void update_unlockables_scene() {
    for (int i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }
    row_count[0] = 3;
}

inline bool is_hat_unlocked(uint8_t index) {
    return save.hats_unlocked[index];
}

uint8_t hat_price[HAT_MAX] = {
    0,    // No hat
    50,   // Red Slime
    50,   // Blue Slime
    80,   // Box
    100,  // M hat
    100,  // L hat
    100,  // Z hat
    80,   // F hat
    150,  // Murph hat
    0,    // Crown (not purchaseable)
};

void draw_hat_price(uint8_t index) {
    Vector2 size = {350, 130};
    Vector2 pos = {.x = SCREEN_WIDTH / 2 - size.x / 2, .y = FLOOR_HEIGHT - size.y * 2};
    DrawRectangleV(pos, size, (Color){22, 22, 22, 225});

    DrawText("Locked!", pos.x + size.x / 2 - MeasureText("Locked!", 20) / 2, pos.y + size.y / 2 - 50, 20, RED);

    if (index != 9) {  // if not crown
        const char *price_text = TextFormat("Price: %d", hat_price[index]);
        DrawText(price_text, pos.x + size.x / 2 - MeasureText(price_text, 20) / 2, pos.y + size.y / 2 - 20, 20, WHITE);
        if (selected_row == 2) return;  // if on confirm button
        DrawText("Press A to buy", pos.x + size.x / 2 - MeasureText("Press A to buy", 20) / 2, pos.y + size.y / 2 + 30, 20, WHITE);
        return;
    }

    // Crown

    const char *crown_explanation_text = TextFormat("Reach 100 highscore to unlock!");
    DrawText(crown_explanation_text, pos.x + size.x / 2 - MeasureText(crown_explanation_text, 20) / 2, pos.y + size.y / 2, 20, WHITE);
}

void draw_unlockables_scene() {
    draw_background();

    // Draw total coins
    DrawRectangle(245, 0, 150, 40, ui_button_color);
    DrawRectangleLines(245, 0, 150, 40, ui_button_selected_color);
    const char *coins_text = TextFormat("Coins: %d", save.total_coins);
    DrawText(coins_text, 245 + (150 - MeasureText(coins_text, 20)) / 2, 10, 20, ui_button_selected_color);

    // Draw player
    Vector2 player_size_unlockables = (Vector2){.x = player.size.x * 2, .y = player.size.y * 2};
    Vector2 player_pos_unlockables = (Vector2){.x = SCREEN_WIDTH / 2 - player_size_unlockables.x / 2, .y = FLOOR_HEIGHT - player_size_unlockables.y};

    DrawRectangleV(player_pos_unlockables, player_size_unlockables, player.color);
    DrawRectangleLinesExV(player_pos_unlockables, player_size_unlockables, 2, BLACK);

    // Draw player hat
    if (save.hat_index != HAT_NIL) {
        float x_pos = player_pos_unlockables.x + player_size_unlockables.x / 4;

        // F and Murph need +3 X alignment
        if (save.hat_index == HAT_F || save.hat_index == HAT_MUPRH) {
            x_pos += 3;
        }

        float y_pos = player_pos_unlockables.y - 32;

        // M and L need +8 Y alignment, Z needs +4, Crown needs +1
        if (save.hat_index == HAT_M || save.hat_index == HAT_L) {
            y_pos += 8;
        } else if (save.hat_index == HAT_Z) {
            y_pos += 4;
        } else if (save.hat_index == HAT_CROWN) {
            y_pos += 1;
        }

        DrawTextureEx(hats[save.hat_index], (Vector2){x_pos, y_pos}, 0, 2, WHITE);
    }

    // Hat selection arrows
    Vector2 arrows_size = (Vector2){.x = 24, .y = 24};

    struct UiArrows hat_arrows = {
        .column = 0,
        .row = 0,
        .pos_left = {.x = player_pos_unlockables.x - arrows_size.x * 2, .y = player_pos_unlockables.y - arrows_size.y * 0.8},
        .pos_right = {.x = player_pos_unlockables.x + player_size_unlockables.x + arrows_size.x, .y = player_pos_unlockables.y - arrows_size.y * 0.8},
        .size = arrows_size,
    };

    if (!is_hat_unlocked(save.hat_index)) {
        draw_hat_price(save.hat_index);
        const bool can_buy_hat = save.total_coins >= hat_price[save.hat_index] && selected_row != 2 && save.hat_index != HAT_CROWN;
        if (IsGamepadButtonReleased(0, A) && can_buy_hat) {
            save.total_coins -= hat_price[save.hat_index];
            save.hats_unlocked[save.hat_index] = true;
        }
    }

    int hat_arrows_return_code = do_arrows(hat_arrows);
    if (is_arrow_selected(hat_arrows) && hat_arrows_return_code != 0) {
        save.hat_index = (save.hat_index + hat_arrows_return_code) % HAT_MAX;
        if (save.hat_index == 255) save.hat_index = HAT_MAX-1;  // Wrap around
    }

    // Color selection arrows
    struct UiArrows color_arrows = {
        .column = 0,
        .row = 1,
        .pos_left = {.x = player_pos_unlockables.x - arrows_size.x * 2, .y = player_pos_unlockables.y + arrows_size.y * 0.8},
        .pos_right = {.x = player_pos_unlockables.x + player_size_unlockables.x + arrows_size.x, .y = player_pos_unlockables.y + arrows_size.y * 0.8},
        .size = arrows_size,
    };

    int color_arrows_return_code = do_arrows(color_arrows);
    if (color_arrows_return_code != 0 && is_arrow_selected(color_arrows)) {
        save.color_index = (save.color_index + color_arrows_return_code) % PLAYER_COLOR_COUNT;
        if (save.color_index == 255) save.color_index = PLAYER_COLOR_COUNT - 1;  // Wrap around
        player.color = player_colors[save.color_index];
    }

    // Draw buttons
    if (do_button(unlockables_confirm_button)) {
        if (is_hat_unlocked(save.hat_index)) {
            save_game();
            change_scene(MAINMENU);
        }
    }
}