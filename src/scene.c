#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <kos/thread.h>
#include "scene.h"
#include "ui.h"
#include "save.h"
#include "helper_functions.h"
#include "timer.h"
#include "scenes/raylogo.h"
#include "scenes/loading.h"
#include "scenes/options.h"
#include "scenes/mainmenu.h"
#include "scenes/game.h"
#include "scenes/credits.h"
#include "scenes/shop.h"
#include "scenes/unlockables.h"

extern uint8_t selected_column;
extern uint8_t selected_row;
extern uint8_t selected_layer;

extern uint8_t row_count[];
extern uint8_t column_count[];

extern uint8_t      vmu_current_frame;
extern uint8_t      vmu_menu_text_frame;
extern bbox_timer_t vmu_menu_text_update_cooldown;
extern bbox_timer_t save_popup_timer;

scene_t current_scene;

static void *save_game_wrapper(void *arg) {
    save_game();
    return NULL;
}

void change_scene(scene_t scene) {
    static void (*init_scene_functions[])(void) = { [RAYLOGO] = init_raylogo_scene, [LOADING] = init_loading_scene,         [MAINMENU] = init_mainmenu_scene, [GAME] = init_game_scene,
                                                    [SHOP] = init_shop_scene,       [UNLOCKABLES] = init_unlockables_scene, [OPTIONS] = init_options_scene,   [CREDITS] = init_credits_scene };

    for (int i = 0; i < MAX_COLUMNS; i++) {
        column_count[i] = 0;
    }

    for (int i = 0; i < MAX_ROWS; i++) {
        row_count[i] = 0;
    }

    selected_column = 0;
    selected_row    = 0;
    selected_layer  = 0;

    vmu_current_frame = 0;

    switch (scene) {
        case GAME:
            if (current_scene == GAME) {
                start_timer(&save_popup_timer, 2.0f);
                thd_create(1, save_game_wrapper, NULL);
            }
            break;
        case MAINMENU:
            start_timer(&vmu_menu_text_update_cooldown, 0.5f); // First frame is longer to give the player time to look at the VMU
            vmu_menu_text_frame = 0;
            if (current_scene != LOADING && current_scene != CREDITS) {
                start_timer(&save_popup_timer, 2.0f);
                thd_create(1, save_game_wrapper, NULL);
            }
            if (current_scene == CREDITS) {
                unload_credits_images();
            }
        default:
            break;
    }
    init_scene_functions[scene]();
    current_scene = scene;
}

void update_current_scene(void) {
    static void (*update_scene_functions[])(void)
        = { [RAYLOGO] = update_raylogo_scene, [LOADING] = update_loading_scene,         [MAINMENU] = update_mainmenu_scene, [GAME] = update_game_scene,
            [SHOP] = update_shop_scene,       [UNLOCKABLES] = update_unlockables_scene, [OPTIONS] = update_options_scene,   [CREDITS] = update_credits_scene };
    update_scene_functions[current_scene]();
    update_timer(&save_popup_timer);
}

void draw_current_scene(void) {
    static void (*draw_scene_functions[])(void) = { [RAYLOGO] = draw_raylogo_scene, [LOADING] = draw_loading_scene,         [MAINMENU] = draw_mainmenu_scene, [GAME] = draw_game_scene,
                                                    [SHOP] = draw_shop_scene,       [UNLOCKABLES] = draw_unlockables_scene, [OPTIONS] = draw_options_scene,   [CREDITS] = draw_credits_scene };
    draw_scene_functions[current_scene]();
    draw_save_popup();
}