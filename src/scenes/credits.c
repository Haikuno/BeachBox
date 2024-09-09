// TODO: finish credits scene

struct UiButton exit_credits_button = {
    .pos = {.x = 43, .y = 370},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Return",
};

void update_credits_scene() {
    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    row_count[0] = 1;
    column_count[0] = 1;
}

void draw_credits_scene() {
    draw_background();
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ui_button_color);

    // TODO: add actual credits
    const char *credits_text = "Made by Psyops :)";
    DrawText(credits_text, (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 20, 30, RAYWHITE);

    if (do_button(exit_credits_button)) {
        change_scene(MAINMENU);
    }
}