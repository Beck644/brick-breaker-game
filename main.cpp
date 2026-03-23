#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "GameData.h"
#include <vector>
#include <cstdlib> // 用于随机数
#include <ctime>   // 初始化随机数种子

// 函数声明（解决未声明报错）
void DrawLives(int lives);
void GenerateRandomBricks(std::vector<Brick>& bricks, int screenWidth);

// 生成随机位置的砖块
void GenerateRandomBricks(std::vector<Brick>& bricks, int screenWidth) {
    bricks.clear(); // 清空原有砖块
    const int brickW = 80, brickH = 30, spacing = 5;
    const int rows = 4, cols = 8; // 4行8列
    
    // 初始化随机数种子
    srand(time(NULL));
    
    for (int row=0; row<rows; row++) {
        for (int col=0; col<cols; col++) {
            // 随机X坐标（限制在窗口内）
            float minX = 10;
            float maxX = screenWidth - brickW - 10;
            float x = minX + (rand() % (int)(maxX - minX));
            
            // 随机Y坐标（限制在窗口上半部分）
            float minY = 20;
            float maxY = 200;
            float y = minY + (rand() % (int)(maxY - minY));
            
            // 随机颜色（橙色系）
            Color color = Color{ (unsigned char)(200 + rand()%55), (unsigned char)(100 + rand()%55), 0, 255 };
            bricks.emplace_back(x, y, brickW, brickH, color);
        }
    }
}

// 绘制生命条（红心）
void DrawLives(int lives) {
    const int heartSize = 30;
    const int startX = 10;
    const int startY = 40;
    
    for (int i=0; i<3; i++) {
        int x = startX + i * (heartSize + 5);
        // 存活的生命显示红色心形，用完的显示灰色
        if (i < lives) {
            DrawText("♥", x, startY, heartSize, RED);
        } else {
            DrawText("♥", x, startY, heartSize, GRAY);
        }
    }
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    // 初始化窗口
    InitWindow(screenWidth, screenHeight, "2D 打砖块游戏（带生命+计分+重生）");
    SetTargetFPS(60);

    // 初始化游戏对象
    Ball ball({ screenWidth/2, screenHeight/2 }, { 4, 4 }, 10);
    Paddle paddle(screenWidth/2 - 60, screenHeight - 30, 120, 20, 8);
    GameData gameData; // 游戏数据（分数、生命）
    
    // 初始生成砖块
    std::vector<Brick> bricks;
    GenerateRandomBricks(bricks, screenWidth);

    // 游戏主循环
    while (!WindowShouldClose()) {
        // ========== 1. 更新逻辑 ==========
        if (!gameData.isGameOver) {
            // 处理砖块拖拽
            for (auto& brick : bricks) {
                brick.Update();
            }

            // 处理挡板拖拽（修复：Paddle 已实现 UpdateDrag）
            paddle.UpdateDrag(screenWidth);

            // 处理小球逻辑
            if (ball.IsAlive()) {
                ball.Move();
                ball.BounceEdge(screenWidth, screenHeight);
                
                // 挡板方向键控制（保留）
                if (IsKeyDown(KEY_LEFT)) paddle.MoveLeft();
                if (IsKeyDown(KEY_RIGHT)) paddle.MoveRight(screenWidth);

                // 球与挡板碰撞
                if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), paddle.GetRect())) {
                    Vector2 ballSpeed = ball.GetSpeed();
                    ballSpeed.y *= -1;
                    
                    float paddleCenter = paddle.GetRect().x + paddle.GetRect().width / 2;
                    float hitOffset = ball.GetPosition().x - paddleCenter;
                    ballSpeed.x = hitOffset * 0.1f;
                    ball.SetSpeed(ballSpeed);
                }

                // 球与砖块碰撞（计分+击碎）
                for (auto& brick : bricks) {
                    if (brick.IsActive() && brick.CheckCollision(ball)) {
                        brick.SetActive(false);
                        gameData.AddScore(5); // 每块砖加5分
                        
                        Vector2 ballSpeed = ball.GetSpeed();
                        ballSpeed.y *= -1;
                        ball.SetSpeed(ballSpeed);
                        break;
                    }
                }

                // 检测是否所有砖块都被击碎 → 重生新砖块
                bool allBricksDestroyed = true;
                for (const auto& brick : bricks) {
                    if (brick.IsActive()) { // 修复：IsActive() 是 const 方法
                        allBricksDestroyed = false;
                        break;
                    }
                }
                if (allBricksDestroyed) {
                    GenerateRandomBricks(bricks, screenWidth);
                }
            } else {
                // 小球死亡 → 扣生命
                gameData.LoseLife();
                
                // 还有生命 → 复活小球
                if (gameData.lives > 0) {
                    ball.Revive({ screenWidth/2, screenHeight/2 });
                }
            }
        } else {
            // 游戏结束 → 按R键重置
            if (IsKeyPressed(KEY_R)) {
                gameData.Reset();
                ball.Revive({ screenWidth/2, screenHeight/2 });
                GenerateRandomBricks(bricks, screenWidth);
            }
        }

        // ========== 2. 绘制逻辑 ==========
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameData.isGameOver) {
            // 绘制游戏元素
            ball.Draw();
            paddle.Draw();
            for (auto& brick : bricks) brick.Draw();

            // 绘制分数
            DrawText(TextFormat("Score: %d", gameData.score), screenWidth - 120, 10, 20, DARKGRAY);
            
            // 绘制生命条
            DrawLives(gameData.lives);
            DrawText("Lives:", 10, 10, 20, DARKGRAY);
        } else {
            // 游戏结束界面
            DrawText("GAME OVER!", screenWidth/2 - 120, screenHeight/2 - 60, 40, RED);
            DrawText(TextFormat("Final Score: %d", gameData.score), screenWidth/2 - 100, screenHeight/2, 25, DARKGRAY);
            DrawText("Press R to restart", screenWidth/2 - 90, screenHeight/2 + 40, 20, DARKGRAY);
        }

        EndDrawing();
    }

    // 关闭窗口
    CloseWindow();
    return 0;
}