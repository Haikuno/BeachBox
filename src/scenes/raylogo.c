void update_raylogo_scene() {
    if (IsGamepadButtonPressed(0, START)) {
        change_scene(LOADING);
    }
}

void draw_raylogo_scene() {
    DrawText("raylib", 100, 100, 44, BLACK);
}