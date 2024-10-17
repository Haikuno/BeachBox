#ifndef BBOX_SCENE_H
#define BBOX_SCENE_H

typedef enum Scene { RAYLOGO, LOADING, MAINMENU, GAME, SHOP, UNLOCKABLES, OPTIONS, CREDITS } scene_t;

void change_scene(scene_t scene);
void update_current_scene(void);
void draw_current_scene(void);
extern scene_t current_scene;
#endif