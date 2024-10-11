#ifndef BBOX_PLAYER_H
#define BBOX_PLAYER_


#include <raylib.h>

#define PLAYER_COLORS_COUNT 17

typedef struct Character {
        Vector2 size;
        Vector2 pos;
        Vector2 velocity;
        float   speed;
        float   max_meter;
        float   meter;
        bool    is_shifted;
        Color   color;
} character_t;

void init_player(void);
void update_player(void);
void draw_player(void);

void move_player(Vector2 direction);
void shift_player(bool should_shift);

void jump(void);
// This function gets called when the player lets go of the jump button, cutting the jump short
void cut_jump(void);

void slow_down(void);
void teleport(void);

#endif