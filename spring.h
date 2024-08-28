#ifndef SPRINGS_S
#define SPRINGS_S

#include <raylib.h>
#include "memmanager.h"
#include "vector.h"

#define MAXIMUM 10000

typedef struct Obtacle {
    Vector2 pos;
    Vector2 size;
    double obsorb;
} Obtacle;


// @return table[y][x]
double** create_table(int x, int y);
void delete_table(int x, int y, double** table);
void clear_table(int x, int y, double** table, double v);

bool intersect_obt(int x, int y, Obtacle* obt);
// 0 - 1
int intersect_obts(int x, int y, Obtacle* obts);

void update_table(int x, int y, double** prevtable, double** newtable, Obtacle* obts);

#endif