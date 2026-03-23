#include "Ball.h"

Ball::Ball(Vector2 pos, Vector2 sp, float r) {
    position = pos;
    speed = sp;
    radius = r;
    isAlive = true;
}

void Ball::Move() {
    if (!isAlive) return;
    position.x += speed.x;
    position.y += speed.y;
}

void Ball::Draw() {
    DrawCircleV(position, radius, RED);
}

void Ball::BounceEdge(int screenWidth, int screenHeight) {
    if (!isAlive) return;
    
    // 左右边界反弹
    if (position.x - radius <= 0 || position.x + radius >= screenWidth) {
        speed.x *= -1;
    }
    // 上边界反弹
    if (position.y - radius <= 0) {
        speed.y *= -1;
    }
    // 下边界：标记死亡（不直接结束游戏，扣生命）
    if (position.y + radius >= screenHeight) {
        Kill();
    }
}