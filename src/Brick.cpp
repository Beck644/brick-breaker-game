#include "Brick.h"

// 无参构造
Brick::Brick() : rect({0,0,0,0}), active(false), color(GOLD) {}

// 你原来的 5 参数构造（补全 Color 参数）
Brick::Brick(float x, float y, float w, float h, Color c) 
    : rect({x,y,w,h}), active(true), color(c) {}

void Brick::Draw() {
    if (active) DrawRectangleRec(rect, color);
}