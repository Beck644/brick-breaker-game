// 必须先定义这个宏，修复 MinGW 链接错误
#define _WIN32_WINNT 0x0601
#include "raylib.h"
#include <cassert>
#include <iostream>
#include <cstdlib> // 引入 crt 相关头文件

// 核心碰撞测试函数
void TestCollisionLogic() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "        碰撞函数单元测试开始         " << std::endl;
    std::cout << "=====================================\n" << std::endl;

    // 测试1：圆与矩形碰撞（球-砖块场景）
    Vector2 ballCenter1 = {100.0f, 100.0f};
    float radius1 = 10.0f;
    Rectangle brick1 = {95.0f, 95.0f, 20.0f, 20.0f};
    bool res1 = CheckCollisionCircleRec(ballCenter1, radius1, brick1);
    assert(res1 && "❌ 测试1失败：圆与矩形应该碰撞！");
    std::cout << "✅ 测试1（圆-矩形碰撞）：通过" << std::endl;

    // 测试2：圆与矩形不碰撞
    Vector2 ballCenter2 = {100.0f, 100.0f};
    float radius2 = 10.0f;
    Rectangle brick2 = {200.0f, 200.0f, 20.0f, 20.0f};
    bool res2 = CheckCollisionCircleRec(ballCenter2, radius2, brick2);
    assert(!res2 && "❌ 测试2失败：圆与矩形不该碰撞！");
    std::cout << "✅ 测试2（圆-矩形不碰撞）：通过" << std::endl;

    // 测试3：圆与矩形碰撞（球-挡板场景）
    Vector2 ballCenter3 = {400.0f, 500.0f};
    float radius3 = 10.0f;
    Rectangle paddle = {350.0f, 510.0f, 100.0f, 20.0f};
    bool res3 = CheckCollisionCircleRec(ballCenter3, radius3, paddle);
    assert(res3 && "❌ 测试3失败：圆与矩形应该碰撞！");
    std::cout << "✅ 测试3（圆-矩形碰撞）：通过" << std::endl;

    std::cout << "\n=====================================" << std::endl;
    std::cout << "        所有碰撞测试通过！🎉         " << std::endl;
    std::cout << "=====================================\n" << std::endl;
}

// 测试程序入口（修复 crt 链接问题）
int main() {
    TestCollisionLogic();
    // 显式调用 exit，避免 crt 析构错误
    std::exit(EXIT_SUCCESS);
    return 0;
}