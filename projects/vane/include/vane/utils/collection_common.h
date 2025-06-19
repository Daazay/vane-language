#pragma once

#include <vane/utils/defines.h>

typedef struct ItemSpecs ItemSpecs;

typedef void(*item_destroy_fn)(void* item);
typedef i32(*item_cmp_fn)(const void* item1, const void* item2);
typedef bool(*item_eq_fn)(const void* item1, const void* item2);
typedef u32(*item_hash_fn)(const void* item);

#define IS_TYPE_PTR(TYPE) ((#TYPE)[sizeof(#TYPE) / sizeof(char) - 2] == '*')

struct ItemSpecs {
    u32 size;
    bool is_ptr;
    item_destroy_fn destroy_fn;
};

#define ITEM_SPECS(ITEM_TYPE, DESTROY_FN) ((ItemSpecs) { \
    .size       = sizeof(ITEM_TYPE), \
    .is_ptr     = IS_TYPE_PTR(ITEM_TYPE), \
    .destroy_fn = (item_destroy_fn)DESTROY_FN, \
})

#define ITEM_SPECS_CAST(ITEM_SPECS, ITEM) (((ITEM_SPECS).is_ptr) ? *(void**)ITEM : ITEM)