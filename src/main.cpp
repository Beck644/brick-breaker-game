#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOMINMAX

#include "raylib.h"
#undef Rectangle
#undef CloseWindow
#undef ShowCursor

#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")

#define WIDTH  800
#define HEIGHT 600
#define BRICK_ROWS 4   // 改为4排，删除最上面一排
#define BRICK_COLS 10
#define PORT 7777

// 网络同步结构体
typedef struct
{
    float ballX, ballY;
    float hostPadX;
    float clientPadX;
    bool bricks[BRICK_ROWS][BRICK_COLS];
} SyncState;

SOCKET udpSock = INVALID_SOCKET;
sockaddr_in peerAddr = { 0 };
bool isHost = false;
bool isConnected = false;
SyncState gameGlobal = { 0 };

// UDP初始化
void NetInit(bool hostMode)
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    udpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    u_long nonBlock = 1;
    ioctlsocket(udpSock, FIONBIO, &nonBlock);

    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(PORT);

    if (hostMode)
    {
        sockaddr_in bindAddr = {0};
        bindAddr.sin_family = AF_INET;
        bindAddr.sin_port = htons(PORT);
        bindAddr.sin_addr.s_addr = INADDR_ANY;
        bind(udpSock, (sockaddr*)&bindAddr, sizeof(bindAddr));
        isHost = true;
    }
    else
    {
        peerAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        isHost = false;
    }
    isConnected = true;
}

void NetSend(SyncState data)
{
    sendto(udpSock, (const char*)&data, sizeof(SyncState), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
}

void NetRecv()
{
    SyncState temp;
    sockaddr_in fromAddr;
    int addrLen = sizeof(fromAddr);
    int ret = recvfrom(udpSock, (char*)&temp, sizeof(SyncState), 0, (sockaddr*)&fromAddr, &addrLen);
    if (ret > 0) { gameGlobal = temp; peerAddr = fromAddr; }
}

int main()
{
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WIDTH, HEIGHT, "BrickBreaker Final");
    SetTargetFPS(60);

    // 游戏物体
    Rectangle hostPad  = { WIDTH/2 - 50, HEIGHT - 40, 100, 16 };
    Rectangle clientPad= { WIDTH/2 - 50, 40,       100, 16 };
    Vector2 ball = { WIDTH/2, HEIGHT/2 };
    Vector2 ballSpeed = { 4.0f, -4.0f };

    Rectangle bricks[BRICK_ROWS][BRICK_COLS];
    bool brickLocal[BRICK_ROWS][BRICK_COLS];

    // 砖块居中布局（4排，无最上排）
    for (int i = 0; i < BRICK_ROWS; i++)
        for (int j = 0; j < BRICK_COLS; j++)
        {
            bricks[i][j] = { (float)(j*75 + 10), (float)(i*25 + 170), 70.0f, 20.0f };
            brickLocal[i][j] = true;
        }

    while (!WindowShouldClose())
    {
        if (!isConnected)
        {
            if (IsKeyPressed(KEY_ONE))  NetInit(true);
            if (IsKeyPressed(KEY_TWO)) NetInit(false);
        }

        if (isConnected) NetRecv();

        // 主机逻辑
        if (isHost && isConnected)
        {
            if (IsKeyDown(KEY_A)) hostPad.x -= 5.0f;
            if (IsKeyDown(KEY_D)) hostPad.x += 5.0f;

            clientPad.x = gameGlobal.clientPadX;
            ball.x += ballSpeed.x;
            ball.y += ballSpeed.y;

            // 左右边界反弹
            if (ball.x < 8 || ball.x > WIDTH - 8) ballSpeed.x *= -1;

            // 顶部&底部 都死亡重置（不反弹）
            if (ball.y < 8 || ball.y > HEIGHT)
            {
                ball = { WIDTH/2, HEIGHT/2 };
                ballSpeed = { 4.0f, -4.0f };
                for(int i=0;i<BRICK_ROWS;i++)for(int j=0;j<BRICK_COLS;j++) brickLocal[i][j]=true;
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

            // 同步发包
            gameGlobal.ballX = ball.x;
            gameGlobal.ballY = ball.y;
            gameGlobal.hostPadX = hostPad.x;
            gameGlobal.clientPadX = clientPad.x;
            memcpy(gameGlobal.bricks, brickLocal, sizeof(brickLocal));
            NetSend(gameGlobal);
        }

        // 客户端逻辑
        if (!isHost && isConnected)
        {
            static bool firstSend = true;
            if (firstSend) { gameGlobal.clientPadX = clientPad.x; NetSend(gameGlobal); firstSend = false; }

            if (IsKeyDown(KEY_LEFT))  clientPad.x -= 5.0f;
            if (IsKeyDown(KEY_RIGHT)) clientPad.x += 5.0f;

            // 强制同步主机数据
            ball.x = gameGlobal.ballX;
            ball.y = gameGlobal.ballY;
            hostPad.x = gameGlobal.hostPadX;
            memcpy(brickLocal, gameGlobal.bricks, sizeof(brickLocal));

            gameGlobal.clientPadX = clientPad.x;
            NetSend(gameGlobal);
        }

        // 渲染
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < BRICK_ROWS; i++)
            for (int j = 0; j < BRICK_COLS; j++)
                if (brickLocal[i][j]) DrawRectangleRec(bricks[i][j], RED);

        DrawRectangleRec(hostPad, BLUE);
        DrawRectangleRec(clientPad, ORANGE);
        DrawCircleV(ball, 8, WHITE);

        // 无乱码英文提示
        if (!isConnected)
        {
            DrawText("KEY 1 = Host (A/D - Bottom Pad)", 200, 220, 26, WHITE);
            DrawText("KEY 2 = Client (Left/Right - Top Pad)", 190, 270, 26, WHITE);
        }

        EndDrawing();
    }

    closesocket(udpSock);
    WSACleanup();
    CloseWindow();
    return 0;
}