#include <stdio.h>
#include <dc/maple.h>
#include <dc/vmu_fb.h>
#include "save.h"
#include "vmu.h"
#include "vmu_animations.h"
#include "timer.h"
#include "scene.h"

#define SET_VMU_ANIMATION(animation) (vmu_current_animation = animation, vmu_current_num_frames = sizeof(animation) / sizeof(animation[0]))

// Vmu animations are drawn to the vmu frame buffer, and then presented to the screen
// Each frame is just an array of raw bits, 1bpp (see vmu_animations.h)

extern scene_t current_scene;
extern bool    is_game_over;

static vmufb_t vmu_fb;
uint8_t        vmu_current_frame   = 0;
uint8_t        vmu_menu_text_frame = 0;
// The first frame is longer to give player time to look at the vmu
bbox_timer_t vmu_menu_text_update_cooldown = { .duration = 0.5f, .is_running = true, .progress = 0 };

static void update_vmu_menu_text(void) {
    update_timer(&vmu_menu_text_update_cooldown);

    char buffer[16];
    switch (vmu_menu_text_frame) {
        case 0:
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 4, 1, 48, 6, 2, "Hello, and");
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 8, 7, 48, 6, 2, "welcome!");
            break;
        case 1:
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 2, 1, 48, 6, 2, "I hope that");
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 5, 7, 48, 6, 2, "you enjoy ");
            break;
        case 2:
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 5, 5, 48, 6, 2, "BeachBox!!");
            break;
        case 3:
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 4, 1, 48, 6, 2, "Check out");
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 8, 7, 48, 6, 2, "the shop-");
            break;
        case 4:
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 4, 1, 48, 6, 2, "-and spend");
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 3, 7, 48, 6, 2, "your money!");
            break;
        case 5:
            snprintf(buffer, sizeof(buffer), "You have %d", get_total_coins());
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 1, 1, 48, 6, 2, buffer);
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 3, 7, 48, 6, 2, "coins left");
            break;
        case 6:
            snprintf(buffer, sizeof(buffer), "%d times!!", get_total_runs());
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 4, 1, 48, 6, 2, "You played");
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 8, 7, 48, 6, 2, buffer);
            break;
        case 7:
            vmufb_print_string_into(&vmu_fb, &vmufb_font4x6, 8, 5, 48, 6, 2, "Congrats!");
            break;
    }

    if (vmu_menu_text_update_cooldown.is_running) {
        return;
    }

    start_timer(&vmu_menu_text_update_cooldown, 0.2f);

    vmu_menu_text_frame = (vmu_menu_text_frame + 1) % 8;
}

static void update_vmu_credits_animation(void) {
    static const char data[192]     = { 0 };
    static int        current_width = 1;
    static bool       should_grow   = true;

    current_width = (current_width + (should_grow ? 1 : -1)) % 49;
    if (current_width == 0 || current_width == 48) should_grow = !should_grow;

    vmufb_paint_area(&vmu_fb, 0, 0, current_width, 32, data);
}

void *draw_vmu_animation(void *param) {
    if (is_save_in_progress() || is_load_in_progress()) return nullptr;

    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);

    if (!vmu) return nullptr;

    static bbox_timer_t vmu_update_cooldown;
    update_timer(&vmu_update_cooldown);

    if (vmu_update_cooldown.is_running) {
        return nullptr;
    }

    start_timer(&vmu_update_cooldown, 0.2f);

    const char **vmu_current_animation  = nullptr;
    int          vmu_current_num_frames = 0;
    static scene_t current_scene;
    current_scene = get_current_scene();
    switch (current_scene) {
        case RAYLOGO:
            SET_VMU_ANIMATION(vmu_raylib_animation);
            break;
        case LOADING:
            SET_VMU_ANIMATION(vmu_loading_animation);
            break;
        case MAINMENU:
            SET_VMU_ANIMATION(vmu_face_animation);
            break;
        case GAME:
            if (is_game_over) {
                SET_VMU_ANIMATION(vmu_game_over_animation);
            } else {
                SET_VMU_ANIMATION(vmu_game_scene_animation);
            }
            break;
        case SHOP:
            SET_VMU_ANIMATION(vmu_shop_animation);
            break;
        case UNLOCKABLES:
            SET_VMU_ANIMATION(vmu_unlockables_animation);
            break;
        case OPTIONS:
            SET_VMU_ANIMATION(vmu_options_animation);
            break;
        case CREDITS:
            SET_VMU_ANIMATION(vmu_credits_animation);
            break;
    }

    if (!vmu_current_animation) return nullptr;

    vmu_current_frame = (vmu_current_frame + 1) % vmu_current_num_frames;
    vmufb_paint_area(&vmu_fb, 0, 0, 48, 32, vmu_current_animation[vmu_current_frame]);

    if (current_scene == MAINMENU) update_vmu_menu_text();
    if (current_scene == CREDITS) update_vmu_credits_animation();

    vmufb_present(&vmu_fb, vmu);

    return nullptr;
};
