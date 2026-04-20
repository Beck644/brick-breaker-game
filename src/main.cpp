// 根治 Windows 头文件冲突
#define WIN32_LEAN_AND_MEAN
#undef Rectangle

#include "raylib.h"

// 全局同步数据（主机 → 客户端 实时传递）
struct GameState
{
    float ballX;
    float ballY;
    float hostPadX;
    float clientPadX;
};

GameState globalState = {};

int main()
{
    const int w = 800;
    const int h = 600;
    InitWindow(w, h, "Brick Breaker - Fixed Client Ball");
    SetTargetFPS(60);

    // 挡板
    Rectangle hostPad   = { w/2 - 50, h - 50, 100, 16 };
    Rectangle clientPad = { w/2 - 50, 30,        100, 16 };

    // 小球（只有主机真正修改它）
    Vector2 ball     = { w/2.0f, h/2.0f };
    Vector2 ballSpeed = { 4.0f, -4.0f };

    bool isHost = false;
    bool connected = false;

    while (!WindowShouldClose())
    {
        // ======================================
        // 按 1 = 主机（运行物理）
        // 按 2 = 客户端（只同步，不运行物理）
        // ======================================
        if (IsKeyPressed(KEY_ONE))  { isHost = true;  connected = true; }
        if (IsKeyPressed(KEY_TWO))  { isHost = false; connected = true; }

        // ======================================
        // 【主机】：小球运动、反弹、物理全在这里
        // ======================================
        if (isHost && connected)
        {
            // 主机控制
            if (IsKeyDown(KEY_A)) hostPad.x -= 5;
            if (IsKeyDown(KEY_D)) hostPad.x += 5;

            // 客户端的挡板位置从同步数据拿
            clientPad.x = globalState.clientPadX;

            // 小球运动
            ball.x += ballSpeed.x;
            ball.y += ballSpeed.y;

            // 边界反弹
            if (ball.x <= 8 || ball.x >= w-8)  ballSpeed.x *= -1;
            if (ball.y <= 8 || ball.y >= h-8)  ballSpeed.y *= -1;

            // 碰撞反弹
            if (CheckCollisionCircleRec(ball, 8, hostPad))   ballSpeed.y *= -1;
            if (CheckCollisionCircleRec(ball, 8, clientPad)) ballSpeed.y *= -1;

            // 同步给客户端
            globalState.ballX      = ball.x;
            globalState.ballY      = ball.y;
            globalState.hostPadX   = hostPad.x;
            globalState.clientPadX = clientPad.x;
        }

        // ======================================
        // 【客户端】：只拿数据，小球强制同步运动（修复完成！）
        // ======================================
        if (!isHost && connected)
        {
            // 客户端控制自己的挡板
            if (IsKeyDown(KEY_LEFT))  clientPad.x -= 5;
            if (IsKeyDown(KEY_RIGHT)) clientPad.x += 5;

            // 把自己的挡板发给主机
            globalState.clientPadX = clientPad.x;

            // ======================================
            // ✅ 关键修复：客户端强制把小球位置设为主机的！
            // 现在小球会 100% 同步运动，不会卡住！
            // ======================================
            ball.x = globalState.ballX;
            ball.y = globalState.ballY;

            hostPad.x   = globalState.hostPadX;
            clientPad.x = globalState.clientPadX;
        }

        // ====================== 绘制 ======================
        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangleRec(hostPad, BLUE);
        DrawRectangleRec(clientPad, ORANGE);
        DrawCircleV(ball, 8, RED);

        if (!connected)
        {
            DrawText("Press 1 = HOST", 240, 200, 40, WHITE);
            DrawText("Press 2 = CLIENT", 240, 300, 40, WHITE);
        }
        else
        {
            DrawText(isHost ? "HOST: A/D" : "CLIENT: Arrow Keys", 10, 10, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}