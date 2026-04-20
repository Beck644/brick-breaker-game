#pragma once
#include "raylib.h"

class Paddle {
private:
    Rectangle rect;
    float speed;

public:
    Paddle();  // 无参构造（Game 里用的是这个）
    Paddle(float x, float y, float w, float h, float sp);  // 你原来的带参构造
    void Reset();
    void MoveLeft();       // 去掉参数，和你声明一致
    void MoveRight();      // 去掉参数，和你声明一致
    void MoveWithMouse();  // 去掉参数，和你声明一致
    void Draw();
    Rectangle GetRect() const { return rect; }
};