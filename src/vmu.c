#include "vmu_frames.h"

vmufb_t vmu_fb;
int vmu_current_frame = 0;

void draw_vmu_animation() {
    static float last_vmu_update = 0;
    maple_device_t *vmu = maple_enum_type(0, MAPLE_FUNC_MEMCARD);

    if (!vmu) return;

    float now = GetTime();
    if (now - last_vmu_update < 0.25f) return;

    last_vmu_update = now;

    const char **vmu_current_animation = NULL;
    int vmu_current_num_frames = 0;

    switch (current_scene) {
        case RAYLOGO:
            break;
        case LOADING:
            vmu_current_animation = vmu_frames_loading;
            vmu_current_num_frames = sizeof(vmu_frames_loading) / sizeof(vmu_frames_loading[0]);
            break;
        case MAINMENU:
            break;
        case GAME:
            if (is_game_over) {
                vmu_current_animation = vmu_frames_game_over;
                vmu_current_num_frames = sizeof(vmu_frames_game_over) / sizeof(vmu_frames_game_over[0]);
            } else {
                vmu_current_animation = vmu_frames_game_scene;
                vmu_current_num_frames = sizeof(vmu_frames_game_scene) / sizeof(vmu_frames_game_scene[0]);
            }
            break;
        case SHOP:
            break;
        case OPTIONS:
            break;
        case CREDITS:
            break;
    }

    if (!vmu_current_animation) return;

    vmu_current_frame = (vmu_current_frame + 1) % vmu_current_num_frames;
    vmufb_paint_area(&vmu_fb, 0, 0, 48, 32, vmu_current_animation[vmu_current_frame]);
    vmufb_present(&vmu_fb, vmu);
};
