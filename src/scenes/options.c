#include <raylib.h>
#include <stddef.h>
#include "../scene.h"
#include "../ui.h"
#include "../config.h"
#include "../background.h"
#include "../save.h"
#include "../audio.h"
#include <adx/snddrv.h>
#include <adx/adx.h>
#include "options.h"
#include <string.h>

extern uint8_t     column_count[];
extern uint8_t     row_count[];
extern const Color ui_background_color;
extern uint8_t     selected_layer;
extern uint8_t     selected_column;
extern uint8_t     selected_row;

// TODO: finish options scene

const uibutton_t music_volume_options_button = {
    .pos    = { .x = 40,  .y = 60 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 0,
    .layer  = 0,
    .text   = "Music volume",
};

const uibutton_t sfx_volume_options_button = {
    .pos    = { .x = 40,  .y = 180 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 1,
    .layer  = 0,
    .text   = "SFX Volume",
};

const uibutton_t newsave_options_button = {
    .pos    = { .x = 40,  .y = 300 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 2,
    .layer  = 0,
    .text   = "New Save",
};

const uibutton_t exit_options_button = {
    .pos    = { .x = 40,  .y = 420 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 3,
    .layer  = 0,
    .text   = "Return",
};



const void init_options_scene(void) {
    row_count[0]    = 4;
    column_count[0] = 1;
    selected_row    = 0;
}

const void update_options_scene(void) {
    //
}

static void new_game_callback(int option, void *user_data) {
    if (option == 1) { // yes pressed
        new_game();
        change_scene(MAINMENU);
        return;
    }
    if (option == 0) { // no pressed
        init_options_scene();
        return;
    }
}
static void DrawRectangleBars(int count, int posX, int posY) {
    const int BAR_WIDTH = 10;
    const int SPACING = 15;
    const int BASE_HEIGHT = 40;

    for (int i = 0; i < 25; i++) {

        DrawRectangle(
            posX + i * SPACING,
            posY,
            BAR_WIDTH,
            BASE_HEIGHT,
            SKYBLUE         
        );
    }
    for (int i = 0; i < count; i++) {

        DrawRectangle(
            posX + i * SPACING,
            posY,
            BAR_WIDTH,
            BASE_HEIGHT,
            BLUE  
        );
    }
}

void draw_options_scene(void) {
    draw_background();

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ui_background_color);


    static void (*callback)(int option, void *user_data) = NULL;

    



    // For volume
    const Vector2 arrows_size_ = (Vector2){ .x = 24, .y = 24 };
    const uiarrows_t music_volume_arrows_ = {
        .column = 0,
        .row = 0,
        .layer = 0,
        .pos_left = {18, 65},
        .pos_right = {187, 65},
        .size = arrows_size_,
    };

    const uiarrows_t sfx_volume_arrows_ = {
        .column = 0,
        .row = 1,
        .layer = 0,
        .pos_left = {18, 185},
        .pos_right = {187, 185},
        .size = arrows_size_,
    };
    static char dialogue_text_[24] = "";


    const int music_arrows_return_code = do_arrows(music_volume_arrows_);
    if (are_arrows_selected(music_volume_arrows_) && music_arrows_return_code != 0) {
        if(music_arrows_return_code == 1){
            snddrv_volume_up();
        } else if(music_arrows_return_code == -1){

            snddrv_volume_down();
        }
        if(BBgetMusicVolume() <= 0){
            adx_pause();
        }

        if(BBgetMusicVolume() >= 1){
            
                adx_resume();
        }

        
        BBsetMusicVolume(music_arrows_return_code);
        if (BBgetMusicVolume() > 24){ BBsetMusicVolume(-1);}; 
        if (BBgetMusicVolume() < 0){ BBsetMusicVolume(1);};
    }

    const int sfx_arrows_return_code = do_arrows(sfx_volume_arrows_);
    if (are_arrows_selected(sfx_volume_arrows_) && sfx_arrows_return_code != 0) {
        BBsetSfxVolume(sfx_arrows_return_code);
        if (BBgetSfxVolume() > 24){ BBsetSfxVolume(-1);}; 
        if (BBgetSfxVolume() < 0){ BBsetSfxVolume(1);};
    }

    if (do_button(music_volume_options_button, true)) {
        callback        = NULL;
        selected_layer  = 0;
        selected_column = 0;
        selected_row    = 0;
        strcpy(dialogue_text_, "Change music volume?");
    }

    if (do_button(sfx_volume_options_button, true)) {
        callback        = NULL;
        selected_layer  = 0;
        selected_column = 0;
        selected_row    = 1;
        strcpy(dialogue_text_, "Change sfx volume?");
    }

    if (do_button(newsave_options_button, true)) {
        callback        = new_game_callback;
        selected_layer  = 1;
        selected_column = 0;
        selected_row    = 0;
        strcpy(dialogue_text_, "Start a new game?");
        
    }

    if (do_button(exit_options_button, true)) {
        change_scene(MAINMENU);
    }

    DrawRectangleBars(BBgetMusicVolume() + 1, 230, 60 );
    DrawRectangleBars(BBgetSfxVolume(), 230, 180 );
    

    draw_confirmation_window(callback, NULL, dialogue_text_);
}