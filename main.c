#include <stdio.h>
#include <raylib.h>
#include "memmanager.h"
#include "vector.h"
#include "spring.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

const unsigned int WinX = 800;
const unsigned int WinY = 600;
const int FPS = 20;
int TICK = 0;

const int BlockSize = 8;

int main() {
    srand(time(NULL));
    prealloc(7708832);
    int XSIZE = WinX/BlockSize;
    int YSIZE = WinY/BlockSize;

    double** table0 = create_table(XSIZE, YSIZE);
    double** table1 = create_table(XSIZE, YSIZE);
    int shift = 0; //0=table 1=prevtable

    Obtacle* obts = new_vec(sizeof(Obtacle), 10);
    {
    Obtacle obt;
    int size = 10;
    int pad = 8;
    int num = 6;
    obt.size.x = size; obt.size.y = size;
    for (int x=0; x<num; x++) {
        for (int y=0; y<num; y++) {
            obt.pos.x = pad + (size+pad)*x; obt.pos.y = pad + (size+pad)*y;
            obt.obsorb = 1;//rand()%5 * 0.25f;
            obts = vec_add(obts, &obt);
        }
    }
    }

    InitWindow(WinX, WinY, "Springs");
    SetTargetFPS(FPS);
    Color BaseColor = GRAY;
    Color ChangeColor;// = {1, 1, 1, 255};
    {
        double len = sqrt(BaseColor.r*BaseColor.r + BaseColor.g*BaseColor.g + BaseColor.b*BaseColor.b);
        ChangeColor.r = -((double)BaseColor.r*2/len);
        ChangeColor.g = -((double)BaseColor.g*2/len);
        ChangeColor.b = -((double)BaseColor.b*2/len);
    }
    double **table = table1, **prevtable = table0, **temptable;
    double bet = 200;
    double betbet = 1, betdef = 1;
    int pause = 0, onetick = 0, tickpause = 0;
    size_t pressed_count;
    char printbuff[64];
    double obsorb = 2;
    while (!WindowShouldClose()) {
        TICK = (TICK+1)%FPS;

        if (!pause || onetick){    
        switch (shift) {
            case 0:
                table = table0;
                prevtable = table1;
                break;
            case 1:
                table = table1;
                prevtable = table0;
                break;
            default:
                printf("shift is not in range %d\n", shift);
                CloseWindow();
                break;
            }
        } 

        if (IsKeyPressed(KEY_SPACE)) {
            pause = (pause+1)%2;
            tickpause = 0;
            if (pause) {
                temptable = table;
                table = prevtable;
            } else {
                table = temptable;
            }
        }

        if (onetick) {
            onetick=0;
            temptable = table;
            table = prevtable;
        }
        if (pause && IsKeyDown(KEY_F) && TICK%2==0) {
            onetick = 1;
            tickpause++;
            table = temptable;
        }

        if (IsKeyDown(KEY_EQUAL)) {
            bet+=betbet;
            pressed_count++;
            if (pressed_count >= FPS*1.5) betbet+=1;
        } else if (IsKeyDown(KEY_MINUS)) {
            bet-=betbet;
            pressed_count++;
            if (pressed_count >= FPS*1.5) betbet+=1;
        } else {
            pressed_count = 0;
            betbet = betdef;
        }

        if (IsKeyDown(KEY_COMMA)) {
            obsorb += 0.02;
        } else if (IsKeyDown(KEY_PERIOD)) {
            obsorb -= 0.02;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_EXTRA) && TICK%3==0) {
            Vector2 mouse = GetMousePosition();
            Obtacle obt;
            obt.pos.x = (int)mouse.x/BlockSize;
            obt.pos.y = (int)mouse.y/BlockSize;
            obt.size = (Vector2){10,10};
            obt.obsorb = 1;
            obts = vec_add(obts, &obt);
        } else if (IsMouseButtonDown(MOUSE_BUTTON_SIDE) && TICK%3==0) {
            Vector2 mouse = GetMousePosition();
            Obtacle obt;
            obt.pos.x = (int)mouse.x/BlockSize;
            obt.pos.y = (int)mouse.y/BlockSize;
            obt.size = (Vector2){10,10};
            obt.obsorb = obsorb;
            obts = vec_add(obts, &obt);
        }

        if (IsKeyPressed(KEY_Z)) {
            vector_metainfo meta = vec_meta(obts);
            vec_remove(obts, meta.length-1);
        }
        if (IsKeyPressed(KEY_D)) {
            Vector2 mouse = GetMousePosition();
            int idx = intersect_obts((int)mouse.x/BlockSize, (int)mouse.y/BlockSize, obts);
            if (idx!=-1) vec_remove(obts, idx);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse = GetMousePosition();
            int mousex = mouse.x;
            int mousey = mouse.y;
            if (mousex >= 0 && mousex < WinX &&
            mousey >= 0 && mousey < WinY) {
                double set;
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) set = bet;
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) set = -bet;
                prevtable[mousey/BlockSize][mousex/BlockSize] = set;
                table[mousey/BlockSize][mousex/BlockSize] = set;
                // prevtable[(mousey/BlockSize + 1)%YSIZE][mousex/BlockSize] += set/4;
                // prevtable[(mousey/BlockSize - 1)%YSIZE][mousex/BlockSize] += set/4;
                // prevtable[mousey/BlockSize][(mousex/BlockSize + 1)%XSIZE] += set/4;
                // prevtable[mousey/BlockSize][(mousex/BlockSize - 1)%XSIZE] += set/4;
                // printf("%lf\n", set);
            }
        }



        if (IsKeyPressed(KEY_R)) {
            clear_table(XSIZE, YSIZE, table0, 0);
            clear_table(XSIZE, YSIZE, table1, 0);
        }

        if (!pause || onetick) update_table(XSIZE, YSIZE, prevtable, table, obts);

        Color col = BaseColor;
        int r,g,b;
        double temp;
        BeginDrawing(); 
        for (int y=0; y<YSIZE; y++) {
            for (int x=0; x<XSIZE; x++) {
                temp = table[y][x];
                if ((!pause || onetick) && prevtable[y][x] == temp) {
                    DrawRectangle(x*BlockSize, y*BlockSize, BlockSize, BlockSize, BaseColor);
                    continue;
                }
                temp *= 100;

                r = BaseColor.r; g = BaseColor.g; b = BaseColor.b;
                r += temp*ChangeColor.r;
                g += temp*ChangeColor.g;
                b += temp*ChangeColor.b;
                r = r>255 ? 255 : (r < 0 ? 0 : r);
                g = g>255 ? 255 : (g < 0 ? 0 : g);
                b = b>255 ? 255 : (b < 0 ? 0 : b);

                col.r = (unsigned char)r; col.g = (unsigned char)g; col.b = (unsigned char)b;

                DrawRectangle(x*BlockSize, y*BlockSize, BlockSize, BlockSize, col);
            }
        }
        
        {
            vector_metainfo meta = vec_meta(obts);
            for (int i=0; i<meta.length; i++) {
                DrawRectangleLines(obts[i].pos.x*BlockSize, obts[i].pos.y*BlockSize, obts[i].size.x*BlockSize, obts[i].size.y*BlockSize,
                 obts[i].obsorb<0 ? RED : (obts[i].obsorb>1 ? BLUE : BLACK));
            }
        }

        // for (int x=0; x<XSIZE; x++) {
        //     DrawLine(x*BlockSize, 0, x*BlockSize, WinY, BLACK);
        // }
        // for (int y=0; y<YSIZE; y++) {
        //     DrawLine(0, y*BlockSize, WinX, y*BlockSize, BLACK);
        // }

        snprintf(printbuff, 64, "%.0lf", bet);
        DrawText(printbuff, 0, 20, 30, BLACK);
        snprintf(printbuff, 64, "%.2lf", obsorb);
        DrawText(printbuff, 0, 50, 30, BLACK);
        if (pause) {
            snprintf(printbuff, 64, "PAUSED", bet);
            DrawText(printbuff, 0, 80, 30, BLACK);
            snprintf(printbuff, 64, "%d", tickpause);
            if (onetick) snprintf(printbuff+strlen(printbuff), 64-strlen(printbuff), " ->");
            DrawText(printbuff, 0, 110, 30, BLACK);
        }
        DrawFPS(0,0);
        
        EndDrawing();

        if (!pause || onetick) shift = (shift+1)%2;
    }
    
    CloseWindow();


    delete_table(XSIZE, YSIZE, table);
    delete_table(XSIZE, YSIZE, prevtable);
    delete_vec(obts);

    page_info(0);
    destroy_pages();
    return 0;
}