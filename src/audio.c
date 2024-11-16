// TODO: implement audio
#include "audio.h"
#include "scene.h"
#include <adx/adx.h>
#include <adx/snddrv.h>
#include <dc/sound/sfxmgr.h>
#include <dc/sound/sound.h>
#include <dc/sound/stream.h>
#include <stdio.h>

// TODO: make volume changeable

#define MAX_VOLUME 200
#define MIN_VOLUME 0


static int sfxVolume   = 120;
static int musicVolume = 12; // roughly 50%

int BBgetMusicVolume(void) {
    return musicVolume;
}

void BBsetMusicVolume(int new_value) {
    musicVolume += new_value;
}
int BBgetSfxVolume(void) {
    return (sfxVolume / 10);
}
void BBsetSfxVolume(int new_value) {
    sfxVolume += (new_value * 10);
}

static sfxhnd_t sfx_menu_move;
static sfxhnd_t sfx_menu_select;
static sfxhnd_t sfx_menu_error;
static sfxhnd_t sfx_coin;
static sfxhnd_t sfx_slowdown;
static sfxhnd_t sfx_slowdown_back;
static sfxhnd_t sfx_teleport;
static sfxhnd_t sfx_gameover;

struct SceneType;

void init_sounds(void) {
    snd_stream_init();

    sfx_menu_move     = snd_sfx_load("rd/audio/menu_move.wav");
    sfx_menu_select   = snd_sfx_load("rd/audio/menu_select.wav");
    sfx_menu_error    = snd_sfx_load("rd/audio/menu_error.wav");
    sfx_coin          = snd_sfx_load("rd/audio/coin.wav");
    sfx_teleport      = snd_sfx_load("rd/audio/teleport.wav");
    sfx_gameover      = snd_sfx_load("rd/audio/gameover.wav");
    sfx_slowdown      = snd_sfx_load("rd/audio/slowdown.wav");
    sfx_slowdown_back = snd_sfx_load("rd/audio/slowdown_back.wav");
}

void play_sfx_menu_move(void) {
    snd_sfx_play_chn(4, sfx_menu_move, sfxVolume, 128);
}

void play_sfx_menu_select(void) {
    snd_sfx_play_chn(4, sfx_menu_select, sfxVolume, 128);
}

void play_sfx_menu_error(void) {
    snd_sfx_play_chn(4, sfx_menu_error, sfxVolume, 128);
}

void play_sfx_coin(void) {
    snd_sfx_play_chn(2, sfx_coin, sfxVolume, 128);
}

void play_sfx_slowdown(void) {
    snd_sfx_play_chn(4, sfx_slowdown, sfxVolume, 128);
}

void play_sfx_slowdown_back(void) {
    snd_sfx_play_chn(4, sfx_slowdown_back, sfxVolume, 128);
}

void play_sfx_game_over(void) {
    snd_sfx_play_chn(4, sfx_gameover, sfxVolume, 128);
}

void play_sfx_teleport(void) {
    snd_sfx_play_chn(4, sfx_teleport, sfxVolume, 128);
}

void play_sfx_purchase(void) {
    snd_sfx_play_chn(2, sfx_coin, sfxVolume, 128);
}

static enum Song { NIL_SONG, MENU_SONG, GAME_SONG, CREDITS_SONG } current_song = NIL_SONG;

static void play_song(void) {
    switch (get_current_song) {
        case NIL_SONG:
            break;
        case MENU_SONG:
            adx_stop();
            if (adx_dec("/rd/audio/menu_song.adx", 1) < 1) {
                printf("Invalid menu ADX file\n");
            }
            break;
        case GAME_SONG:
            adx_stop();
            if (adx_dec("/rd/audio/gamescene.adx", 1) < 1) {
                printf("Invalid game ADX file\n");
            }
            break;
        case CREDITS_SONG:
            adx_stop();
            if (adx_dec("/rd/audio/credits.adx", 1) < 1) {
                printf("Invalid credit ADX file\n");
            }
            break;
        default:
            break;
    }
}

void update_song(void) {
    const enum Song prev_song = current_song;

    switch (current_scene) {
        case RAYLOGO:
            current_song = NIL_SONG;
            break;
        case LOADING:
        case MAINMENU:
        case SHOP:
        case UNLOCKABLES:
        case OPTIONS:
            current_song = MENU_SONG;
            break;
        case GAME:
            current_song = GAME_SONG;
            break;
        case CREDITS:
            current_song = CREDITS_SONG;
            break;
    }

    if (prev_song != current_song) {
        play_song();

        // Wait for the song to start
        while (snddrv.drv_status != SNDDRV_STATUS_STREAMING) {
            thd_sleep(1);
        }

        if (snddrv.drv_status != SNDDRV_STATUS_NULL) {
            // First, set the volume to 0
            for (int i = 0; i < 25; i++) {
                snddrv_volume_down();
            }

            if (!musicVolume == 0) {
                // Then, set the volume to musicVolume
                for (int i = 0; i < musicVolume; i++) {
                    snddrv_volume_up();
                }
            }
        }
    }
}