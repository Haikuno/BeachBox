// TODO: implement audio
#define MAX_VOLUME 200
#define MIN_VOLUME 0

int sfxVolume = 120;
int musicVolume = 120;




void update_song(void) {
    // TODO: implement songs
    switch (current_scene) {
        case RAYLOGO:
            //
            break;
        case LOADING:
            //
            break;


        case SHOP:
            //
            break;
        case UNLOCKABLES:
            //
            break;
        case OPTIONS:
            //
            break;
        case CREDITS:
            //
            break;
        case MAINMENU:
            //
            
        case GAME:
            if (adx_dec("/rd/audio/gamescene.adx", 1) < 1)
            {
                printf("Invalid ADX file\n");
                
            }
            //
        break;


    
    }
    thd_pass(); 
    
}

// TODO: implement sfx
void play_sfx_coin(void) {
    //
}

void play_sfx_slowdown(void) {
    //
}

void play_sfx_game_over(void) {
    //
}

void play_sfx_jump(void) {
    //
}

void play_sfx_teleport(void) {
    //
}

void play_sfx_menu_move(void) {
    //
}

void play_sfx_menu_select(void) {
    //
}

void play_sfx_menu_exit(void) {
    //
}

void play_sfx_menu_inactive_button(void) {
    //
}

void play_sfx_shop_purchase(void) {
    //
}