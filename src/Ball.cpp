#include "Ball.h"

// 无参构造（Game 里用的是这个）
Ball::Ball() {
    radius = 10.0f;
    Reset();
}

// 你原来的带参构造（保留兼容）
Ball::Ball(Vector2 pos, Vector2 sp, float r) 
    : position(pos), speedX(sp.x), speedY(sp.y), radius(r) {}

void Ball::Reset() {
    position = {400, 300};
    speedX = 2.5f;
    speedY = -2.5f;
}

// 把你原来的 Move() 改名为 Update()
void Ball::Update() {
    position.x += speedX;
    position.y += speedY;
}

// 你原来的边界反弹函数（名字改回 BounceEdge）
void Ball::BounceEdge(int screenWidth, int screenHeight) {
    if (position.x - radius <= 0 || position.x + radius >= screenWidth) BounceX();
    if (position.y - radius <= 0) BounceY();
}

void Ball::Draw() {
    DrawCircleV(position, radius, MAROON);
}
void Ball::BounceX() { speedX *= -1; }
void Ball::BounceY() { speedY *= -1; }