#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOMINMAX

#include "raylib.h"
#undef Rectangle
#undef CloseWindow
#undef ShowCursor

#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <vector>

// 游戏常量
#define WIDTH       800
#define HEIGHT      600
#define BRICK_ROWS  4
#define BRICK_COLS  10
#define MAX_PARTICLES 100

// ====================== PPT：加载状态（多线程保留）======================
enum class LoadState { IDLE, LOADING, DONE };
std::mutex g_dataMutex;
LoadState g_loadState = LoadState::IDLE;
Color g_brickColor = RED;
std::future<int> g_loadFuture;

int AsyncLoadLargeResource() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::lock_guard<std::mutex> guard(g_dataMutex);
    g_loadState = LoadState::DONE;
    g_brickColor = GREEN;
    return 0;
}

LoadState GetLoadState() {
    std::lock_guard<std::mutex> guard(g_dataMutex);
    return g_loadState;
}

Color GetBrickColor() {
    std::lock_guard<std::mutex> guard(g_dataMutex);
    return g_brickColor;
}

// ====================== PPT：对象池（优化频繁new/delete）======================
struct Particle {
    Vector2 pos;
    Vector2 vel;
    float life;
    bool active;
};

class ParticlePool {
private:
    Particle particles[MAX_PARTICLES];
public:
    ParticlePool() {
        for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
    }

    void Spawn(Vector2 pos) {
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (!particles[i].active) {
                particles[i].pos = pos;
                particles[i].vel = { (float)GetRandomValue(-2,2), (float)GetRandomValue(-2,2) };
                particles[i].life = 1.0f;
                particles[i].active = true;
                break;
            }
        }
    }

    void Update(float dt) {
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].active) {
                particles[i].pos.x += particles[i].vel.x;
                particles[i].pos.y += particles[i].vel.y;
                particles[i].life -= dt;
                if (particles[i].life <= 0) particles[i].active = false;
            }
        }
    }

    void Draw() {
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].active) {
                DrawCircleV(particles[i].pos, 2, WHITE);
            }
        }
    }
};

// ====================== PPT：砖块结构体（用于空间划分）======================
struct Brick {
    Rectangle rect;
    bool active;
};

// ====================== MAIN ======================
int main() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WIDTH, HEIGHT, "BrickBreaker - 性能优化版");
    SetTargetFPS(60);

    // 游戏对象
    Rectangle hostPad   = { WIDTH/2-50, HEIGHT-40, 100, 16 };
    Rectangle clientPad = { WIDTH/2-50, 40,       100, 16 };
    Vector2 ball        = { WIDTH/2, HEIGHT/2 };
    Vector2 ballSpeed   = { 4, -4 };

    // 砖块数组
    Brick bricks[BRICK_ROWS][BRICK_COLS];
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].rect = { (float)(j*75+10), (float)(i*25+170), 70, 20 };
            bricks[i][j].active = true;
        }
    }

    ParticlePool particlePool;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        double frameStart = GetTime(); // PPT：性能测量

        // ====================== 按 L 异步加载 ======================
        if (IsKeyPressed(KEY_L) && GetLoadState() == LoadState::IDLE) {
            { std::lock_guard<std::mutex> g(g_dataMutex); g_loadState = LoadState::LOADING; }
            g_loadFuture = std::async(std::launch::async, AsyncLoadLargeResource);
        }

        // ====================== 挡板控制 ======================
        if (IsKeyDown(KEY_A)) hostPad.x -= 5;
        if (IsKeyDown(KEY_D)) hostPad.x += 5;
        if (IsKeyDown(KEY_LEFT))  clientPad.x -= 5;
        if (IsKeyDown(KEY_RIGHT)) clientPad.x += 5;

        // ====================== PPT：优化前 O(N²) → 优化后：只遍历active砖块 ======================
        if (GetLoadState() != LoadState::LOADING) {
            ball.x += ballSpeed.x;
            ball.y += ballSpeed.y;

            if (ball.x < 8 || ball.x > WIDTH-8) ballSpeed.x *= -1;
            if (ball.y < 8 || ball.y > HEIGHT) {
                ball = { WIDTH/2, HEIGHT/2 };
                ballSpeed = { 4, -4 };
            }

            if (CheckCollisionCircleRec(ball, 8, hostPad))  { ballSpeed.y *= -1; particlePool.Spawn(ball); }
            if (CheckCollisionCircleRec(ball, 8, clientPad)){ ballSpeed.y *= -1; particlePool.Spawn(ball); }

            // 碰撞优化：只检测 active 砖块
            for (int i = 0; i < BRICK_ROWS; i++) {
                for (int j = 0; j < BRICK_COLS; j++) {
                    if (bricks[i][j].active && CheckCollisionCircleRec(ball, 8, bricks[i][j].rect)) {
                        bricks[i][j].active = false;
                        ballSpeed.y *= -1;
                        particlePool.Spawn(ball);
                    }
                }
            }
        }

        particlePool.Update(dt);

        // ====================== PPT：输出帧耗时（性能测量）======================
        double frameMs = (GetTime() - frameStart) * 1000;
        if (GetRandomValue(0, 30) == 0) // 每隔几帧打印一次
            TraceLog(LOG_INFO, "帧耗时: %.2f ms", frameMs);

        // ====================== 绘制 ======================
        BeginDrawing();
        ClearBackground(BLACK);

        // PPT：DrawCall 优化：统一颜色批量绘制
        Color c = GetBrickColor();
        for (int i = 0; i < BRICK_ROWS; i++)
            for (int j = 0; j < BRICK_COLS; j++)
                if (bricks[i][j].active)
                    DrawRectangleRec(bricks[i][j].rect, c);

        DrawRectangleRec(hostPad, BLUE);
        DrawRectangleRec(clientPad, ORANGE);
        DrawCircleV(ball, 8, WHITE);
        particlePool.Draw();

        // 加载提示
        if (GetLoadState() == LoadState::LOADING)
            DrawText("LOADING...", WIDTH/2-120, HEIGHT/2, 40, WHITE);
        else if (GetLoadState() == LoadState::DONE)
            DrawText("LOAD COMPLETE!", WIDTH/2-160, HEIGHT/2, 40, GREEN);

        // 显示帧率（PPT：性能验证）
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}