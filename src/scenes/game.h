#ifndef BBOX_GAME_H
#define BBOX_GAME_H

void init_game_scene(void);
void update_game_scene(void);
void draw_game_scene(void);
void lose_game(void);

float get_current_object_speed(void);
void set_current_object_speed(float newSpeed);

bool  get_is_slowing_down(void);
void set_is_slowing_down(bool newBool);

bool get_held_a_during_death(void);
void set_held_a_during_death(bool newBool);

bool get_is_teleporting(void);
void set_is_teleporting(bool newBool);

#endif