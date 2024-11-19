#ifndef BBOX_GAME_H
#define BBOX_GAME_H

// Initializes the game scene
void init_game_scene(void);

// Updates the game scene
void update_game_scene(void);

// Draws the game scene
void draw_game_scene(void);

// Ends the current run
void end_game(void);

// Returns true if the game is over
const bool is_game_over(void);

// Returns true if the game is paused
const bool is_game_paused(void);

// Toggles the game pause
void toggle_pause(void);

// Returns true if a new high score was reached
const bool is_new_high_score(void);

// Returns the coins earned in the current run
const uint16_t get_current_coins(void);

// Adds a coin to the current run's score
void add_coin(void);

#endif