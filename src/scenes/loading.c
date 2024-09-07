struct UiButton load_button = {
    .pos = {.x = 10, .y = SCREEN_HEIGHT * 0.45},
    .size = {.x = 300, .y = 50},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Load",
};

struct UiButton new_game_button = {
    .pos = {.x = 320, .y = SCREEN_HEIGHT * 0.45},
    .size = {.x = 300, .y = 50},
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
    row_count[0] = 1;
}

void load_game_callback() {
    int return_code = load_game();

    switch (return_code) {
        case -1:
            printf(" NO VMU FOUND! \n");
            break;
        case 0:
            printf(" NO SAVES FOUND! \n");
            break;
        case 1:
            printf(" SUCCESSFULLY LOADED GAME \n");
            change_scene(MAINMENU);
            break;
    }
}

void new_game_callback() {
    int return_code = new_game();

    switch (return_code) {
        case -1:
            printf(" NO VMU FOUND! \n");
            break;
        case 0:
            printf(" NOT ENOUGH SPACE IN VMU! \n");
            break;
        case 1:
            printf(" SUCCESSFULLY CREATED NEW GAME \n");
            change_scene(MAINMENU);
            break;
    }
}

void draw_loading_scene() {
    static void (*callback)() = NULL;

    if (do_button(load_button, GRAY)) {
        callback = load_game_callback;
        selected_layer = 1;
        selected_column = 0;
        selected_row = 0;
    }

    if (do_button(new_game_button, GRAY)) {
        callback = new_game_callback;
        selected_layer = 1;
        selected_column = 0;
        selected_row = 0;
    }

    draw_confirmation_window(callback);
}