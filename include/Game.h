#pragma once
#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "GameData.h"
#include <vector>

// 游戏状态机（PPT 要求：消除 bool 瘟疫，用枚举管理状态）
enum class GameState {
    MENU,       // 主菜单
    PLAYING,    // 游戏中
    PAUSED,     // 暂停
    GAMEOVER    // 游戏结束
};

class Game {
private:
    // 游戏核心对象（封装私有，符合面向对象）
    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;
    GameData gameData;
    GameState currentState;

    // 窗口配置（后续会从 config.json 读取，先写死过渡）
    int screenWidth;
    int screenHeight;

    // 内部辅助方法（对外隐藏）
    void InitBricks();    // 初始化砖块
    void ResetGame();     // 重置游戏状态
    void CheckCollisions();// 检测所有碰撞
    void HandleInput();   // 处理输入

public:
    // 构造函数：初始化窗口尺寸
    Game(int width, int height);
    ~Game() = default;

    // 核心接口（仅暴露给 main.cpp，符合封装原则）
    void Init();    // 游戏初始化（加载资源、创建对象）
    void Update();  // 逐帧更新逻辑
    void Draw();    // 逐帧渲染画面
    void Shutdown();// 清理资源
};