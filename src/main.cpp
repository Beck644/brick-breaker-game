// 屏蔽Windows与Raylib命名冲突
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOMINMAX

#include "raylib.h"
#undef Rectangle
#undef CloseWindow
#undef ShowCursor

// ================== PPT要求：多线程核心头文件 ==================
#include <thread>
#include <mutex>
#include <future>
#include <chrono>

// 游戏基础常量
#define WIDTH  800
#define HEIGHT 600
#define BRICK_ROWS 4
#define BRICK_COLS 10

// ================== PPT要求：加载状态枚举 ==================
enum class LoadState { IDLE, LOADING, DONE };

// ================== 线程安全的共享数据（互斥锁保护） ==================
std::mutex g_dataMutex;
LoadState g_loadState = LoadState::IDLE;
Color g_brickColor = RED; // 砖块初始颜色，加载完成后变更
std::future<int> g_loadFuture; // 异步任务句柄，避免主线程阻塞

// ================== PPT要求：异步加载函数（工作线程执行） ==================
int AsyncLoadLargeResource()
{
    // 模拟加载大型纹理/资源的耗时操作（2秒，可自行调整）
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // 加锁修改共享数据，避免数据竞争
    std::lock_guard<std::mutex> guard(g_dataMutex);
    g_loadState = LoadState::DONE;
    g_brickColor = GREEN; // 加载完成，砖块变为绿色

    return 0; // 模拟返回资源ID，可扩展为真实纹理加载
}

// ================== 线程安全的读操作封装 ==================
LoadState GetLoadState()
{
    std::lock_guard<std::mutex> guard(g_dataMutex);
    return g_loadState;
}

Color GetBrickColor()
{
    std::lock_guard<std::mutex> guard(g_dataMutex);
    return g_brickColor;
}

int main()
{
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WIDTH, HEIGHT, "BrickBreaker 多线程异步加载作业");
    SetTargetFPS(60);

    // 游戏核心物体
    Rectangle hostPad  = { WIDTH/2 - 50, HEIGHT - 40, 100, 16 };
    Rectangle clientPad= { WIDTH/2 - 50, 40,       100, 16 };
    Vector2 ball = { WIDTH/2, HEIGHT/2 };
    Vector2 ballSpeed = { 4.0f, -4.0f };

    Rectangle bricks[BRICK_ROWS][BRICK_COLS];
    bool brickLocal[BRICK_ROWS][BRICK_COLS];

    // 砖块初始化
    for (int i = 0; i < BRICK_ROWS; i++)
        for (int j = 0; j < BRICK_COLS; j++)
        {
            bricks[i][j] = { (float)(j*75 + 10), (float)(i*25 + 170), 70.0f, 20.0f };
            brickLocal[i][j] = true;
        }

    while (!WindowShouldClose())
    {
        // ================== PPT要求：按L键触发异步加载 ==================
        if (IsKeyPressed(KEY_L) && GetLoadState() == LoadState::IDLE)
        {
            // 加锁修改加载状态
            {
                std::lock_guard<std::mutex> guard(g_dataMutex);
                g_loadState = LoadState::LOADING;
            }
            // 启动异步任务：强制新线程执行，符合PPT要求
            g_loadFuture = std::async(std::launch::async, AsyncLoadLargeResource);
        }

        // ================== 游戏核心逻辑（主线程不卡顿） ==================
        // 挡板控制
        if (IsKeyDown(KEY_A)) hostPad.x -= 5.0f;
        if (IsKeyDown(KEY_D)) hostPad.x += 5.0f;
        if (IsKeyDown(KEY_LEFT))  clientPad.x -= 5.0f;
        if (IsKeyDown(KEY_RIGHT)) clientPad.x += 5.0f;

        // 小球物理（仅在非加载状态运行，也可保留全程运行）
        if (GetLoadState() != LoadState::LOADING)
        {
            ball.x += ballSpeed.x;
            ball.y += ballSpeed.y;

            // 边界处理
            if (ball.x < 8 || ball.x > WIDTH - 8) ballSpeed.x *= -1;
            if (ball.y < 8 || ball.y > HEIGHT)
            {
                ball = { WIDTH/2, HEIGHT/2 };
                ballSpeed = { 4.0f, -4.0f };
            }

            // 挡板碰撞
            if (CheckCollisionCircleRec(ball, 8, hostPad)) ballSpeed.y *= -1;
            if (CheckCollisionCircleRec(ball, 8, clientPad)) ballSpeed.y *= -1;

            // 砖块碰撞
            for (int i = 0; i < BRICK_ROWS; i++)
                for (int j = 0; j < BRICK_COLS; j++)
                    if (brickLocal[i][j] && CheckCollisionCircleRec(ball, 8, bricks[i][j]))
                    {
                        brickLocal[i][j] = false;
                        ballSpeed.y *= -1;
                    }
        }

        // ================== 渲染（全程在主线程，符合Raylib限制） ==================
        BeginDrawing();
        ClearBackground(BLACK);

        // 绘制砖块（使用线程安全的颜色）
        Color currentBrickColor = GetBrickColor();
        for (int i = 0; i < BRICK_ROWS; i++)
            for (int j = 0; j < BRICK_COLS; j++)
                if (brickLocal[i][j])
                    DrawRectangleRec(bricks[i][j], currentBrickColor);

        // 绘制挡板和小球
        DrawRectangleRec(hostPad, BLUE);
        DrawRectangleRec(clientPad, ORANGE);
        DrawCircleV(ball, 8, WHITE);

        // ================== PPT要求：加载期间显示Loading... ==================
        if (GetLoadState() == LoadState::LOADING)
        {
            DrawText("Loading...", WIDTH/2 - 120, HEIGHT/2 - 20, 40, WHITE);
        }
        // 加载完成提示
        else if (GetLoadState() == LoadState::DONE)
        {
            DrawText("Load Complete!", WIDTH/2 - 150, HEIGHT/2 - 20, 40, GREEN);
        }
        // 初始提示
        else
        {
            DrawText("Press L to Start Async Load", WIDTH/2 - 220, 30, 24, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}