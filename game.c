#include "game.h"

Game* InitGame(int WinX, int WinY, int FPS) {
    Game* game = memloc(sizeof(Game));

    game->WinX = WinX;
    game->WinY = WinY;
    game->FPS = FPS;
    game->TICK = 0;
    game->shift = 0;
    game->BlockSize = 8;
    game->XSIZE = WinX/game->BlockSize;
    game->YSIZE = WinY/game->BlockSize;

    game->table0 = create_table(game->XSIZE, game->YSIZE);
    game->table1 = create_table(game->XSIZE, game->YSIZE);
     //0=table 1=prevtable

    game->obts = new_vec(sizeof(Obtacle), 10);
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
                game->obts = vec_add(game->obts, &obt);
            }
        }
        }
    game->BaseColor = GRAY;
    
    {
        double len = sqrt(game->BaseColor.r*game->BaseColor.r + game->BaseColor.g*game->BaseColor.g + game->BaseColor.b*game->BaseColor.b);
        game->ChangeColor.r = -((double)game->BaseColor.r*2/len);
        game->ChangeColor.g = -((double)game->BaseColor.g*2/len);
        game->ChangeColor.b = -((double)game->BaseColor.b*2/len);
    }
    game->table = game->table1;
    game->prevtable = game->table0;
    game->bet = 2;
    game->betbet = 0.1;
    game->betdef = 0.1;
    game->pause = 0;
    game->onetick = 0;
    game->tickpause = 0;
    game->pressed_count = 0;
    game->obsorb = 2;
    game->firstupdate = 0;

    return game;
}


void UpdateGame(Game* game) {
        game->TICK = (game->TICK+1)%game->FPS;
        
        if (!game->pause || game->onetick){    
        switch (game->shift) {
            case 0:
                game->table = game->table0;
                game->prevtable = game->table1;
                break;
            case 1:
                game->table = game->table1;
                game->prevtable = game->table0;
                break;
            default:
                printf("shift is not in range %d\n", game->shift);
                CloseWindow();
                break;
            }
        } 
        
        if (IsKeyPressed(KEY_SPACE)) {
            game->pause = (game->pause+1)%2;
            game->tickpause = 0;
            if (game->pause) {
                game->temptable = game->table;
                game->table = game->prevtable;
            } else {
                game->table = game->temptable;
            }
        }

        if (game->onetick) {
            game->onetick=0;
            game->temptable = game->table;
            game->table = game->prevtable;
        }
        if (game->pause && IsKeyDown(KEY_F) && game->TICK%2==0) {
            game->onetick = 1;
            game->tickpause++;
            game->table = game->temptable;
        }
        
        if (IsKeyDown(KEY_EQUAL)) {
            game->bet+=game->betbet;
            game->pressed_count++;
            if (game->pressed_count >= game->FPS*1.5) game->betbet+=game->betdef;
        } else if (IsKeyDown(KEY_MINUS)) {
            game->bet-=game->betbet;
            game->pressed_count++;
            if (game->pressed_count >= game->FPS*1.5) game->betbet+=game->betdef;
        } else {
            game->pressed_count = 0;
            game->betbet = game->betdef;
        }

        if (IsKeyDown(KEY_COMMA)) {
            game->obsorb += 0.02;
        } else if (IsKeyDown(KEY_PERIOD)) {
            game->obsorb -= 0.02;
        }

        if ((IsMouseButtonDown(MOUSE_BUTTON_EXTRA) || IsKeyDown(KEY_UP)) && game->TICK%3==0) {
            Vector2 mouse = GetMousePosition();
            Obtacle obt;
            obt.pos.x = (int)mouse.x/game->BlockSize;
            obt.pos.y = (int)mouse.y/game->BlockSize;
            obt.size = (Vector2){10,10};
            obt.obsorb = 1;
            game->obts = vec_add(game->obts, &obt);
        } else if ((IsMouseButtonDown(MOUSE_BUTTON_SIDE) || IsKeyDown(KEY_DOWN)) && game->TICK%3==0) {
            Vector2 mouse = GetMousePosition();
            Obtacle obt;
            obt.pos.x = (int)mouse.x/game->BlockSize;
            obt.pos.y = (int)mouse.y/game->BlockSize;
            obt.size = (Vector2){10,10};
            obt.obsorb = game->obsorb;
            game->obts = vec_add(game->obts, &obt);
        }

        if (IsKeyPressed(KEY_Z)) {
            vector_metainfo meta = vec_meta(game->obts);
            vec_remove(game->obts, meta.length-1);
        }
        if (IsKeyPressed(KEY_D)) {
            Vector2 mouse = GetMousePosition();
            int idx = intersect_obts((int)mouse.x/game->BlockSize, (int)mouse.y/game->BlockSize, game->obts);
            if (idx!=-1) vec_remove(game->obts, idx);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse = GetMousePosition();
            int mousex = mouse.x;
            int mousey = mouse.y;
            if (mousex >= 0 && mousex < game->WinX &&
            mousey >= 0 && mousey < game->WinY) {
                double set;
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) set = game->bet;
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) set = -game->bet;
                game->prevtable[mousey/game->BlockSize][mousex/game->BlockSize] = set;
                game->table[mousey/game->BlockSize][mousex/game->BlockSize] = set;
                // prevtable[(mousey/BlockSize + 1)%YSIZE][mousex/BlockSize] += set/4;
                // prevtable[(mousey/BlockSize - 1)%YSIZE][mousex/BlockSize] += set/4;
                // prevtable[mousey/BlockSize][(mousex/BlockSize + 1)%XSIZE] += set/4;
                // prevtable[mousey/BlockSize][(mousex/BlockSize - 1)%XSIZE] += set/4;
                // printf("%lf\n", set);
            }
        }



        if (IsKeyPressed(KEY_R)) {
            clear_table(game->XSIZE, game->YSIZE, game->table0, 0);
            clear_table(game->XSIZE, game->YSIZE, game->table1, 0);
        }
        
        if (!game->pause || game->onetick) update_table(game->XSIZE, game->YSIZE, game->prevtable, game->table, game->obts);
        
        Color col = game->BaseColor;
        game->BaseColor.r = 120;
        int r,g,b;
        double temp;
        for (int y=0; y<game->YSIZE; y++) {
            for (int x=0; x<game->XSIZE; x++) {
                temp = game->table[y][x];
                
                if (game->firstupdate && (!game->pause || game->onetick) && game->prevtable[y][x] == temp) {
                    // DrawRectangle(x*game->BlockSize, y*game->BlockSize, game->BlockSize, game->BlockSize, game->BaseColor);
                    game->firstupdate = 1;
                    continue;
                }
                temp *= 100;
                
                r = game->BaseColor.r; g = game->BaseColor.g; b = game->BaseColor.b;
                r += temp*game->ChangeColor.r;
                g += temp*game->ChangeColor.g;
                b += temp*game->ChangeColor.b;
                r = r>255 ? 255 : (r < 0 ? 0 : r);
                g = g>255 ? 255 : (g < 0 ? 0 : g);
                b = b>255 ? 255 : (b < 0 ? 0 : b);

                col.r = (unsigned char)r; col.g = (unsigned char)g; col.b = (unsigned char)b;
                col.r = 120;
                DrawRectangle(x*game->BlockSize, y*game->BlockSize, game->BlockSize, game->BlockSize, col);
            }
        }

        {
            vector_metainfo meta = vec_meta(game->obts);
            for (int i=0; i<meta.length; i++) {
                DrawRectangleLines(game->obts[i].pos.x*game->BlockSize, game->obts[i].pos.y*game->BlockSize,
                 game->obts[i].size.x*game->BlockSize, game->obts[i].size.y*game->BlockSize,
                 game->obts[i].obsorb<0 ? RED : (game->obts[i].obsorb>1 ? BLUE : BLACK));
            }
        }

        // for (int x=0; x<XSIZE; x++) {
        //     DrawLine(x*BlockSize, 0, x*BlockSize, WinY, BLACK);
        // }
        // for (int y=0; y<YSIZE; y++) {
        //     DrawLine(0, y*BlockSize, WinX, y*BlockSize, BLACK);
        // }

        snprintf(game->printbuff, 64, "%.1lf", game->bet);
        DrawText(game->printbuff, 0, 20, 30, BLACK);
        snprintf(game->printbuff, 64, "%.2lf", game->obsorb);
        DrawText(game->printbuff, 0, 50, 30, BLACK);
        if (game->pause) {
            snprintf(game->printbuff, 64, "PAUSED", game->bet);
            DrawText(game->printbuff, 0, 80, 30, BLACK);
            snprintf(game->printbuff, 64, "%d", game->tickpause);
            if (game->onetick) snprintf(game->printbuff+strlen(game->printbuff), 64-strlen(game->printbuff), " ->");
            DrawText(game->printbuff, 0, 110, 30, BLACK);
        }
        DrawFPS(0,0);

        if (!game->pause || game->onetick) game->shift = (game->shift+1)%2;
}


void CloseGame(Game* game) {
    delete_table(game->XSIZE, game->YSIZE, game->table);
    delete_table(game->XSIZE, game->YSIZE, game->prevtable);
    delete_vec(game->obts);
    memfree(game);
}