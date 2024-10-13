#ifndef BBOX_HELPER_FUNCTIONS_H
#define BBOX_HELPER_FUNCTIONS_H

#include <math.h>
#include <raylib.h>

#define BBOX_MIN(a, b) ((a) < (b) ? (a) : (b))
#define BBOX_MAX(a, b) ((a) > (b) ? (a) : (b))

#define BBOX_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

// NOTE: These are in PascalCase to follow raylib naming conventions

inline void DrawRectangleLinesV(Vector2 position, Vector2 size, Color color) {
    DrawRectangleLines((int)position.x, (int)position.y, (int)size.x, (int)size.y, color);
}

inline void DrawRectangleLinesExV(Vector2 position, Vector2 size, float lineThick, Color color) {
    DrawRectangleLinesEx((Rectangle){ position.x, position.y, size.x, size.y }, lineThick, color);
}

inline void DrawCircleLinesExV(Vector2 center, float radius, float lineThick, Color color) {
    for (int i = 0; i < 360; i += 10) {
        DrawLineEx((Vector2){ center.x + cosf(DEG2RAD * i) * radius, center.y + sinf(DEG2RAD * i) * radius },
                   (Vector2){ center.x + cosf(DEG2RAD * (i + 10)) * radius, center.y + sinf(DEG2RAD * (i + 10)) * radius }, lineThick, color);
    }
}

inline bool CheckCollisionRectangleV(Vector2 vec1, Vector2 pos1, Vector2 vec2, Vector2 pos2) {
    return CheckCollisionRecs((Rectangle){ vec1.x, vec1.y, pos1.x, pos1.y }, (Rectangle){ vec2.x, vec2.y, pos2.x, pos2.y });
}

inline void invert_color(Color *color) {
    color->r = 255 - color->r;
    color->g = 255 - color->g;
    color->b = 255 - color->b;
}

#endif