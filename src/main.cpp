#include "raylib.h"
typedef enum {MENU,PLAY,PAUSE,OVER}State;
typedef enum {NONE,BIG,FAST,SLOW}PowerUpType;

typedef struct {Vector2 pos; bool active; PowerUpType type;} PowerUp;
typedef struct {Vector2 pos; Color col; bool active;} Particle;

int main() {
    const int w=800,h=600;
    Rectangle p={w/2-50.0f,h-50.0f,100.0f,15.0f};
    Vector2 b={(float)w/2,(float)h/2};
    Vector2 s={4.0f,-4.0f};
    bool bricks[50]={0};
    int life=3,score=0;
    bool god=0;
    State st=MENU;
    PowerUp pu={0};
    Particle parts[30]={0};

    InitWindow(w,h,"Breakout");
    SetTargetFPS(60);
    for(int i=0;i<50;i++)bricks[i]=1;

    while(!WindowShouldClose()){
        if(st==MENU&&IsKeyPressed(KEY_SPACE)){
            st=PLAY;life=3;score=0;god=0;pu.active=0;
            p=(Rectangle){(float)w/2-50,(float)h-50,100,15};
            b=(Vector2){(float)w/2,(float)h/2};
            s=(Vector2){4,-4};
            for(int i=0;i<50;i++)bricks[i]=1;
        }
        else if(st==PLAY){
            if(IsKeyPressed(KEY_P))st=PAUSE;
            if(life>0){
                p.x+=(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT))*6.0f;
                if(p.x<0)p.x=0;
                if(p.x+p.width>w)p.x=(float)w-p.width;
                if(IsKeyPressed(KEY_G))god=!god;

                b.x+=s.x;b.y+=s.y;
                if(b.x<=8||b.x>=w-8)s.x*=-1;
                if(b.y<=8)s.y*=-1;
                if(god&&b.y>=h-8)s.y*=-1;
                else if(b.y>=h-8){
                    life--;
                    b=(Vector2){p.x+p.width/2,p.y-10};
                    s.y=-4;
                }

                if(CheckCollisionCircleRec(b,8,p)&&s.y>0)s.y*=-1;

                for(int i=0;i<50;i++){
                    if(bricks[i]){
                        Rectangle r = {
                            (float)(55 + i%10*80),
                            (float)(60 + i/10*35),
                            70.0f,
                            25.0f
                        };
                        if(CheckCollisionCircleRec(b,8,r)){
                            bricks[i]=0;
                            s.y*=-1;
                            score+=10;

                            if(!pu.active&&GetRandomValue(0,3)==0){
                                pu.pos=(Vector2){r.x+r.width/2,r.y+r.height/2};
                                int randType = GetRandomValue(1,3);
                                pu.type = (PowerUpType)randType;
                                pu.active=1;
                            }

                            for(int j=0;j<30;j++){
                                if(!parts[j].active){
                                    parts[j].pos=(Vector2){r.x+r.width/2,r.y+r.height/2};
                                    parts[j].col=GREEN;
                                    parts[j].active=1;
                                    break;
                                }
                            }
                        }
                    }
                }

                if(pu.active){
                    pu.pos.y+=2.0f;
                    if(CheckCollisionCircleRec(pu.pos,8,p)){
                        if(pu.type==BIG)p.width=140.0f;
                        else if(pu.type==FAST)s.y*=1.2f;
                        else if(pu.type==SLOW)s.y*=0.8f;
                        pu.active=0;
                    }
                }

                bool clear=1;
                for(int i=0;i<50;i++)if(bricks[i])clear=0;
                if(clear){
                    for(int i=0;i<50;i++)bricks[i]=1;
                    score+=1000;
                }
                if(life<=0)st=OVER;
            }
        }
        else if(st==PAUSE&&IsKeyPressed(KEY_P))st=PLAY;
        else if(st==OVER&&IsKeyPressed(KEY_SPACE))st=MENU;

        BeginDrawing();
        ClearBackground(BLACK);
        if(st==MENU){
            DrawText("BRICK BREAKER",w/2-180,h/2-80,50,BLUE);
            DrawText("Press SPACE to start",w/2-150,h/2+20,30,WHITE);
        }else{
            DrawRectangleRec(p,BLUE);
            DrawCircleV(b,8,RED);
            for(int i=0;i<50;i++){
                if(bricks[i]){
                    DrawRectangle(
                        55+i%10*80,
                        60+i/10*35,
                        70,25,
                        GREEN
                    );
                }
            }
            if(pu.active)DrawCircleV(pu.pos,8,ORANGE);
            for(int i=0;i<30;i++){
                if(parts[i].active){
                    DrawPixelV(parts[i].pos,parts[i].col);
                    parts[i].pos.y+=5;
                }
            }
            DrawText(TextFormat("Score:%d Life:%d",score,life),10,10,20,WHITE);
            if(god)DrawText("GOD MODE",w/2-80,30,25,ORANGE);
            if(st==PAUSE)DrawText("PAUSED",w/2-80,h/2-20,40,YELLOW);
            if(st==OVER){
                DrawText("GAME OVER",w/2-100,h/2-40,40,RED);
                DrawText("SPACE to menu",w/2-120,h/2+40,25,WHITE);
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}