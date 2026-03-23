#ifndef BRICK_H
#define BRICK_H

#include "raylib.h"
#include "Ball.h"

class Brick {
private:
    Rectangle rect;
    bool active;
    Color color;
    bool isDragging;
    Vector2 dragStartOffset;

public:
    Brick(float x, float y, float w, float h, Color c);
    void Update();
    void Draw();
    bool CheckCollision(Ball ball);
    void SetActive(bool a) { active = a; }
    bool IsActive() const { return active; } // 这里必须加 const
    Rectangle GetRect() { return rect; }
};

#endif