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


int BBgetMusicVolume(void);
void BBsetMusicVolume(int new_value);
int BBgetSfxVolume(void);
void BBsetSfxVolume(int new_value);

#endif