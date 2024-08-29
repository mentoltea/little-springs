#include "externfuncs.h"

memloc_t* memloc = NULL;
memfree_t* memfree = NULL;
new_vec_t* new_vec = NULL;
delete_vec_t* delete_vec = NULL;
vec_meta_t* vec_meta = NULL;
vec_add_t* vec_add = NULL;
vec_remove_t* vec_remove = NULL;

void load_extern_funcs(memloc_t* m1,memfree_t* f1,new_vec_t* nv1,delete_vec_t* dv1,vec_meta_t* vm1,vec_add_t* va1,vec_remove_t* vr1) {
    memloc = m1;
    memfree = f1;
    new_vec = nv1;
    delete_vec = dv1;
    vec_meta = vm1;
    vec_add = va1;
    vec_remove = vr1;
}