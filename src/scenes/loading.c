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

struct Timer error_timer = {.is_done = true};
char error_text[50];

void update_loading_scene() {
    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    column_count[0] = 2;
    row_count[0] = 1;
    update_timer(&error_timer);
}

void draw_error_popup() {
    if (error_timer.is_done) return;

    DrawRectangle(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 100, 500, 200, (Color){55, 55, 55, 250});
    DrawText(error_text, (int)(SCREEN_WIDTH / 2 - MeasureText(error_text, 32) / 2), (int)(SCREEN_HEIGHT / 2 - 15), 32, RED);
}

void load_game_callback() {
    int return_code = load_game();

    switch (return_code) {
        case -1:
            start_timer(&error_timer, 3.0f);
            strcpy(error_text, "NO VMU FOUND!");
            break;
        case 0:
            start_timer(&error_timer, 3.0f);
            strcpy(error_text, "NO SAVE DATA FOUND!");
            break;
        case 1:
            change_scene(MAINMENU);
            break;
    }
}

void new_game_callback() {
    int return_code = new_game();

    switch (return_code) {
        case -1:
            start_timer(&error_timer, 3.0f);
            strcpy(error_text, "NO VMU FOUND!");
            break;
        case 0:
            start_timer(&error_timer, 3.0f);
            strcpy(error_text, "NOT ENOUGH SPACE IN VMU!");
            break;
        case 1:
            change_scene(MAINMENU);
            break;
    }
}

void draw_loading_scene() {
    static void (*callback)() = NULL;

    if (do_button(load_button) && error_timer.is_done) {
        if (error_timer.is_done) {
            callback = load_game_callback;
            selected_layer = 1;
            selected_column = 0;
            selected_row = 0;
        }
    }

    if (do_button(new_game_button) && error_timer.is_done) {
        callback = new_game_callback;
        selected_layer = 1;
        selected_column = 0;
        selected_row = 0;
    }

    draw_confirmation_window(callback);
    draw_error_popup();
}