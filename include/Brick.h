#pragma once
#include "raylib.h"

class Brick {
private:
    Rectangle rect;
    bool active;
    Color color;

public:
    Brick();  // 无参构造
    // 你原来的 5 参数构造（x,y,w,h,color）
    Brick(float x, float y, float w, float h, Color c);
    void Draw();
    bool IsActive() const { return active; }
    void SetActive(bool a) { active = a; }
    Rectangle GetRect() const { return rect; }
};