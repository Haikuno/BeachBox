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

struct UiButton options_button = {
    .pos = {.x = 43, .y = 270},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 2,
    .layer = 0,
    .text = "Options",
};

struct UiButton credits_button = {
    .pos = {.x = 43, .y = 320},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 3,
    .layer = 0,
    .text = "Credits",
};

void update_mainmenu_scene() {
    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    row_count[0] = 4;
}

// TODO: make this look pretty
void draw_mainmenu_scene() {
    draw_background();
    DrawText("BeachBox", 100, 100, 44, BLACK);

    if (do_button(play_button, GRAY)) {
        change_scene(GAME);
    }
    if (do_button(shop_button, GRAY)) {
        change_scene(SHOP);
    }
    if (do_button(options_button, GRAY)) {
        //change_scene(OPTIONS);
        printf("Options not implemented yet!\n");
    }
    if (do_button(credits_button, GRAY)) {
        //change_scene(CREDITS);
        printf("Credits not implemented yet!\n");
    }

    DrawRectangle(380, 80, 200, 100, (Color){200, 200, 200, 150});
    DrawText(TextFormat("Coins: %d", save.total_coins), 400, 100, 20, BLACK);
    DrawText(TextFormat("Runs: %d", save.total_runs), 400, 120, 20, BLACK);
    DrawText(TextFormat("Highscore: %d", save.high_score), 400, 140, 20, BLACK);
    DrawRectangleLinesV((Vector2){.x = 380, .y = 80}, (Vector2){.x = 200, .y = 100}, BLACK);
}