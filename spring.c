#include "spring.h"

double** create_table(int x, int y) {
    double** table = memloc(sizeof(double*)*y);
    for (int iy=0; iy<y; iy++) {
        table[iy] = memloc(sizeof(double)*x);
        for (int ix=0; ix<x; ix++) {
            table[iy][ix] = 0;
        }
    }
    return table;
}

void delete_table(int x, int y, double** table) {
    for (int iy=0; iy<y; iy++) {
        memfree(table[iy]);
    }
    memfree(table);
}

double abs_d(double d) {
    if (d<0) return -d;
    return d;
}

int sign(double d) {
    if (d<0) return -1;
    return 1;
}

void update_table(int x, int y, double** prevtable, double** newtable, Obtacle* obts) {
    for (int iy=0; iy<y; iy++) {
        for (int ix=0; ix<x; ix++) {
            int idx = intersect_obts(ix, iy, obts);
            double obsorb = idx<0 ? 0 : obts[idx].obsorb;
            // if (obsorb==1) continue;
            double temp = 0;//prevtable[iy][ix] /2;

            temp += prevtable[(iy+1)%y][ix]; // temp.y += prevtable[(iy+1)%y][ix].y;
            temp += prevtable[(iy-1)%y][ix]; // temp.y += prevtable[(iy-1)%y][ix].y;
            temp += prevtable[iy][(ix+1)%x]; // temp.y += prevtable[iy][(ix+1)%x].y;
            temp += prevtable[iy][(ix-1)%x]; // temp.y += prevtable[iy][(ix-1)%x].y;

            //temp.x = temp.x/5; temp.y = temp.y/5;
            temp = (1-obsorb)*temp/4;
            if (abs_d(temp)> MAXIMUM) {
                temp = MAXIMUM/2*sign(temp);
            }

            newtable[iy][ix] = temp;
        }
    }
}

bool intersect_obt(int x, int y, Obtacle* obt) {
    if (x >= obt->pos.x && x < obt->pos.x + obt->size.x
    && y >= obt->pos.y && y < obt->pos.y + obt->size.y) return true;
    return false;
}

int intersect_obts(int x, int y, Obtacle* obts) {
    vector_metainfo meta = vec_meta(obts);
    for (int i=0; i<meta.length; i++) {
        if (intersect_obt(x, y, obts+i)) return i;
    }
    return -1;
}

void clear_table(int x, int y, double** table, double v) {
    for (int iy=0; iy<y; iy++) {
        for (int ix=0; ix<x; ix++) {
            table[iy][ix] = v;
        }
    }
}