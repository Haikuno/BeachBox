typedef struct
{
    Vector2 position;
    Vector2 speed;
    Texture2D texture;
    bool initialized;
} BouncingImage;

BouncingImage logobounce;
BouncingImage raylibbounce;
BouncingImage kosbounce;

void update_credits_images(){

        static bool pngCreditsInit = 0;

        if (pngCreditsInit == 0)
        {
            kosbounce.initialized = 1;
            kosbounce.speed = (Vector2){5,5};
            kosbounce.texture = LoadTexture("/rd/creditspngs/kosapple.png");
            kosbounce.position = (Vector2){50, 50};

            raylibbounce.initialized = 1;
            raylibbounce.speed = (Vector2){5,5};
            raylibbounce.texture = LoadTexture("/rd/creditspngs/rayliblogo.png");
            raylibbounce.position = (Vector2){50, 250};

            logobounce.initialized = 1;
            logobounce.speed = (Vector2){5,5};
            logobounce.texture = LoadTexture("/rd/creditspngs/rayliblogo.png");
            logobounce.position = (Vector2){300, 250};
            pngCreditsInit = 1;
        }
        
}

void draw_credits_images(){
    DrawTexture(raylibbounce.texture, raylibbounce.position.x, raylibbounce.position.y, WHITE);
    DrawTexture(kosbounce.texture, kosbounce.position.x, kosbounce.position.y, WHITE);
    DrawTexture(logobounce.texture, logobounce.position.x, logobounce.position.y, WHITE);
}