#ifndef BBOX_AUDIO_H
#define BBOX_AUDIO_H


// Loads audio
void init_sounds(void);

// Changes songs when you switch scenes
void update_song(void);


// Plays sound effects
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

// Gets the music volume
int BB_get_music_volume(void);
// Sets the music volume
void BB_set_music_volume(int new_value);
// Gets sfx volume
int BB_get_sfx_volume(void);
// Sets sfx volume
void BB_set_sfx_volume(int new_value);

#endif