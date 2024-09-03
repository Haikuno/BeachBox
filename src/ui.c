#define MAX_COLUMNS 10

uint8_t selected_column = 0;
uint8_t selected_row = 0;
uint8_t selected_layer = 0;  // Used for confirmation windows

uint8_t row_count[MAX_COLUMNS];
uint8_t column_count[MAX_COLUMNS];

struct UiButton {
    Vector2 pos;
    Vector2 size;
    const char* text;
    uint8_t column;
    uint8_t row;
    uint8_t layer;
};

void change_scene(enum Scene scene) {
    current_scene = scene;
    selected_column = 0;
    selected_row = 0;
    selected_layer = 0;

    switch (scene) {
        case GAME:
            init_game_scene();
            break;
        default:
            break;
    }
}

void move_cursor(char direction) {
    switch (direction) {
        case 'L':
            if (selected_column > 0) {
                selected_column--;
                selected_row = row_count[selected_column] - 1 >= selected_row ? selected_row : row_count[selected_column] - 1;
            }
            break;

        case 'R':
            if (selected_column < column_count[selected_row] - 1) {
                selected_column++;
                selected_row = row_count[selected_column] - 1 >= selected_row ? selected_row : row_count[selected_column] - 1;
            }
            break;

        case 'U':
            if (selected_row > 0) selected_row--;
            break;

        case 'D':
            if (selected_row < row_count[selected_column] - 1) selected_row++;  // -1 because we start at 0
            break;
    }

    if (selected_row == 255) selected_row = 0;
    if (selected_column == 255) selected_column = 0;
}

bool is_button_selected(struct UiButton button) {
    return button.column == selected_column && button.row == selected_row && button.layer == selected_layer;
}

// returns true if the button is pressed
bool do_button(struct UiButton button) {
    bool result = false;

    // TODO: make this look pretty

    Color color = DARKGRAY;
    if (is_button_selected(button)) {
        color = LIGHTGRAY;
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            color = RAYWHITE;
            result = true;
        }
    }

    DrawRectangleV(button.pos, button.size, color);
    DrawRectangleLinesV(button.pos, button.size, BLACK);
    DrawText(button.text, (int)(button.pos.x + button.size.x / 2 - MeasureText(button.text, 20) / 2), (int)(button.pos.y + button.size.y / 2 - 10), 20, BLACK);

    return result;
}