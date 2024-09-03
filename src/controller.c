enum Buttons {
    DPAD_UP = 1,
    DPAD_RIGHT,
    DPAD_DOWN,
    DPAD_LEFT,
    Y,
    B,
    A,
    X,
    START = 15
};

void update_controller() {
    switch (current_scene) {
        case GAME:
            if (IsGamepadButtonDown(0, DPAD_LEFT)) {
                if (!is_game_over) {
                    move_player((Vector2){.x = -1, .y = 0});
                } else {
                    move_cursor('L');
                }
            }
            if (IsGamepadButtonDown(0, DPAD_RIGHT)) {
                if (!is_game_over) {
                    move_player((Vector2){.x = 1, .y = 0});
                } else {
                    move_cursor('R');                }
            }

            if (IsGamepadButtonDown(0, A)) {
                jump();
            } else if (IsGamepadButtonReleased(0, A)) {
                cut_jump();
            }

            if (IsGamepadButtonPressed(0, X)) {
                toggle_player_shift();
            } else if (IsGamepadButtonReleased(0, X)) {
                toggle_player_shift();
            }
            break;

        case LOADING:
        case MAINMENU:
        case SHOP:
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