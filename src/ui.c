#define MAX_COLUMNS 10
#define MAX_ROWS 10

uint8_t selected_column = 0;
uint8_t selected_row = 0;
uint8_t selected_layer = 0;  // Used for confirmation windows

uint8_t row_count[MAX_COLUMNS] = {1};
uint8_t column_count[MAX_ROWS] = {1};

// The struct that holds the data for each button
struct UiButton {
    Vector2 pos;
    Vector2 size;
    const char *text;
    uint8_t column;
    uint8_t row;
    uint8_t layer;
};

// Forward declarations
void init_game_scene();

// Changes the current scene, and calls the corresponding init function (if needed)
inline void change_scene(enum Scene scene) {
    current_scene = scene;
    selected_column = 0;
    selected_row = 0;
    selected_layer = 0;
    vmu_current_frame = 0;

    switch (scene) {
        case GAME:
            init_game_scene();
            break;
        default:
            break;
    }
}

// Moves the "cursor" when the player presses the DPAD, changing the selected UiButton
void move_cursor(char direction) {
    if (selected_layer == 1) {
        row_count[0] = 1;
        column_count[0] = 2;
    }

    const uint8_t prev_column = selected_column;
    const uint8_t prev_row = selected_row;

    bool can_move_cursor_left = selected_column > 0 || (selected_column != 0 && row_count[selected_column - 1] > 0);
    bool can_move_cursor_right = selected_column < column_count[selected_row] - 1 || (selected_column < MAX_COLUMNS && row_count[selected_column + 1] > 0);
    bool can_move_cursor_up = selected_row > 0;
    bool can_move_cursor_down = selected_row < row_count[selected_column] - 1;

    switch (direction) {
        case 'L':
            if (can_move_cursor_left) {
                selected_column--;
            }
            selected_row = MIN(selected_row, row_count[selected_column] - 1);
            break;

        case 'R':
            if (can_move_cursor_right) {
                selected_column++;
            }
            selected_row = MIN(selected_row, row_count[selected_column] - 1);
            break;

        case 'U':
            if (can_move_cursor_up) {
                selected_row--;
            } else {
                selected_row = row_count[selected_column] - 1;  // Loop around
            }
            selected_column = MIN(selected_column, column_count[selected_row] - 1);
            break;

        case 'D':
            if (can_move_cursor_down) {
                selected_row++;
            } else {
                selected_row = 0;  // Loop around
            }
            selected_column = MIN(selected_column, column_count[selected_row] - 1);
            break;

        default:
            break;
    }

    // Prevent overflow
    if (selected_row == 255) selected_row = prev_row;
    if (selected_column == 255) selected_column = prev_column;
}

// Checks if the passed UiButton is selected
inline bool is_button_selected(struct UiButton button) {
    return button.column == selected_column && button.row == selected_row && button.layer == selected_layer;
}

// Draws a rotating sun around the selected UiButton
void draw_rotating_sun(Vector2 anchor_pos) {
    static float angle = 0.0f;
    angle = fmodf(angle + 0.75f, 360.0f);

    const Rectangle rect_1 = {anchor_pos.x, anchor_pos.y, 26.25, 26.25};
    const Rectangle rect_2 = {anchor_pos.x, anchor_pos.y, 22.5, 22.5};

    DrawRectanglePro(rect_1, (Vector2){13.125, 13.125}, angle, BLACK);
    DrawRectanglePro(rect_1, (Vector2){13.125, 13.125}, angle - 45, BLACK);
    DrawRectanglePro(rect_2, (Vector2){11.25, 11.25}, angle, YELLOW);
    DrawRectanglePro(rect_2, (Vector2){11.25, 11.25}, angle - 45, YELLOW);
}

// Returns true if the button is pressed
bool do_button(struct UiButton button, Color color) {
    const bool is_selected = is_button_selected(button);
    const bool is_pressed = IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);

    if (is_selected) {
        color = (Color){235, 245, 255, 255};
        draw_rotating_sun(button.pos);
    }

    DrawRectangleV(button.pos, button.size, color);
    DrawRectangleLinesV(button.pos, button.size, BLACK);
    DrawText(button.text, (int)(button.pos.x + button.size.x / 2 - MeasureText(button.text, 20) / 2), (int)(button.pos.y + button.size.y / 2 - 10), 20, BLACK);

    return is_selected && is_pressed;
}

// Takes a function pointer to call when the action is confirmed
void draw_confirmation_window(void (*callback)()) {
    if (selected_layer == 0) return;

    const Vector2 conf_window_size = {SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT * 0.5f};
    const Vector2 conf_window_pos = {(SCREEN_WIDTH - conf_window_size.x) / 2.0f, (SCREEN_HEIGHT - conf_window_size.y) / 2.0f};
    const Color conf_window_color = {200, 200, 200, 200};

    DrawRectangleV(conf_window_pos, conf_window_size, conf_window_color);

    const Vector2 button_size = {conf_window_size.x * 0.4f, conf_window_size.y * 0.25f};

    struct UiButton yes_button = {.pos = {conf_window_pos.x + 30, conf_window_pos.y + conf_window_size.y * 0.5f}, .size = button_size, .column = 0, .row = 0, .layer = 1, .text = "Yes"};
    struct UiButton no_button = {.pos = {conf_window_pos.x + conf_window_size.x - button_size.x - 30, conf_window_pos.y + conf_window_size.y * 0.5f}, .size = button_size, .column = 1, .row = 0, .layer = 1, .text = "No"};

    DrawText("Are you sure?", (int)(conf_window_pos.x + conf_window_size.x / 2 - MeasureText("Are you sure?", 20) / 2), (int)(conf_window_pos.y + conf_window_size.y * 0.25f - 10), 20, BLACK);

    static bool first_a_release = true; // We ignore the first release of A as it is released on the first frame (since you need to release A to open this menu)

    if (first_a_release && IsGamepadButtonReleased(0, A)) {
        first_a_release = false;
        return;
    }

    if (do_button(yes_button, GRAY)) {
        callback();
        selected_layer = 0;
        first_a_release = true;
        return;
    }

    if (do_button(no_button, GRAY)) {
        selected_layer = 0;
        first_a_release = true;
        return;
    }
}
