#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "raylib.h"

// 游戏全局数据
struct GameData {
    int score;          // 分数
    int lives;          // 剩余生命
    const int maxLives = 3; // 最大生命数
    bool isGameOver;    // 游戏是否结束

    // 初始化
    GameData() {
        score = 0;
        lives = maxLives;
        isGameOver = false;
    }

    // 加分
    void AddScore(int points) {
        score += points;
    }

    // 扣生命
    void LoseLife() {
        lives--;
        if (lives <= 0) {
            lives = 0;
            isGameOver = true;
        }
    }

    // 重置游戏
    void Reset() {
        score = 0;
        lives = maxLives;
        isGameOver = false;
    }
};

#endif