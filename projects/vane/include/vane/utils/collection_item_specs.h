#pragma once

#include <vane/utils/defines.h>

typedef struct CollectionItemSpecs CollectionItemSpecs;
typedef void(*item_destroy_fn)(void* item);

struct CollectionItemSpecs {
    u32 size;
    bool is_ptr;
    item_destroy_fn destroy_fn;
};

#define COLLECTION_ITEM_SPECS(TYPE, DESTROY_FN) ((CollectionItemSpecs) { \
    .size       = sizeof(TYPE), \
    .is_ptr     = IS_TYPE_PTR(TYPE), \
    .destroy_fn = (item_destroy_fn)DESTROY_FN, \
})

#define COLLECTION_ITEM_CAST(ITEM_SPECS, ITEM) ((ITEM_SPECS.is_ptr) ? *(void**)ITEM : ITEM)