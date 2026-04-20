#pragma once
#include "raylib.h"

class Ball {
private:
    Vector2 position;
    float speedX;
    float speedY;
    float radius;
    bool invincible = false;

public:
    Ball();  // 无参构造
    Ball(Vector2 pos, Vector2 sp, float r);  // 你原来的带参构造
    void Reset();
    void Update();  // 对应你原来的 Move()
    void Draw();
    void BounceX();
    void BounceY();
    void BounceEdge(int screenWidth, int screenHeight);  // 你原来的边界反弹

    Vector2 GetPosition() const { return position; }
    float GetRadius() const { return radius; }
    void SetInvincible(bool inv) { invincible = inv; }
    bool IsInvincible() const { return invincible; }
};