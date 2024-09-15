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

void init_game_scene(void) {
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

void update_game_scene(void) {
    if (is_game_over) return;
    if (is_game_paused) return;
    update_objects();
    update_player();
}


void draw_game_over(void) {
    is_slowing_down = false;   // To turn off the inverted color effect
    current_object_speed = 5;  // To reset the object speed
    if (new_high_score) save.high_score = current_coins;

    DrawRectangleV((Vector2){SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4}, (Vector2){SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5}, ui_button_color);
    DrawText("You lost", SCREEN_WIDTH / 2 - MeasureText("You lost", 30) / 2, SCREEN_HEIGHT / 4 + 20, 30, RAYWHITE);

    const char *coins_text = TextFormat("Score: %d", current_coins);
    const char *high_score_text = TextFormat("High Score: %d", save.high_score);

    DrawText(coins_text, SCREEN_WIDTH / 2 - MeasureText(coins_text, 20) / 2, SCREEN_HEIGHT / 4 + 75, 20, RAYWHITE);
    DrawText(high_score_text, SCREEN_WIDTH / 2 - MeasureText(high_score_text, 20) / 2, SCREEN_HEIGHT / 4 + 95, 20, RAYWHITE);
    if (new_high_score) {
        DrawText("New High Score!", SCREEN_WIDTH / 2 - MeasureText("New High Score!", 20) / 2, SCREEN_HEIGHT / 4 + 115, 20, RAYWHITE);
    }

    // TODO: if the A button was being held down before dying, releasing it will trigger a button
    // this can cause the player to accidentally press the buttons
    // make an exception for this case

    

    if (held_a_during_death && IsGamepadButtonReleased(0, A)) {
        held_a_during_death = false;
        return;
    }

    if (do_button(play_again, true)) {
        thd_create(1, save_game, 0);
        init_game_scene();
    }

    if (do_button(main_menu_from_game, true)) {
        thd_create(1, save_game, 0);
        change_scene(MAINMENU);
    }
}

void draw_player_ui(void) {
    DrawRectangle(10, 10, 180, 80, ui_background_color);

    // Draw current coint count
    const char *coins_text = TextFormat("Coins: %d", current_coins);
    DrawText(coins_text, 100 - MeasureText(coins_text, 20) / 2, 20, 20, RAYWHITE);

    // Meter
    DrawRectangleV((Vector2){.x = 25, .y = 50}, (Vector2){.x = 150, .y = 15}, ui_button_color);
    DrawRectangleV((Vector2){.x = 25, .y = 50}, (Vector2){.x = Lerp(0, 150, player.meter / player.max_meter), .y = 15}, BLUE);
    DrawRectangleLinesV((Vector2){.x = 25, .y = 50}, (Vector2){.x = 150, .y = 15}, BLACK);

    // Teleport cooldown
    if (!teleport_cooldown_timer.is_done) {
        DrawRectangleV((Vector2){.x = 25, .y = 75}, (Vector2){.x = 150, .y = 8}, RAYWHITE);
        DrawRectangleV((Vector2){.x = 25, .y = 75}, (Vector2){.x = 150 * Lerp(0, 1, (teleport_cooldown_timer.progress / teleport_cooldown_timer.duration)), .y = 8}, DARKGRAY);
        DrawRectangleLinesV((Vector2){.x = 25, .y = 75}, (Vector2){.x = 150, .y = 8}, BLACK);
    }
}

void draw_pause_menu(void) {
    DrawRectangleV((Vector2){SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3}, (Vector2){SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.20}, ui_button_color);
    DrawText("Game Paused!", SCREEN_WIDTH / 2 - MeasureText("Game Paused!", 30) / 2, SCREEN_HEIGHT / 3 + 30, 30, RAYWHITE);
}

void draw_game_scene(void) {
    draw_background();
    if (is_game_over) {
        draw_game_over();
        return;
    }

    // We change the draw order of the objects if the player is shifted / teleporting or not
    if (player.is_shifted || is_teleporting) {
        draw_objects();
        draw_player();
    } else {
        draw_player();
        draw_objects();
    }

    draw_player_ui();

    if (is_game_paused) {
        draw_pause_menu();
    }
}