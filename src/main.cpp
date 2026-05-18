#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOMINMAX

#include "raylib.h"
#undef Rectangle
#undef CloseWindow
#undef ShowCursor

#include "../include/json.hpp"
using json = nlohmann::json;

#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <fstream>
#include <string>

// 游戏常量
#define WIDTH       800
#define HEIGHT      600
#define MAX_LEVELS  3

// 加载状态（多线程保留）
enum class LoadState { IDLE, LOADING, DONE };
std::mutex g_dataMutex;
LoadState g_loadState = LoadState::IDLE;
Color g_brickColor = RED;
std::future<int> g_loadFuture;

// 游戏全局状态
int g_score = 0;
int g_lives = 3;
int g_currentLevel = 1;
bool g_gameOver = false;

// 砖块结构体
struct Brick {
    Rectangle rect;
    bool active;
};

Brick g_bricks[10][10]; // 最大支持10x10砖块
int g_brickRows = 4;
int g_brickCols = 10;

// ====================== PPT要求：JSON加载带错误处理 ======================
json LoadJSONWithFallback(const std::string& path, const json& fallback) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            TraceLog(LOG_WARNING, "File not found: %s, using default config", path.c_str());
            return fallback;
        }
        json config;
        file >> config;
        return config;
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "JSON parse failed: %s, using default config", e.what());
        return fallback;
    }
}

// ====================== 从JSON加载完整关卡（用于新游戏）======================
void LoadFullLevel(int level) {
    g_currentLevel = level;
    g_gameOver = false;

    // 默认关卡配置
    json defaultConfig = {
        {"rows", 4},
        {"cols", 10},
        {"brick_width", 70},
        {"brick_height", 20},
        {"start_x", 10},
        {"start_y", 170},
        {"layout", {
            {1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1}
        }}
    };

    std::string filename = "levels/level" + std::to_string(level) + ".json";
    json config = LoadJSONWithFallback(filename, defaultConfig);

    // 解析配置
    g_brickRows = config["rows"].get<int>();
    g_brickCols = config["cols"].get<int>();
    float brickWidth = config["brick_width"].get<float>();
    float brickHeight = config["brick_height"].get<float>();
    float startX = config["start_x"].get<float>();
    float startY = config["start_y"].get<float>();
    auto layout = config["layout"];

    // 清空旧砖块
    memset(g_bricks, 0, sizeof(g_bricks));

    // 加载新砖块
    for (int i = 0; i < g_brickRows; i++) {
        for (int j = 0; j < g_brickCols; j++) {
            if (layout[i][j].get<int>() == 1) {
                g_bricks[i][j].rect = {
                    startX + j * brickWidth,
                    startY + i * brickHeight,
                    brickWidth,
                    brickHeight
                };
                g_bricks[i][j].active = true;
            } else {
                g_bricks[i][j].active = false;
            }
        }
    }

    TraceLog(LOG_INFO, "Full level %d loaded", level);
}

// ====================== 从存档加载关卡（保留砖块状态）======================
void LoadLevelFromSave(int level, const json& brickStates) {
    g_currentLevel = level;
    g_gameOver = false;

    // 先加载完整关卡的基本信息（位置、尺寸）
    json defaultConfig = {
        {"rows", 4},
        {"cols", 10},
        {"brick_width", 70},
        {"brick_height", 20},
        {"start_x", 10},
        {"start_y", 170}
    };

    std::string filename = "levels/level" + std::to_string(level) + ".json";
    json config = LoadJSONWithFallback(filename, defaultConfig);

    g_brickRows = config["rows"].get<int>();
    g_brickCols = config["cols"].get<int>();
    float brickWidth = config["brick_width"].get<float>();
    float brickHeight = config["brick_height"].get<float>();
    float startX = config["start_x"].get<float>();
    float startY = config["start_y"].get<float>();

    // 清空旧砖块
    memset(g_bricks, 0, sizeof(g_bricks));

    // 从存档恢复砖块状态
    for (int i = 0; i < g_brickRows; i++) {
        for (int j = 0; j < g_brickCols; j++) {
            g_bricks[i][j].rect = {
                startX + j * brickWidth,
                startY + i * brickHeight,
                brickWidth,
                brickHeight
            };
            g_bricks[i][j].active = brickStates[i][j].get<bool>();
        }
    }

    TraceLog(LOG_INFO, "Level %d loaded from save with brick states", level);
}

// ====================== PPT要求：保存游戏（完整保存砖块状态）======================
void SaveGame() {
    json save;
    save["version"] = 1;
    save["current_level"] = g_currentLevel;
    save["score"] = g_score;
    save["lives"] = g_lives;

    // 【新增】保存所有砖块的状态
    json brickStates = json::array();
    for (int i = 0; i < g_brickRows; i++) {
        json row = json::array();
        for (int j = 0; j < g_brickCols; j++) {
            row.push_back(g_bricks[i][j].active);
        }
        brickStates.push_back(row);
    }
    save["brick_states"] = brickStates;

    std::ofstream file("save.json");
    file << save.dump(4); // 格式化输出，缩进4空格
    TraceLog(LOG_INFO, "Game saved to save.json with brick states");
}

// ====================== PPT要求：加载游戏（恢复砖块状态）======================
bool LoadGame() {
    try {
        std::ifstream file("save.json");
        if (!file.is_open()) {
            return false;
        }
        json save;
        file >> save;

        if (save["version"].get<int>() == 1) {
            g_currentLevel = save["current_level"].get<int>();
            g_score = save["score"].get<int>();
            g_lives = save["lives"].get<int>();
            
            // 【新增】从存档恢复砖块状态
            if (save.contains("brick_states")) {
                LoadLevelFromSave(g_currentLevel, save["brick_states"]);
            } else {
                // 兼容旧版存档（没有砖块状态）
                LoadFullLevel(g_currentLevel);
            }
            
            TraceLog(LOG_INFO, "Save loaded successfully with brick states");
            return true;
        } else {
            TraceLog(LOG_WARNING, "Save version incompatible");
            return false;
        }
    } catch (...) {
        TraceLog(LOG_ERROR, "Save load failed");
        return false;
    }
}

// 异步加载函数（保留）
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

// 检测是否所有砖块都被打碎
bool AllBricksDestroyed() {
    for (int i = 0; i < g_brickRows; i++) {
        for (int j = 0; j < g_brickCols; j++) {
            if (g_bricks[i][j].active) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WIDTH, HEIGHT, "BrickBreaker - Data Persistence");
    SetTargetFPS(60);

    // 游戏对象
    Rectangle hostPad   = { WIDTH/2-50, HEIGHT-40, 100, 16 };
    Rectangle clientPad = { WIDTH/2-50, 40,       100, 16 };
    Vector2 ball        = { WIDTH/2, HEIGHT/2 };
    Vector2 ballSpeed   = { 4, -4 };

    // ====================== PPT要求：启动时检测存档 ======================
    bool hasSave = std::ifstream("save.json").good();
    bool continueGame = false;

    if (hasSave) {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Save Found!", WIDTH/2-90, HEIGHT/2-60, 32, WHITE);
            DrawText("Press C to Continue", WIDTH/2-140, HEIGHT/2, 24, WHITE);
            DrawText("Press N for New Game", WIDTH/2-150, HEIGHT/2+40, 24, WHITE);
            EndDrawing();

            if (IsKeyPressed(KEY_C)) {
                continueGame = true;
                break;
            }
            if (IsKeyPressed(KEY_N)) {
                continueGame = false;
                break;
            }
        }
    }

    if (continueGame) {
        LoadGame();
    } else {
        LoadFullLevel(1);
        g_score = 0;
        g_lives = 3;
    }

    while (!WindowShouldClose()) {
        // 按 S 手动存档
        if (IsKeyPressed(KEY_S)) {
            SaveGame();
        }

        // 按 L 异步加载
        if (IsKeyPressed(KEY_L) && GetLoadState() == LoadState::IDLE) {
            { std::lock_guard<std::mutex> g(g_dataMutex); g_loadState = LoadState::LOADING; }
            g_loadFuture = std::async(std::launch::async, AsyncLoadLargeResource);
        }

        if (!g_gameOver) {
            // 挡板控制
            if (IsKeyDown(KEY_A)) hostPad.x -= 5;
            if (IsKeyDown(KEY_D)) hostPad.x += 5;
            if (IsKeyDown(KEY_LEFT))  clientPad.x -= 5;
            if (IsKeyDown(KEY_RIGHT)) clientPad.x += 5;

            // 小球物理
            ball.x += ballSpeed.x;
            ball.y += ballSpeed.y;

            // 边界处理
            if (ball.x < 8 || ball.x > WIDTH-8) ballSpeed.x *= -1;

            // 上下边界死亡
            if (ball.y < 8 || ball.y > HEIGHT) {
                g_lives--;
                if (g_lives <= 0) {
                    g_gameOver = true;
                } else {
                    ball = { WIDTH/2, HEIGHT/2 };
                    ballSpeed = { 4, -4 };
                }
            }

            // 挡板碰撞
            if (CheckCollisionCircleRec(ball, 8, hostPad))  ballSpeed.y *= -1;
            if (CheckCollisionCircleRec(ball, 8, clientPad)) ballSpeed.y *= -1;

            // 砖块碰撞
            Color brickColor = GetBrickColor();
            for (int i = 0; i < g_brickRows; i++) {
                for (int j = 0; j < g_brickCols; j++) {
                    if (g_bricks[i][j].active && CheckCollisionCircleRec(ball, 8, g_bricks[i][j].rect)) {
                        g_bricks[i][j].active = false;
                        ballSpeed.y *= -1;
                        g_score += 10;
                    }
                }
            }

            // ====================== PPT要求：通关自动加载下一关 ======================
            if (AllBricksDestroyed()) {
                if (g_currentLevel < MAX_LEVELS) {
                    g_currentLevel++;
                    LoadFullLevel(g_currentLevel);
                    ball = { WIDTH/2, HEIGHT/2 };
                    ballSpeed = { 4, -4 };
                    SaveGame(); // 自动保存进度
                } else {
                    g_gameOver = true;
                }
            }
        }

        // 游戏结束重置
        if (g_gameOver && IsKeyPressed(KEY_R)) {
            LoadFullLevel(1);
            g_score = 0;
            g_lives = 3;
            ball = { WIDTH/2, HEIGHT/2 };
            ballSpeed = { 4, -4 };
        }

        // 渲染
        BeginDrawing();
        ClearBackground(BLACK);

        // 绘制砖块
        Color brickColor = GetBrickColor();
        for (int i = 0; i < g_brickRows; i++) {
            for (int j = 0; j < g_brickCols; j++) {
                if (g_bricks[i][j].active) {
                    DrawRectangleRec(g_bricks[i][j].rect, brickColor);
                }
            }
        }

        DrawRectangleRec(hostPad, BLUE);
        DrawRectangleRec(clientPad, ORANGE);
        DrawCircleV(ball, 8, WHITE);

        // UI（全英文，无乱码）
        DrawText(TextFormat("Score: %d", g_score), 10, 10, 20, WHITE);
        DrawText(TextFormat("Lives: %d", g_lives), 10, 40, 20, WHITE);
        DrawText(TextFormat("Level: %d/%d", g_currentLevel, MAX_LEVELS), 10, 70, 20, WHITE);
        DrawText("S: Save | L: Load", WIDTH-200, 10, 18, GRAY);

        // 加载提示
        if (GetLoadState() == LoadState::LOADING)
            DrawText("LOADING...", WIDTH/2-120, HEIGHT/2, 40, WHITE);

        // 游戏结束提示
        if (g_gameOver) {
            if (g_lives <= 0) {
                DrawText("GAME OVER", WIDTH/2-120, HEIGHT/2-40, 40, RED);
            } else {
                DrawText("YOU WIN!", WIDTH/2-100, HEIGHT/2-40, 40, GREEN);
            }
            DrawText("Press R to Restart", WIDTH/2-130, HEIGHT/2+20, 24, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}