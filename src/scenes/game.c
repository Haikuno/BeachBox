struct UiButton play_again = {
    .pos = {.x = 170, .y = SCREEN_HEIGHT * 0.6},
    .size = {.x = 140, .y = 50},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Play Again",
};

struct UiButton main_menu_from_game = {
    .pos = {.x = 330, .y = SCREEN_HEIGHT * 0.6},
    .size = {.x = 140, .y = 50},
    .column = 1,
    .row = 0,
    .layer = 0,
    .text = "Main Menu",
};

void init_game_scene() {
    init_player();
    init_objects();
    is_game_over = false;
    current_coins = 0;
    new_high_score = false;

    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        row_count[i] = 0;
        column_count[i] = 0;
    }

    column_count[0] = 2;
}

void update_game_scene() {
    if (is_game_over) return;
    update_objects();
    update_player();
}

void draw_game_over() {
    is_slowing_down = false;   // To turn off the inverted color effect
    current_object_speed = 5;  // To reset the object speed
    if (new_high_score) save.high_score = current_coins;

    DrawRectangleV((Vector2){SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4}, (Vector2){SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5}, ui_button_color);
    DrawText("You lost", (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 20, 30, RAYWHITE);

    const char *coins_text = TextFormat("Score: %d", current_coins);
    const char *high_score_text = TextFormat("High Score: %d", save.high_score);

    DrawText(coins_text, (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 75, 20, RAYWHITE);
    DrawText(high_score_text, (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 95, 20, RAYWHITE);
    if (new_high_score) {
        DrawText("New High Score!", (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 115, 20, RAYWHITE);
    }

    if (do_button(play_again, true)) {
        save_game();
        init_game_scene();
    }

    if (do_button(main_menu_from_game, true)) {
        save_game();
        change_scene(MAINMENU);
    }
}

void draw_player_ui() {
    DrawRectangle(10, 10, 180, 80, ui_button_color);

    // Draw current coint count
    DrawText(TextFormat("Coins: %d", current_coins), 55, 20, 20, RAYWHITE);

    DrawRectangleV((Vector2){.x = 20, .y = 50}, (Vector2){.x = 150, .y = 15}, DARKBLUE);
    DrawRectangleV((Vector2){.x = 20, .y = 50}, (Vector2){.x = Lerp(0, 150, player.meter / player.max_meter), .y = 15}, BLUE);
    DrawRectangleLinesV((Vector2){.x = 20, .y = 50}, (Vector2){.x = 150, .y = 15}, BLACK);

    if (!teleport_cooldown_timer.is_done) {
        DrawRectangleV((Vector2){.x = 20, .y = 75}, (Vector2){.x = 150, .y = 8}, WHITE);
        DrawRectangleV((Vector2){.x = 20, .y = 75}, (Vector2){.x = 150 * Lerp(0, 1, (GetTime() - teleport_cooldown_timer.time_started) / teleport_cooldown_timer.duration), .y = 8}, DARKGRAY);
        DrawRectangleLinesV((Vector2){.x = 20, .y = 75}, (Vector2){.x = 150, .y = 8}, BLACK);
    }
}

void draw_game_scene() {
    draw_background();
    if (is_game_over) {
        draw_game_over();
        return;
    }

    // We change the draw order of the objects based on if the player is shifted / teleporting or not
    if (player.is_shifted || is_teleporting) {
        draw_objects();
        draw_player();
    } else {
        draw_player();
        draw_objects();
    }

    draw_player_ui();
}