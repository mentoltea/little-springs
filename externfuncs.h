#ifndef EXTERN_FUNCS_S
#define EXTERN_FUNCS_S

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// #include "memmanager.h"
typedef void *(memloc_t)(size_t size);
typedef void (memfree_t)(void *ptr);

// #include "vector.h"
typedef void* vector;
typedef struct {
    size_t length; // number of elements
    size_t capacity; // capacity in number of elemets
    size_t size; // size of one element
} vector_metainfo;
typedef vector (new_vec_t)(size_t elem_size, size_t prealloc);
typedef void (delete_vec_t)(vector vec);
typedef vector_metainfo (vec_meta_t)(vector vec);
typedef vector (vec_add_t)(vector vec, void* elem);
typedef void (vec_remove_t)(vector vec, size_t index);


extern memloc_t* memloc;
extern memfree_t* memfree;
extern new_vec_t* new_vec;
extern delete_vec_t* delete_vec;
extern vec_meta_t* vec_meta;
extern vec_add_t* vec_add;
extern vec_remove_t* vec_remove;

void load_extern_funcs(memloc_t*,memfree_t*,new_vec_t*,delete_vec_t*,vec_meta_t*,vec_add_t*,vec_remove_t*);
void load_raylib();
/*

*/

#endif