#include "Paddle.h"

Paddle::Paddle(float x, float y, float w, float h, float sp) {
    rect = { x, y, w, h };
    speed = sp;
    isDragging = false;
    dragOffset = { 0, 0 };
}

void Paddle::Draw() {
    DrawRectangleRec(rect, BLUE);
    if (isDragging) {
        DrawRectangleLinesEx(rect, 2, YELLOW);
    }
}

void Paddle::MoveLeft() {
    rect.x -= speed;
    if (rect.x < 0) rect.x = 0;
}

void Paddle::MoveRight(int screenWidth) {
    rect.x += speed;
    if (rect.x + rect.width > screenWidth) {
        rect.x = screenWidth - rect.width;
    }
}

// 关键：实现拖拽逻辑
void Paddle::UpdateDrag(int screenWidth) {
    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, rect)) {
        isDragging = true;
        dragOffset.x = mousePos.x - rect.x;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging = false;
    }

    if (isDragging) {
        rect.x = mousePos.x - dragOffset.x;
        if (rect.x < 0) rect.x = 0;
        if (rect.x + rect.width > screenWidth) rect.x = screenWidth - rect.width;
    }
}