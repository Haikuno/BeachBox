#define MAX_COLUMNS 10
#define MAX_ROWS 10

uint8_t selected_column = 0;
uint8_t selected_row = 0;
uint8_t selected_layer = 0;  // Used for confirmation windows

uint8_t row_count[MAX_COLUMNS] = {1};
uint8_t column_count[MAX_ROWS] = {1};

struct UiButton {
    Vector2 pos;
    Vector2 size;
    const char* text;
    uint8_t column;
    uint8_t row;
    uint8_t layer;
};

// Forward declarations
void init_game_scene();

inline void change_scene(enum Scene scene) {
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

inline bool is_button_selected(struct UiButton button) {
    return button.column == selected_column && button.row == selected_row && button.layer == selected_layer;
}

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

// returns true if the button is pressed
bool do_button(struct UiButton button, Color color) {
    if (selected_layer != button.layer) return false;

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