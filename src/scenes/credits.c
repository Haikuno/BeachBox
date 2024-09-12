// TODO: finish credits scene
#include "../creditsbounce.c"
struct UiButton exit_credits_button = {
    .pos = {.x = 43, .y = 370},
    .size = {.x = 150, .y = 40},
    .column = 0,
    .row = 0,
    .layer = 0,
    .text = "Return",
};


void update_credits_scene()
{

    update_credits_images();

    for (uint8_t i = 0; i < MAX_COLUMNS; i++)
    {
        column_count[i] = 0;
        row_count[i] = 0;
    }

    row_count[0] = 1;
    column_count[0] = 1;
}

void draw_credits_scene()
{
    draw_background();
    draw_credits_images();


    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ui_background_color);

    // TODO: add actual credits
    const char *credits_text = "Made by Psyops :)";
    const char *credits_text2 = "Thanks to KallistiOS and Raylib";
    DrawText(credits_text, (int)(SCREEN_WIDTH / 4) + 35, (int)(SCREEN_HEIGHT / 4) + 20, 30, RAYWHITE);
    DrawText(credits_text2, (int)(SCREEN_WIDTH / 4) - 80, (int)(SCREEN_HEIGHT / 4) + 65, 30, RAYWHITE);

    if (do_button(exit_credits_button, true))
    {
        unload_credits_images();
        change_scene(MAINMENU);
    }
}