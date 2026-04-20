#include "Paddle.h"

// 无参构造（Game 里用的是这个）
Paddle::Paddle() {
    speed = 8.0f;
    Reset();
}

// 你原来的带参构造（保留兼容）
Paddle::Paddle(float x, float y, float w, float h, float sp) 
    : rect({x,y,w,h}), speed(sp) {}

void Paddle::Reset() {
    rect = {350, 550, 100, 20};
}

// 去掉参数，和声明一致
void Paddle::MoveLeft() {
    rect.x -= speed;
    if (rect.x < 0) rect.x = 0;
}
void Paddle::MoveRight() {
    rect.x += speed;
    if (rect.x + rect.width > 800) rect.x = 800 - rect.width;
}
void Paddle::MoveWithMouse() {
    rect.x = GetMouseX() - rect.width / 2;
    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.width > 800) rect.x = 800 - rect.width;
}

void Paddle::Draw() {
    DrawRectangleRec(rect, DARKBLUE);
}