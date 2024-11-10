#ifndef BBOX_AUDIO_H
#define BBOX_AUDIO_H

void init_sounds(void);
void update_song(void);

void play_sfx_menu_move(void);
void play_sfx_menu_select(void);
void play_sfx_menu_error(void);
void play_sfx_coin(void);
void play_sfx_slowdown(void);
void play_sfx_slowdown_back(void);
void play_sfx_game_over(void);
void play_sfx_teleport(void);
void play_sfx_menu_move(void);
void play_sfx_menu_select(void);
void play_sfx_menu_error(void);
void play_sfx_purchase(void);


int getMusicVolumeBB(void);
void setMusicVolumeBB(int new_value);
int getSfxVolumeBB(void);
void setSfxVolumeBB(int new_value);

#endif