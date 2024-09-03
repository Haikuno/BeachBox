void DrawRectangleLinesV(Vector2 position, Vector2 size, Color color) {
    DrawRectangleLines((int)position.x, (int)position.y, (int)size.x, (int)size.y, color);
}

bool CheckCollisionRectangleV(Vector2 vec1, Vector2 pos1, Vector2 vec2, Vector2 pos2) {
    return CheckCollisionRecs((Rectangle){vec1.x, vec1.y, pos1.x, pos1.y}, (Rectangle){vec2.x, vec2.y, pos2.x, pos2.y});
}