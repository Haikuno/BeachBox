#include <stddef.h>
#include <raylib.h>
#include <stdbool.h>
#include <kos/thread.h>
#include "src/audio.h"
#include "src/config.h"
#include "src/scene.h"
#include "src/save.h"
#include "src/controller.h"
#include "src/player.h"
#include "src/hats.h"
#include "src/vmu.h"

extern save_t  save;
// extern scene_t current_scene;
extern uint8_t selected_column;
extern uint8_t selected_row;
extern uint8_t column_count[];
extern uint8_t row_count[];

void init_game(void) {
    change_scene(RAYLOGO);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BeachBox");
    load_hats();
}

void update_game(void) {
    update_controller();
    update_song();
    update_current_scene();

#ifdef DEBUG_INFINITE_COINS
    save.total_coins = 9999;
#endif
}

void draw_game(void) {
    BeginDrawing();
    draw_current_scene();
#ifdef DEBUG_DRAW_FPS
    DrawRectangle(0, 440, 130, 50, (Color){ 22, 22, 22, 200 });
    DrawFPS(27, 450);
#endif

#ifdef DEBUG_DRAW_CURSOR_INFO
    DrawRectangle(320, 20, 300, 120, (Color){ 22, 22, 22, 200 });
    DrawText(TextFormat("col: %d, row: %d", selected_column, selected_row), 395, 40, 20, RED);
    DrawText(TextFormat("col_count: %d, row_count: %d", column_count[selected_row], row_count[selected_column]), 345,
             80, 20, RED);
#endif
    EndDrawing();
    thd_create(1, draw_vmu_animation, NULL);
}

int main(int argc, char **argv) {
    init_game();
    init_sounds();

    while (true) {
        update_game();
        draw_game();
    }

    return 0;
}