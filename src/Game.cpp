#include "Game.h"
#include <iostream>

Game::Game(int width, int height)
    : screenWidth(width), screenHeight(height), currentState(GameState::MENU) {}

void Game::Init() {
    InitBricks();
    ResetGame();
}

void Game::InitBricks() {
    bricks.clear();
    float w = 80;
    float h = 25;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            // 修复：补上颜色参数 GOLD
            bricks.emplace_back(10 + j * (w + 10), 60 + i * (h + 10), w, h, GOLD);
        }
    }
}

void Game::ResetGame() {
    gameData.score = 0;
    gameData.lives = 3;
    ball.Reset();
    paddle.Reset();
}

void Game::HandleInput() {
    switch (currentState) {
    case GameState::MENU:
        if (IsKeyPressed(KEY_SPACE)) currentState = GameState::PLAYING;
        break;

    case GameState::PLAYING:
        if (IsKeyPressed(KEY_P)) currentState = GameState::PAUSED;

        // 无敌模式 G 键
        if (IsKeyPressed(KEY_G)) {
            ball.SetInvincible(!ball.IsInvincible());
            std::cout << "无敌: " << (ball.IsInvincible() ? "ON" : "OFF") << std::endl;
        }

        // 修复：调用无参数版本
        if (IsKeyDown(KEY_LEFT)) paddle.MoveLeft();
        if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight();
        paddle.MoveWithMouse();
        break;

    case GameState::PAUSED:
        if (IsKeyPressed(KEY_P)) currentState = GameState::PLAYING;
        break;

    case GameState::GAMEOVER:
        if (IsKeyPressed(KEY_R)) {
            currentState = GameState::MENU;
            ResetGame();
        }
        break;
    }
}

void Game::CheckCollisions() {
    if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), paddle.GetRect())) {
        ball.BounceY();
    }

    for (auto& b : bricks) {
        if (b.IsActive() && CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), b.GetRect())) {
            b.SetActive(false);
            gameData.score += 10;
            ball.BounceY();
        }
    }
}

void Game::Update() {
    HandleInput();

    if (currentState == GameState::PLAYING) {
        ball.Update();
        CheckCollisions();

        // 修复：调用你原来的边界反弹函数
        ball.BounceEdge(screenWidth, screenHeight);

        // 无敌判断
        if (!ball.IsInvincible() && ball.GetPosition().y + ball.GetRadius() >= screenHeight) {
            gameData.lives--;
            ball.Reset();
            if (gameData.lives <= 0) currentState = GameState::GAMEOVER;
        }
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentState) {
    case GameState::MENU:
        DrawText("BRICK BREAKER", 250, 200, 50, DARKBLUE);
        DrawText("按空格开始", 320, 300, 30, DARKGRAY);
        break;

    case GameState::PLAYING:
    case GameState::PAUSED:
        ball.Draw();
        paddle.Draw();
        for (auto& b : bricks) if (b.IsActive()) b.Draw();

        DrawText(TextFormat("分数: %d", gameData.score), 20, 20, 24, BLACK);
        DrawText(TextFormat("生命: %d", gameData.lives), 680, 20, 24, BLACK);

        if (currentState == GameState::PAUSED) {
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0,0,0,150 });
            DrawText("已暂停", 330, 250, 50, WHITE);
        }
        break;

    case GameState::GAMEOVER:
        DrawText("游戏结束", 280, 200, 50, RED);
        DrawText(TextFormat("最终分数: %d", gameData.score), 300, 280, 30, BLACK);
        DrawText("按 R 返回菜单", 290, 330, 30, DARKGRAY);
        break;
    }

    EndDrawing();
}

void Game::Shutdown() {
    std::cout << "游戏退出" << std::endl;
}