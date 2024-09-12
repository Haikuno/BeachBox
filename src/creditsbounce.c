typedef struct {
    Vector2 position;
    Vector2 speed;
    Texture2D texture;
    bool initialized;
} BouncingImage;

BouncingImage images[3];

void initialize_images(void) {
    const char* paths[] = {"/rd/creditspngs/kosapple.png", "/rd/creditspngs/rayliblogo.png", "/rd/creditspngs/psyopslogo.png"};
    Vector2 positions[] = {{50, 50}, {490, 250}, {300, 350}};

    for (int i = 0; i < 3; i++) {
        images[i].initialized = true;
        images[i].speed = (Vector2){4, 3};
        images[i].texture = LoadTexture(paths[i]);
        images[i].position = positions[i];
    }
}
bool creditsInit = false;
void update_credits_images(void) {
    

    if (!creditsInit) {
        initialize_images();
        creditsInit = true;
    }

    for (int i = 0; i < 3; i++) {
        images[i].position.x += images[i].speed.x;
        images[i].position.y += images[i].speed.y;

        if (images[i].position.x <= 0 || images[i].position.x + images[i].texture.width >= SCREEN_WIDTH)
            images[i].speed.x *= -1;
        if (images[i].position.y <= 0 || images[i].position.y + images[i].texture.height >= SCREEN_HEIGHT)
            images[i].speed.y *= -1;
            
    }
}

void draw_credits_images(void) {
    for (int i = 0; i < 3; i++) {
        DrawTexture(images[i].texture, (int)images[i].position.x, (int)images[i].position.y, WHITE);
    }
}

void unload_credits_images(void) {
    for (int i = 0; i < 3; i++) {
            creditsInit = 0;
            UnloadTexture(images[i].texture);
            images[i].initialized = false;
    }
}