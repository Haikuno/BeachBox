struct UiButton play_button = {
    .pos = {.x = 43, .y = 170},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Play",
};

struct UiButton shop_button = {
    .pos = {.x = 43, .y = 220},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 1,
    .layer = 0,
    .text = "Shop",
};

struct UiButton unlockables_button = {
    .pos = {.x = 43, .y = 270},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 2,
    .layer = 0,
    .text = "Unlockables",
};

struct UiButton options_button = {
    .pos = {.x = 43, .y = 320},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 3,
    .layer = 0,
    .text = "Options",
};

struct UiButton credits_button = {
    .pos = {.x = 43, .y = 370},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 4,
    .layer = 0,
    .text = "Credits",
};

void update_mainmenu_scene() {
    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    row_count[0] = 5;
    column_count[0] = 1;
}

void draw_mainmenu_scene() {
    draw_background();
    DrawText("BeachBox", 100, 100, 44, BLACK);
    DrawText("BeachBox", 98, 100, 44, RAYWHITE);

    if (do_button(play_button, true)) {
        change_scene(GAME);
    }
    if (do_button(shop_button, true)) {
        change_scene(SHOP);
    }
    if (do_button(unlockables_button, true)) {
        change_scene(UNLOCKABLES);
    }
    if (do_button(options_button, true)) {
        change_scene(OPTIONS);
    }
    if (do_button(credits_button, true)) {
        change_scene(CREDITS);
    }

    DrawRectangle(380, 80, 200, 100, ui_background_color);
    DrawText(TextFormat("Coins: %d", save.total_coins), 400, 100, 20, RAYWHITE);
    DrawText(TextFormat("Runs: %d", save.total_runs), 400, 120, 20, RAYWHITE);
    DrawText(TextFormat("Highscore: %d", save.high_score), 400, 140, 20, RAYWHITE);
    DrawRectangleLinesV((Vector2){.x = 380, .y = 80}, (Vector2){.x = 200, .y = 100}, RAYWHITE);
}