#include <kos.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glkos.h>
#include "dc/maple.h"
#include "dc/vmufs.h"
#include <dc/vmu_fb.h>
#include <raylib.h>
#include "raymath.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRAVITY 0.55

// Debug settings, just comment one out to turn it on

// #define DEBUG_DRAW_FPS
// #define DEBUG_DRAW_CURSOR_INFO
// #define DEBUG_GODMODE
// #define DEBUG_INFINITE_COINS

enum Scene {
    RAYLOGO,
    LOADING,
    MAINMENU,
    GAME,
    SHOP,
    UNLOCKABLES,
    OPTIONS,
    CREDITS
} current_scene = RAYLOGO;

enum Hats {
    HAT_NIL,
    HAT_SLIME_RED,
    HAT_SLIME_BLUE,
    HAT_BOX,
    HAT_M,
    HAT_L,
    HAT_Z,
    HAT_F,
    HAT_MUPRH,
    HAT_CROWN,
    HAT_MAX
};

#include "src/helper_functions.c"
#include "src/timer.c"
#include "src/save.c"
#include "src/player.c"
#include "src/objects.c"
#include "src/controller.c"
#include "src/background.c"
#include "src/vmu.c"
#include "src/ui.c"

#include "src/scenes/raylogo.c"
#include "src/scenes/loading.c"
#include "src/scenes/mainmenu.c"
#include "src/scenes/game.c"
#include "src/scenes/shop.c"
#include "src/scenes/unlockables.c"
#include "src/scenes/options.c"
#include "src/scenes/credits.c"

void init_game(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BeachBox");
    hats[HAT_SLIME_RED] = LoadTexture("rd/hats/slime_red.png");
    hats[HAT_SLIME_BLUE] = LoadTexture("rd/hats/slime_blue.png");
    hats[HAT_BOX] = LoadTexture("rd/hats/box.png");
    hats[HAT_M] = LoadTexture("rd/hats/m.png");
    hats[HAT_L] = LoadTexture("rd/hats/l.png");
    hats[HAT_Z] = LoadTexture("rd/hats/z.png");
    hats[HAT_F] = LoadTexture("rd/hats/f.png");
    hats[HAT_MUPRH] = LoadTexture("rd/hats/murph.png");
    hats[HAT_CROWN] = LoadTexture("rd/hats/crown.png");
}

void update_game(void) {
    update_controller();
    update_timer(&save_popup_timer);

    static void (*update_scene_functions[])(void) = {
        [RAYLOGO] = update_raylogo_scene,
        [LOADING] = update_loading_scene,
        [MAINMENU] = update_mainmenu_scene,
        [GAME] = update_game_scene,
        [SHOP] = update_shop_scene,
        [UNLOCKABLES] = update_unlockables_scene,
        [OPTIONS] = update_options_scene,
        [CREDITS] = update_credits_scene};

    update_scene_functions[current_scene]();

#ifdef DEBUG_INFINITE_COINS
    save.total_coins = 9999;
#endif
}

void draw_game(void) {
    thd_create(1, draw_vmu_animation, NULL);
    BeginDrawing();
    static void (*draw_scene_functions[])(void) = {
        [RAYLOGO] = draw_raylogo_scene,
        [LOADING] = draw_loading_scene,
        [MAINMENU] = draw_mainmenu_scene,
        [GAME] = draw_game_scene,
        [SHOP] = draw_shop_scene,
        [UNLOCKABLES] = draw_unlockables_scene,
        [OPTIONS] = draw_options_scene,
        [CREDITS] = draw_credits_scene,
    };

    draw_scene_functions[current_scene]();

#ifdef DEBUG_DRAW_FPS
    DrawRectangle(0, 440, 130, 50, (Color){22, 22, 22, 200});
    DrawFPS(27, 450);
#endif

#ifdef DEBUG_DRAW_CURSOR_INFO
    DrawRectangle(320, 20, 300, 120, (Color){22, 22, 22, 200});
    DrawText(TextFormat("col: %d, row: %d", selected_column, selected_row), 395, 40, 20, RED);
    DrawText(TextFormat("col_count: %d, row_count: %d", column_count[selected_row], row_count[selected_column]), 345, 80, 20, RED);
#endif

    draw_save_popup();

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