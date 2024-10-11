#include <raylib.h>
#include "controller.h"
#include "scene.h"
#include "ui.h"
#include "player.h"

extern scene_t current_scene;
extern bool    is_game_over;
extern bool    is_game_paused;

bool IsTriggerPressed(int gamepad, trigger_t trigger) {
    static bool is_left_trigger_down;
    static bool is_right_trigger_down;
    const float trigger_threshold = 0.5f;

    bool *is_trigger_down  = (trigger == LEFT_TRIGGER) ? &is_left_trigger_down : &is_right_trigger_down;
    float trigger_movement = GetGamepadAxisMovement(0, trigger);

    if (trigger_movement > trigger_threshold && !*is_trigger_down) {
        *is_trigger_down = true;
        return true;
    }

    if (trigger_movement <= trigger_threshold && *is_trigger_down) {
        *is_trigger_down = false;
    }

    return false;
}

// NOTE: This is in PascalCase to follow raylib naming conventions
void update_controller(void) {
    switch (current_scene) {
        case GAME:
            if (is_game_over) {
                if (IsGamepadButtonPressed(0, DPAD_LEFT)) {
                    move_cursor('L');
                } else if (IsGamepadButtonPressed(0, DPAD_RIGHT)) {
                    move_cursor('R');
                }
                break;
            }

            if (IsGamepadButtonPressed(0, START)) {
                is_game_paused = !is_game_paused;
            }

            if (is_game_paused) break;

            float axis_movement_left_x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);

            if (axis_movement_left_x != 0) {
                move_player((Vector2){ .x = axis_movement_left_x, .y = 0 });
            } else {
                if (IsGamepadButtonDown(0, DPAD_LEFT)) {
                    move_player((Vector2){ .x = -1, .y = 0 });
                }
                if (IsGamepadButtonDown(0, DPAD_RIGHT)) {
                    move_player((Vector2){ .x = 1, .y = 0 });
                }
            }

            if (IsGamepadButtonDown(0, A)) {
                jump();
            } else if (IsGamepadButtonReleased(0, A)) {
                cut_jump();
            }

            if (IsTriggerPressed(0, LEFT_TRIGGER)) {
                slow_down();
            }
            if (IsTriggerPressed(0, RIGHT_TRIGGER)) {
                teleport();
            }

            shift_player(IsGamepadButtonDown(0, X));
            break;

        case RAYLOGO:
            if (IsGamepadButtonPressed(0, START)) {
                change_scene(LOADING);
            }

        case LOADING:
        case MAINMENU:
        case SHOP:
        case UNLOCKABLES:
        case OPTIONS:
        case CREDITS:
            if (IsGamepadButtonPressed(0, DPAD_LEFT)) {
                move_cursor('L');
            }
            if (IsGamepadButtonPressed(0, DPAD_RIGHT)) {
                move_cursor('R');
            }
            if (IsGamepadButtonPressed(0, DPAD_UP)) {
                move_cursor('U');
            }
            if (IsGamepadButtonPressed(0, DPAD_DOWN)) {
                move_cursor('D');
            }
            break;

        default:
            break;
    }
}