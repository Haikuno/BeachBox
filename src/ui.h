#ifndef BBOX_UI_H
#define BBOX_UI_H

#include <raylib.h>
#include <stdint.h>

#define MAX_COLUMNS 10
#define MAX_ROWS 10

typedef struct UIButton {
        const Vector2 pos;
        const Vector2 size;
        const uint8_t column;
        const uint8_t row;
        const uint8_t layer;
        const char   *text;
} uibutton_t;

typedef struct UIArrows {
        const Vector2 size;
        const Vector2 pos_left;
        const Vector2 pos_right;
        const uint8_t column;
        const uint8_t row;
} uiarrows_t;

// Moves the "cursor" when the player presses the D-Pad, changing the selected UI element
// Direction is one of 'L', 'R', 'U', 'D' for left, right, up, down
void move_cursor(char direction);

// Draws a rotating sun around the selected UIButton
void draw_rotating_sun(Vector2 anchor_pos);

// Draws an UIButton. Returns true if the button is pressed
bool do_button(uibutton_t button, bool is_active);

// Draws UIArrows. Returns 1 if right is pressed, -1 if left is pressed, 0 otherwise
int do_arrows(uiarrows_t arrows);

// Takes a function pointer to call when the action is confirmed, as well as the params to pass to that function
void draw_confirmation_window(void (*callback)(void *user_data), void *user_data);

bool are_arrows_selected(uiarrows_t arrows);

#endif