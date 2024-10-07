// TODO: implement audio
#define MAX_VOLUME 200
#define MIN_VOLUME 0

int sfxVolume = 120;
int musicVolume = 120;




sfxhnd_t sfx_coin;
sfxhnd_t sfx_slowdown;
sfxhnd_t sfx_teleport;
sfxhnd_t sfx_gameover;


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
// atlas - 1 = game / 2 = credits

void update_song(void) {
    switch (current_scene) {
        case RAYLOGO:
            newSong = 2;  // Assuming 0 means no song
            break;
        case LOADING:
            newSong = 2;  // Assign appropriate song numbers
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
            newSong = 2;  // Using the credits song (atlas = 2)
            break;
        case MAINMENU:
        case GAME:
            newSong = 1;  // Using the game song (atlas = 1)
            break;
    }

    // Only load the new song if it's different from the current one
    if (newSong != currentSong) {
        currentSong = newSong;
        
        switch (currentSong) {
            case 1:
                if (adx_dec("/rd/audio/gamescene.adx", 1) < 1) {
                    printf("Invalid game ADX file\n");
                }
                break;
            case 2:
                if (adx_dec("/rd/audio/credit.adx", 1) < 1) {
                    printf("Invalid credit ADX file\n");
                }
                break;
            default:
                // Handle default case, maybe stop music or load a default track
                break;
        }
    }

    thd_pass();
}

// TODO: implement sfx
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
    //snd_sfx_play_chn(4, beep1, sfxVolume, 128);
}