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

// Called when "NO" is pressed when exiting, to reset column and row count
static void init_options_scene_wrapper(void *user_data) {
    init_options_scene();
}

void update_options_scene(void) {
    //
}

static void new_game_wrapper(void *user_data) {
    new_game();
    change_scene(MAINMENU);
    return;
}

void draw_options_scene(void) {
    draw_background();
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ui_background_color);

    static void (*callback)(void *user_data) = NULL;

    // TODO: add options (music volume, sfx volume)
    DrawText("WIP!", (int)(SCREEN_WIDTH / 4) + 95, (int)(SCREEN_HEIGHT / 4) + 20, 30, RAYWHITE);

    if (do_button(newsave_options_button, true)) {
        callback        = new_game_wrapper;
        selected_layer  = 1;
        selected_column = 0;
        selected_row    = 0;
    }

    if (do_button(exit_options_button, true)) {
        change_scene(MAINMENU);
    }

    draw_confirmation_window(callback, NULL, init_options_scene_wrapper, NULL, "Start a new game?");
}