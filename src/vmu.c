#include "vmu_animations.h"

// Vmu animations are drawn to the vmu frame buffer, and then presented to the screen
// Each frame is just an array of raw bits, 1bpp (see vmu_animations.h)

vmufb_t vmu_fb;
int vmu_current_frame = 0;

#define SET_VMU_ANIMATION(animation) (vmu_current_animation = animation, vmu_current_num_frames = sizeof(animation) / sizeof(animation[0]))

void *draw_vmu_animation(void *param) {
    static float last_vmu_update = 0;
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);

    if (!vmu) return NULL;

    float now = GetTime();
    if (now - last_vmu_update < 0.25f) return NULL;

    last_vmu_update = now;

    const char **vmu_current_animation = NULL;
    int vmu_current_num_frames = 0;

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
            // TODO: unlockables VMU animation
            break;
        case OPTIONS:
            SET_VMU_ANIMATION(vmu_knob_animation);
            break;
        case CREDITS:
            SET_VMU_ANIMATION(vmu_credits_animation);
            break;
    }

    if (!vmu_current_animation) return NULL;

    vmu_current_frame = (vmu_current_frame + 1) % vmu_current_num_frames;
    vmufb_paint_area(&vmu_fb, 0, 0, 48, 32, vmu_current_animation[vmu_current_frame]);
    vmufb_present(&vmu_fb, vmu);

    return NULL;
};
