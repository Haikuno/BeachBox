struct UiButton play_again = {
    .pos = {.x = 180, .y = SCREEN_HEIGHT * 0.55},
    .size = {.x = 130, .y = 50},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Play Again",
};

struct UiButton main_menu_from_game = {
    .pos = {.x = 330, .y = SCREEN_HEIGHT * 0.55},
    .size = {.x = 130, .y = 50},
    .column = 1,
    .row = 0,
    .layer = 0,
    .text = "Main Menu",
};

void init_game_scene() {
    init_player();
    init_objects();
    is_game_over = false;
    has_player_released_A = false;
    current_coins = 0;

    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        row_count[i] = 0;
        column_count[i] = 0;
    }

    column_count[0] = 2;
}

void update_game_scene() {
    if (is_game_over) return;

    if (IsGamepadButtonReleased(0, A)) {
        has_player_released_A = true;
    }

    update_objects();
    update_player();
}

void draw_game_over() {
    ClearBackground((Color){135, 206, 235, 255});

    DrawRectangleV((Vector2){SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4}, (Vector2){SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5}, RAYWHITE);
    DrawText("You lost", (int)(SCREEN_WIDTH / 4) + 90, (int)(SCREEN_HEIGHT / 4) + 20, 30, BLACK);

    if (do_button(play_again)) {
        save.total_runs++;
        save.total_coins += current_coins;
        if (current_coins > save.high_score) {
            save.high_score = current_coins;
        }
        save_game();
        init_game_scene();
    }

    if (do_button(main_menu_from_game)) {
        save.total_runs++;
        save.total_coins += current_coins;
        if (current_coins > save.high_score) {
            save.high_score = current_coins;
        }
        save_game();
        change_scene(MAINMENU);
    }
}

void draw_player_meter() {
    DrawRectangleV((Vector2){.x = 20, .y = 60}, (Vector2){.x = 150, .y = 15}, DARKBLUE);
    DrawRectangleV((Vector2){.x = 20, .y = 60}, (Vector2){.x = 150 * (player.meter / player.max_meter), .y = 15}, BLUE);
    DrawRectangleLinesV((Vector2){.x = 20, .y = 60}, (Vector2){.x = 150, .y = 15}, BLACK);
}

void draw_game_scene() {
    if (!is_game_over) {
        ClearBackground((Color){135, 206, 235, 255});

        // Draw floor TODO: make this pretty, and a separate function. same thing for background
        DrawRectangle(0, FLOOR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - FLOOR_HEIGHT, (Color){242, 195, 68, 255});

        // Draw current coint count
        DrawText(TextFormat("Coins: %d", current_coins), 20, 20, 20, BLACK);

        draw_player_meter();
        draw_objects();
        draw_player();
    } else {
        draw_game_over();
    }
}