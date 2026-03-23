#ifndef BALL_H
#define BALL_H

#include "raylib.h"

class Ball {
private:
    Vector2 position;
    Vector2 speed;
    float radius;
    bool isAlive;  // 小球是否存活（碰下边框后死亡）

public:
    Ball(Vector2 pos, Vector2 sp, float r);
    void Move();
    void Draw();
    void BounceEdge(int screenWidth, int screenHeight);
    Vector2 GetPosition() { return position; }
    float GetRadius() { return radius; }
    Vector2 GetSpeed() { return speed; }
    void SetSpeed(Vector2 newSpeed) { speed = newSpeed; }
    bool IsAlive() { return isAlive; }
    void Kill() { isAlive = false; }
    void Revive(Vector2 pos) { // 复活小球（重新定位）
        position = pos;
        isAlive = true;
        speed = { 4, 4 }; // 重置速度
    }
};

#endif