#define RAYLOGO_WIDTH 290
#define RAYLOGO_HEIGHT 300

#define RAYLOGO_SQUARE_SIZE 15

#define RAYLOGO_COLUMNS RAYLOGO_WIDTH / RAYLOGO_SQUARE_SIZE
#define RAYLOGO_ROWS RAYLOGO_HEIGHT / RAYLOGO_SQUARE_SIZE

struct Timer raylogo_timer = {.duration = 3.5f, .is_done = false};

const float animation_duration = 1.0f;
float time_elapsed = 0.0f;

void update_raylogo_scene() {
    if (IsGamepadButtonPressed(0, START) || raylogo_timer.is_done) {
        change_scene(LOADING);
    }
    update_timer(&raylogo_timer);
    time_elapsed = GetTime() - animation_duration;
}

void draw_raylogo_scene() {
    ClearBackground(RAYWHITE);

    // Text

    Vector2 text_pos = (Vector2){SCREEN_WIDTH / 2 - MeasureText("raylib", 52) / 2 + 35, SCREEN_HEIGHT / 2 + 65};

    Vector2 white_box_size = {
        .x = (MeasureText("raylib", 52) + 40) * (1 - time_elapsed),
        .y = 80,
    };

    DrawText("raylib", (int)text_pos.x, (int)text_pos.y, 52, BLACK);
    DrawRectangleV(text_pos, white_box_size, RAYWHITE);

    // Borders

    for (int row = 0; row < RAYLOGO_ROWS; row++) {
        for (int column = 0; column < RAYLOGO_COLUMNS; column++) {
            bool is_border_square = row == 0 || column == 0 || row == RAYLOGO_ROWS - 1 || column == RAYLOGO_COLUMNS - 1;
            if (!is_border_square) continue;

            float delay = 1 * (fabs(column - row) / (float)((RAYLOGO_ROWS + RAYLOGO_COLUMNS) / 2));
            float amount = MIN(1, MAX(0, time_elapsed - delay) * 2);
            float current_size = Lerp(0, RAYLOGO_SQUARE_SIZE, amount);

            Vector2 pos = {
                .x = SCREEN_WIDTH / 2 - RAYLOGO_WIDTH / 2 + column * RAYLOGO_SQUARE_SIZE,
                .y = SCREEN_HEIGHT / 2 - RAYLOGO_HEIGHT / 2 + row * RAYLOGO_SQUARE_SIZE};

            DrawRectangleV(pos, (Vector2){current_size, current_size}, BLACK);
        }
    }
}