🧱 Brick Breaker Game
基于 Raylib 开发的经典打砖块游戏，支持菜单、暂停和无敌模式。
✨ 功能特性
完整游戏流程：开始菜单 → 游戏中 → 暂停 → 游戏结束 → 返回菜单
无敌模式：按 G 键开启 / 关闭，小球碰到底部自动反弹，不会掉命
无限关卡：清空当前砖块后自动生成新一波砖块
暂停功能：按 P 键暂停 / 继续游戏
分数与生命系统：实时显示分数和剩余生命
🎮 操作说明
表格
按键	功能
← →	左右移动挡板
G	开启 / 关闭无敌模式
P	暂停 / 继续游戏
SPACE	开始游戏 / 游戏结束后返回菜单
🛠️ 编译与运行
环境要求
C++ 编译器（支持 C++11 或更高版本）
Raylib 库
编译命令
bash
运行
g++ src/main.cpp -o BrickBreaker -lraylib -lopengl32 -lgdi32 -lwinmm
运行
直接生成的可执行文件 BrickBreaker.exe 即可运行游戏。
📁 项目结构
plaintext
BrickBreaker/git add README.md
├── src/
│   ├── main.cpp          # 主程序入口
│   ├── Ball.cpp/h        # 小球类
│   ├── Brick.cpp/h       # 砖块类
│   ├── Paddle.cpp/h      # 挡板类
│   ├── GameData.h        # 游戏数据定义
│   └── test_collision.cpp# 碰撞检测单元测试
├── include/              # Raylib 头文件
├── docs/                 # 文档与代码评审记录
└── makefile              # 构建脚本
📝 更新日志
feat: complete brick breaker game：完成基础打砖块游戏逻辑
feat: add menu, pause and god mode：新增菜单、暂停功能和无敌模式
test: add collision unit test file：添加碰撞检测单元测试
📄 License
MIT License
