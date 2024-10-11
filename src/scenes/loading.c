#include <stddef.h>
#include <stdio.h>
#include "../ui.h"
#include "../timer.h"
#include "../config.h"
#include "../scene.h"
#include "../save.h"
#include "../background.h"
#include "loading.h"

extern uint8_t row_count[];
extern uint8_t column_count[];
extern uint8_t selected_layer;

uibutton_t load_button = {
    .pos    = { .x = 10,  .y = SCREEN_HEIGHT * 0.45 },
    .size   = { .x = 300, .y = 50                   },
    .column = 0,
    .row    = 0,
    .text   = "Load",
};

uibutton_t new_game_button = {
    .pos    = { .x = 320, .y = SCREEN_HEIGHT * 0.45 },
    .size   = { .x = 300, .y = 50                   },
    .column = 1,
    .row    = 0,
    .text   = "New",
};

bbox_timer_t error_popup_timer;
char         error_text[50];

void init_loading_scene(void) {
    column_count[0] = 2;
    row_count[0]    = 1;
}

void update_loading_scene(void) {
    update_timer(&error_popup_timer);
}

void draw_error_popup(void) {
    if (!error_popup_timer.is_running) return;

    DrawRectangle(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 100, 500, 200, (Color){ 1, 17, 34, 220 });
    DrawText(error_text, (int)(SCREEN_WIDTH / 2 - MeasureText(error_text, 32) / 2), (int)(SCREEN_HEIGHT / 2 - 15), 32,
             RED);
}

void load_game_callback(void *user_data) {
    int return_code = load_game();

    switch (return_code) {
        case -1:
            start_timer(&error_popup_timer, 3.0f);
            snprintf(error_text, sizeof(error_text), "NO VMU FOUND!");
            break;
        case 0:
            start_timer(&error_popup_timer, 3.0f);
            snprintf(error_text, sizeof(error_text), "NO SAVE DATA FOUND!");
            break;
        case 1:
            change_scene(MAINMENU);
            break;
    }
}

static void new_game_callback(void *user_data) {
    new_game();
    int return_code = save_game();

    switch (return_code) {
        case -1:
            start_timer(&error_popup_timer, 3.0f);
            snprintf(error_text, sizeof(error_text), "NO VMU FOUND!");
            break;
        case 0:
            start_timer(&error_popup_timer, 3.0f);
            snprintf(error_text, sizeof(error_text), "NOT ENOUGH SPACE IN VMU!");
            break;
        case 1:
            change_scene(MAINMENU);
            break;
    }
}

void draw_loading_scene(void) {
    draw_background();

    static void (*callback)(void *user_data) = NULL;

    if (do_button(load_button, true) && !error_popup_timer.is_running) {
        callback       = load_game_callback;
        selected_layer = 1;
    }

    if (do_button(new_game_button, true) && !error_popup_timer.is_running) {
        callback       = new_game_callback;
        selected_layer = 1;
    }

    draw_confirmation_window(callback, NULL);
    draw_error_popup();
}