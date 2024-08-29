#include <stdio.h>
#include <raylib.h>
#include "memmanager.h"
#include "vector.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <libloaderapi.h>
#include <errhandlingapi.h>


typedef void Game;
const unsigned int WinX = 800;
const unsigned int WinY = 600;
const int FPS = 20;
int TICK = 0;

typedef void *(memloc_t)(size_t size);
typedef void (memfree_t)(void *ptr);
typedef vector (new_vec_t)(size_t elem_size, size_t prealloc);
typedef void (delete_vec_t)(vector vec);
typedef vector_metainfo (vec_meta_t)(vector vec);
typedef vector (vec_add_t)(vector vec, void* elem);
typedef void (vec_remove_t)(vector vec, size_t index);

typedef void (load_extern_funcs_t)(memloc_t*,memfree_t*,new_vec_t*,delete_vec_t*,vec_meta_t*,vec_add_t*,vec_remove_t*);
load_extern_funcs_t *load_extern_funcs = NULL;

Game* game = NULL;
typedef Game* (InitGame_t)(int WinX, int WinY, int FPS);
typedef void (UpdateGame_t)(Game* game);
typedef void (CloseGame_t)(Game* game);

InitGame_t *InitGame = NULL;
UpdateGame_t *UpdateGame = NULL;
CloseGame_t *CloseGame = NULL;


HMODULE gamelib = NULL;
int shift = 0;
bool LoadGameLibrary() {
    if (gamelib!=NULL) FreeLibrary(gamelib);
    if (shift==0) gamelib = LoadLibraryA("game0.dll");
    else if (shift==1) gamelib = LoadLibraryA("game1.dll");
    else return false;
    shift = (shift+1)%2;
    if (gamelib==NULL) return false;

    load_extern_funcs = (void*)GetProcAddress(gamelib, "load_extern_funcs");
    if (load_extern_funcs==NULL) return false;

    InitGame =(void*) GetProcAddress(gamelib, "InitGame");
    if (InitGame==NULL) return false;
    
    UpdateGame = (void*)GetProcAddress(gamelib, "UpdateGame");
    if (UpdateGame==NULL) return false;

    CloseGame = (void*)GetProcAddress(gamelib, "CloseGame");
    if (CloseGame==NULL) return false;

    return true;
}

bool LoadGame() {
    if (!LoadGameLibrary()) {
        return false;
    }
    load_extern_funcs(memloc, memfree, new_vec, delete_vec, vec_meta, vec_add, vec_remove);
    return true;
}



int main() {
    if (!LoadGame()) {
        printf("Cant load lib: %d\n", GetLastError());
        goto GAME_ABS_EXIT;
    }
    srand(time(NULL));
    prealloc(7708832);
    InitWindow(WinX, WinY, "Springs");
    SetTargetFPS(FPS);
    
    game = InitGame(WinX, WinY, FPS);
    while (!WindowShouldClose()) {
        TICK++;
        TICK = TICK%FPS;

        BeginDrawing();
        if (IsKeyPressed(KEY_RIGHT_ALT) && TICK%2==0) {
            printf("Reloading...\n");
            if (!LoadGame()) {
                printf("Cant load lib: %d\n", GetLastError());
                goto GAME_ABS_EXIT;
            }
        }
        UpdateGame(game);
        EndDrawing();
    }

GAME_EXIT:
    CloseWindow();
    CloseGame(game);
    
GAME_ABS_EXIT:
    page_info(0);
    destroy_pages();
    return 0;
}
