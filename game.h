#ifndef GAME_S
#define GAME_S

#include "externfuncs.h"
#include <stdio.h>
#include <raylib.h>
#include "spring.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

typedef struct Game {
    unsigned int WinX;
    unsigned int WinY;
    int FPS;
    int TICK;
    double **table0;
    double **table1;
    int shift;
    int BlockSize;
    int XSIZE;
    int YSIZE;
    Obtacle* obts;
    Color BaseColor;
    Color ChangeColor;
    double **table;
    double **prevtable;
    double **temptable;
    double bet;
    double betbet;
    double betdef;
    int pause; int onetick; int tickpause;
    size_t pressed_count;
    char printbuff[64];
    double obsorb;
    int firstupdate;
} Game;


Game* InitGame(int WinX, int WinY, int FPS);
void UpdateGame(Game* game);
void CloseGame(Game* game);

#endif