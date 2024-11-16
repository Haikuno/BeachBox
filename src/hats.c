#include "hats.h"

static Texture2D hats[HAT_COUNT];

void load_hats(void) {
    hats[HAT_SLIME_RED]  = LoadTexture("rd/hats/slime_red.png");
    hats[HAT_SLIME_BLUE] = LoadTexture("rd/hats/slime_blue.png");
    hats[HAT_BOX]        = LoadTexture("rd/hats/box.png");
    hats[HAT_M]          = LoadTexture("rd/hats/m.png");
    hats[HAT_L]          = LoadTexture("rd/hats/l.png");
    hats[HAT_Z]          = LoadTexture("rd/hats/z.png");
    hats[HAT_F]          = LoadTexture("rd/hats/f.png");
    hats[HAT_MUPRH]      = LoadTexture("rd/hats/murph.png");
    hats[HAT_CROWN]      = LoadTexture("rd/hats/crown.png");
}

void unload_hats(void) {
    for (int i = 0; i < HAT_COUNT; i++) {
        UnloadTexture(hats[i]);
    }
}

const Texture2D *get_hat_texture(const hat_t hat) {
    return &hats[hat];
}