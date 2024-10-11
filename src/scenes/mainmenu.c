#include "mainmenu.h"
#include "../ui.h"
#include "../scene.h"
#include "../save.h"
#include "../helper_functions.h"
#include "../config.h"
#include "../background.h"

extern uint8_t column_count[];
extern uint8_t row_count[];
extern Color   ui_background_color;
extern save_t  save;

uibutton_t play_button = {
    .pos    = { .x = 43,  .y = 170 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 0,
    .layer  = 0,
    .text   = "Play",
};

uibutton_t shop_button = {
    .pos    = { .x = 43,  .y = 220 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 1,
    .layer  = 0,
    .text   = "Shop",
};

uibutton_t unlockables_button = {
    .pos    = { .x = 43,  .y = 270 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 2,
    .layer  = 0,
    .text   = "Unlockables",
};

uibutton_t options_button = {
    .pos    = { .x = 43,  .y = 320 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 3,
    .layer  = 0,
    .text   = "Options",
};

uibutton_t credits_button = {
    .pos    = { .x = 43,  .y = 370 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 4,
    .layer  = 0,
    .text   = "Credits",
};

void init_mainmenu_scene(void) {
    row_count[0]    = 5;
    column_count[0] = 1;
}

void update_mainmenu_scene(void) {
    //
}

void draw_mainmenu_scene(void) {
    draw_background();
    DrawText("BeachBox", 100, 100, 44, BLACK);
    DrawText("BeachBox", 98, 100, 44, RAYWHITE);

    if (do_button(play_button, true)) {
        change_scene(GAME);
    }
    if (do_button(shop_button, true)) {
        change_scene(SHOP);
    }
    if (do_button(unlockables_button, true)) {
        change_scene(UNLOCKABLES);
    }
    if (do_button(options_button, true)) {
        change_scene(OPTIONS);
    }
    if (do_button(credits_button, true)) {
        change_scene(CREDITS);
    }

    DrawRectangle(380, 80, 200, 100, ui_background_color);
    DrawText(TextFormat("Coins: %d", save.total_coins), 400, 100, 20, RAYWHITE);
    DrawText(TextFormat("Runs: %d", save.total_runs), 400, 120, 20, RAYWHITE);
    DrawText(TextFormat("Highscore: %d", save.high_score), 400, 140, 20, RAYWHITE);
    DrawRectangleLinesV((Vector2){ .x = 380, .y = 80 }, (Vector2){ .x = 200, .y = 100 }, RAYWHITE);
}