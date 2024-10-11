#include <raylib.h>
#include "../scene.h"
#include "../ui.h"
#include "../config.h"
#include "../background.h"
#include "credits.h"

extern uint8_t     column_count[];
extern uint8_t     row_count[];
extern const Color ui_background_color;

uibutton_t exit_credits_button = {
    .pos    = { .x = 43,  .y = 370 },
    .size   = { .x = 150, .y = 40  },
    .column = 0,
    .row    = 0,
    .layer  = 0,
    .text   = "Return",
};

struct BouncingImage {
        Vector2   position;
        Vector2   speed;
        Texture2D texture;
} images[3];

void initialize_images(void) {
    const char *paths[]
        = { "/rd/creditspngs/koslogo.png", "/rd/creditspngs/rayliblogo.png", "/rd/creditspngs/psyopslogo.png" };
    Vector2 positions[] = {
        { 50,  50  },
        { 490, 250 },
        { 300, 350 }
    };

    for (int i = 0; i < 3; i++) {
        images[i].speed    = (Vector2){ 4, 3 };
        images[i].texture  = LoadTexture(paths[i]);
        images[i].position = positions[i];
    }
}

void unload_credits_images(void) {
    for (int i = 0; i < 3; i++) {
        UnloadTexture(images[i].texture);
    }
}

void init_credits_scene(void) {
    row_count[0]    = 1;
    column_count[0] = 1;
    initialize_images();
}

void update_credits_scene(void) {
    for (int i = 0; i < 3; i++) {
        images[i].position.x += images[i].speed.x;
        images[i].position.y += images[i].speed.y;

        if (images[i].position.x <= 0 || images[i].position.x + images[i].texture.width >= SCREEN_WIDTH)
            images[i].speed.x *= -1;
        if (images[i].position.y <= 0 || images[i].position.y + images[i].texture.height >= SCREEN_HEIGHT)
            images[i].speed.y *= -1;
    }
}

void draw_credits_scene(void) {
    draw_background();

    for (int i = 0; i < 3; i++) {
        DrawTexture(images[i].texture, (int)images[i].position.x, (int)images[i].position.y, WHITE);
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ui_background_color);

    // TODO: add actual credits
    const char *credits_text  = "Made by Psyops :)";
    const char *credits_text2 = "Thanks to KallistiOS and Raylib";
    DrawText(credits_text, (int)(SCREEN_WIDTH / 4) + 35, (int)(SCREEN_HEIGHT / 4) + 20, 30, RAYWHITE);
    DrawText(credits_text2, (int)(SCREEN_WIDTH / 4) - 80, (int)(SCREEN_HEIGHT / 4) + 65, 30, RAYWHITE);

    if (do_button(exit_credits_button, true)) {
        unload_credits_images();
        change_scene(MAINMENU);
    }
}