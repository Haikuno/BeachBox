// TODO: finish options scene

struct UiButton sfx_volume_options_button = {
    .pos = {.x = 43, .y = 100},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "SFX Volume",
};
struct UiButton music_volume_options_button = {
    .pos = {.x = 43, .y = 160},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 1,
    .layer = 0,
    .text = "Music Volume",
};

struct UiButton newsave_options_button = {
    .pos = {.x = 43, .y = 220},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 2,
    .layer = 0,
    .text = "New Save",
};

struct UiButton exit_options_button = {
    .pos = {.x = 43, .y = 320},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 3,
    .layer = 0,
    .text = "Return",
};



void update_options_scene(void) {
    for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    row_count[0] = 4;
    column_count[0] = 1;
}



void draw_options_scene(void) {
    draw_background();
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ui_background_color);

    static void (*callback)() = NULL;

    // TODO: add options (music volume, sfx volume)
    DrawText("WIP!", (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 20, 30, RAYWHITE);

    if (do_button(exit_options_button, true)) {
        change_scene(MAINMENU);
    }
    if (do_button(sfx_volume_options_button, true)) {
        volume_bool = 1;
        selected_layer = 2;
        selected_column = 0;
        selected_row = 0;
        
    }
    if (do_button(music_volume_options_button, true)) {
        selected_layer = 2;
        selected_column = 0;
        selected_row = 0;
        
    }
    if (do_button(newsave_options_button, true)) {
        callback = new_game_callback;
        selected_layer = 1;
        selected_column = 0;
        selected_row = 0;
    }

    draw_confirmation_window(callback);
    draw_sfx_volume_window(callback);
    draw_music_volume_window(callback);
    draw_saved_popup();
}