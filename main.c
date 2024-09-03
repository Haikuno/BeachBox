#include <kos.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glkos.h>
#include <raylib.h>
#include "dc/maple.h"
#include "dc/vmufs.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRAVITY 0.5

// Debug settings, just comment one out to turn it on
#define DEBUG_DRAW_FPS
// #define DEBUG_DRAW_CURSOR_INFO
#define DEBUG_GODMODE

enum Scene {
    LOADING,
    MAINMENU,
    GAME,
    SHOP,
    OPTIONS,
    CREDITS
} current_scene = LOADING;

#include "src/helper_functions.c"

#include "src/save.c"

#include "src/player.c"
#include "src/objects.c"

#include "src/ui.c"

#include "src/controller.c"

#include "src/scenes/loading.c"
#include "src/scenes/mainmenu.c"
#include "src/scenes/game.c"
#include "src/scenes/shop.c"
#include "src/scenes/options.c"
#include "src/scenes/credits.c"

void init_game() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BeachBox");
}

void update_game() {
    update_controller();

    switch (current_scene) {
        case LOADING:
            update_loading_scene();
            break;
        case MAINMENU:
            update_mainmenu_scene();
            break;
        case GAME:
            update_game_scene();
            break;
        default:
            break;
    }
}

void draw_game() {
    BeginDrawing();

    #ifdef DEBUG_DRAW_FPS
    DrawFPS(    400, 10);
    #endif

    #ifdef DEBUG_DRAW_CURSOR_INFO
    DrawText(TextFormat("col: %d, row: %d", selected_column, selected_row), 430, 30, 20, BLACK);
    DrawText(TextFormat("col_count: %d, row_count: %d", column_count[selected_row], row_count[selected_column]), 350, 50, 20, BLACK);
    #endif

    switch (current_scene) {
        case LOADING:
            draw_loading_scene();
            break;
        case MAINMENU:
            draw_mainmenu_scene();
            break;
        case GAME:
            draw_game_scene();
            break;
        default:
            break;
    }

    EndDrawing();
}

int main(void) {
    init_game();

    // Main game loop
    while (1) {
        update_game();
        draw_game();
    }
}