// TODO: implement audio
#include <stdio.h>
#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>
#include <dc/sound/stream.h>
#include <adx/adx.h>
#include <adx/snddrv.h>
#include "audio.h"
#include "scene.h"

int sfxVolume = 120;
int musicVolume = 120;


#define MAX_VOLUME 200
#define MIN_VOLUME 0



sfxhnd_t sfx_coin;
sfxhnd_t sfx_slowdown;
sfxhnd_t sfx_teleport;
sfxhnd_t sfx_gameover;

extern void init_sounds(void);
extern void update_song(void);
extern void play_sfx_coin(void);
extern void play_sfx_slowdown(void);
extern void play_sfx_game_over(void);
extern void play_sfx_jump(void);
extern void play_sfx_teleport(void);
extern void play_sfx_menu_move(void);
extern void play_sfx_menu_select(void);
extern void play_sfx_menu_exit(void);
extern void play_sfx_menu_inactive_button(void);
extern void play_sfx_shop_purchase(void);

struct SceneType;


void init_sounds() {
    

    snd_init();
    snd_stream_init();
    sfx_coin = snd_sfx_load("rd/audio/coin-adpcm.wav");


    sfx_teleport = snd_sfx_load("rd/audio/teleport-adpcm.wav");
    sfx_gameover = snd_sfx_load("rd/audio/gameover-adpcm.wav");
    sfx_slowdown = snd_sfx_load("rd/audio/slowdown-adpcm.wav");

}





int newSong = 0;
int currentSong = 0;


void update_song() {
    switch (current_scene) {
        case RAYLOGO:
            newSong = 2;  
            break;
        case LOADING:
            newSong = 2;  
            break;
        case SHOP:
            newSong = 2;
            break;
        case UNLOCKABLES:
            newSong = 2;
            break;
        case OPTIONS:
            newSong = 2;
            break;
        case CREDITS:
            newSong = 2; 
            break;
        case MAINMENU:
        case GAME:
            newSong = 1;  
            break;
    }

    if (newSong != currentSong) {
        currentSong = newSong;
        
        switch (currentSong) {
            case 1:
                adx_stop();
                if (adx_dec("/rd/audio/gamescene.adx", 1) < 1) {
                    printf("Invalid game ADX file\n");
                }
                break;
            case 2:
                adx_stop();
                if (adx_dec("/rd/audio/credit.adx", 1) < 1) {
                    printf("Invalid credit ADX file\n");
                }
                break;
            default:
                break;
        }
    }

    thd_pass();
}

void play_sfx_coin(void) {
    snd_sfx_play_chn(2, sfx_coin, sfxVolume, 128);
}

void play_sfx_slowdown(void) {
    snd_sfx_play_chn(4, sfx_slowdown, sfxVolume, 128);
}

void play_sfx_game_over(void) {
    snd_sfx_play_chn(4, sfx_gameover, sfxVolume, 128);
}

void play_sfx_jump(void) {
    //snd_sfx_play_chn(4, beep1, sfxVolume, 128);
}

void play_sfx_teleport(void) {
    snd_sfx_play_chn(4, sfx_teleport, sfxVolume, 128);
}

void play_sfx_menu_move(void) {
    //snd_sfx_play_chn(4, beep1, sfxVolume, 128);
}

void play_sfx_menu_select(void) {
    //snd_sfx_play_chn(4, beep1, sfxVolume, 128);
}

void play_sfx_menu_exit(void) {
    //snd_sfx_play_chn(4, beep1, sfxVolume, 128);
}

void play_sfx_menu_inactive_button(void) {
    //snd_sfx_play_chn(4, beep1, sfxVolume, 128);
}

void play_sfx_shop_purchase(void) {
    snd_sfx_play_chn(2, sfx_coin, sfxVolume, 128);
}