#ifndef BBOX_CONTROLLER_H
#define BBOX_CONTROLLER_H

typedef enum Buttons { DPAD_UP = 1, DPAD_RIGHT, DPAD_DOWN, DPAD_LEFT, Y, B, A, X, START = 15 } button_t;

typedef enum Triggers { LEFT_TRIGGER = 4, RIGHT_TRIGGER } trigger_t;

void update_controller(void);

#endif