#ifndef PADDLE_H
#define PADDLE_H

#include "raylib.h"

class Paddle {
private:
    Rectangle rect;
    float speed;
    bool isDragging;
    Vector2 dragOffset;

public:
    Paddle(float x, float y, float w, float h, float sp);
    void Draw();
    void MoveLeft();
    void MoveRight(int screenWidth);
    void UpdateDrag(int screenWidth); // 关键：声明拖拽方法
    Rectangle GetRect() { return rect; }
};

#endif