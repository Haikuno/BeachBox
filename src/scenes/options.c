// TODO: finish options scene

struct UiButton exit_options_button = {
    .pos = {.x = 43, .y = 320},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Return",
};

void update_options_scene() {
    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    row_count[0] = 1;
    column_count[0] = 1;
}

void draw_options_scene() {
    draw_background();
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ui_background_color);

    // TODO: add actual credits
    DrawText("WIP!", (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 20, 30, RAYWHITE);

    if (do_button(exit_options_button, true)) {
        change_scene(MAINMENU);
    }
}