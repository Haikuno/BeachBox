#include <raylib.h>
#include <stddef.h>
#include "../scene.h"
#include "../ui.h"
#include "../config.h"
#include "../background.h"
#include "../save.h"
#include "options.h"

extern uint8_t     column_count[];
extern uint8_t     row_count[];
extern const Color ui_background_color;
extern uint8_t     selected_layer;
extern uint8_t     selected_column;
extern uint8_t     selected_row;

// TODO: finish options scene

const uibutton_t newsave_options_button = {
    .pos    = { .x = 43,  .y = 220 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 0,
    .layer  = 0,
    .text   = "New Save",
};

const uibutton_t exit_options_button = {
    .pos    = { .x = 43,  .y = 320 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 1,
    .layer  = 0,
    .text   = "Return",
};

void init_options_scene(void) {
    row_count[0]    = 2;
    column_count[0] = 1;
    selected_row    = 1;
}

void update_options_scene(void) {
    //
}

static void new_game_callback(int option, void *user_data) {
    if (option == 1) { // yes pressed
        new_game();
        change_scene(MAINMENU);
        return;
    }
    if (option == 0) { // no pressed
        init_options_scene();
        return;
    }
}

void draw_options_scene(void) {
    draw_background();
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ui_background_color);

    // TODO: add options (music volume, sfx volume)
    DrawText("WIP!", (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 20, 30, RAYWHITE);

    static void (*callback)(int option, void *user_data) = NULL;

    if (do_button(newsave_options_button, true)) {
        callback        = new_game_callback;
        selected_layer  = 1;
        selected_column = 0;
        selected_row    = 0;
    }

    if (do_button(exit_options_button, true)) {
        change_scene(MAINMENU);
    }

    draw_confirmation_window(callback, NULL, "Start a new game?");
}