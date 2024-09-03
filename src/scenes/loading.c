struct UiButton load_button = {
    .pos = {.x = 80, .y = SCREEN_HEIGHT * 0.45},
    .size = {.x = 200, .y = 50},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Load",
};

struct UiButton new_game_button = {
    .pos = {.x = 320, .y = SCREEN_HEIGHT * 0.45},
    .size = {.x = 200, .y = 50},
    .column = 1,
    .row = 0,
    .layer = 0,
    .text = "New",
};

void update_loading_scene() {
    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    column_count[0] = 2;
}

void draw_loading_scene() {
    ClearBackground(RAYWHITE);

    // TODO: add visual feedback, add confirmation windows

    if (do_button(load_button)) {
        int code = load_game();
        if (code == -1) {
            printf(" NO VMU FOUND! \n");
        } else if (code == 1) {
            printf(" SUCCESSFULLY LOADED GAME \n");
            change_scene(MAINMENU);
        }
    }

    if (do_button(new_game_button)) {
        int code = new_game();
        if (code == -1) {
            printf(" NO VMU FOUND! \n");
        } else if (code == 0) {
            printf(" NOT ENOUGH SPACE IN VMU! \n");
        } else if (code == 1) {
            printf(" SUCCESSFULLY CREATED NEW GAME \n");
            change_scene(MAINMENU);
        }
    }
}