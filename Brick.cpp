#include "Brick.h"

Brick::Brick(float x, float y, float w, float h, Color c) {
    rect = { x, y, w, h };
    active = true;
    color = c;
    isDragging = false;
    dragStartOffset = { 0, 0 };
}

void Brick::Update() {
    if (!active) return;

    Vector2 mousePos = GetMousePosition();
    // 优先检测鼠标拖拽（不受任何逻辑干扰）
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, rect)) {
            isDragging = true;
            dragStartOffset.x = mousePos.x - rect.x;
            dragStartOffset.y = mousePos.y - rect.y;
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging = false;
    }

    if (isDragging) {
        rect.x = mousePos.x - dragStartOffset.x;
        rect.y = mousePos.y - dragStartOffset.y;
        // 边界限制（手动判断，兼容所有Raylib版本）
        if (rect.x < 0) rect.x = 0;
        if (rect.y < 0) rect.y = 0;
        if (rect.x + rect.width > GetScreenWidth()) rect.x = GetScreenWidth() - rect.width;
        if (rect.y + rect.height > GetScreenHeight()) rect.y = GetScreenHeight() - rect.height;
    }
}

void Brick::Draw() {
    if (!active) return;
    
    if (isDragging) DrawRectangleRec(rect, Fade(YELLOW, 0.8f));
    else DrawRectangleRec(rect, color);
    
    DrawRectangleLinesEx(rect, 2, BLACK);
}

bool Brick::CheckCollision(Ball ball) {
    if (!active) return false;
    return CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), rect);
}