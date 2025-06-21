#pragma once

#include <vane/utils/defines.h>

typedef struct ItemSpecs ItemSpecs;
typedef struct HashItemSpecs HashItemSpecs;

typedef void(*item_destroy_fn)(void* item);
typedef i32(*item_cmp_fn)(const void* item1, const void* item2);
typedef bool(*item_eq_fn)(const void* item1, const void* item2);
typedef u32(*item_hash_fn)(const void* item);

#define IS_TYPE_PTR(TYPE) ((#TYPE)[sizeof(#TYPE) / sizeof(char) - 2] == '*')

#define ITEM_SPECS_CAST(ITEM_SPECS, ITEM) (((ITEM_SPECS).is_ptr) ? *(void**)ITEM : ITEM)

#define ITEM_SPECS(ITEM_TYPE, DESTROY_FN) ((ItemSpecs) { \
    .size       = sizeof(ITEM_TYPE), \
    .is_ptr     = IS_TYPE_PTR(ITEM_TYPE), \
    .destroy_fn = (item_destroy_fn)DESTROY_FN, \
})

#define HASH_ITEM_SPECS(ITEM_TYPE, HASH_FN, EQ_FN, DESTROY_FN) ((HashItemSpecs) { \
    .common  = ITEM_SPECS(ITEM_TYPE, DESTROY_FN), \
    .hash_fn = (item_hash_fn)HASH_FN, \
    .eq_fn   = (item_eq_fn)EQ_FN \
})

struct ItemSpecs {
    u32 size;
    bool is_ptr;
    item_destroy_fn destroy_fn;
};

struct HashItemSpecs {
    ItemSpecs common;
    item_hash_fn hash_fn;
    item_eq_fn eq_fn;
};