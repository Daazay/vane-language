#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/collection_common.h"

typedef struct Vector Vector;
typedef struct ItemSpecs VectorItemSpecs;

#define VECTOR_ITEM_SPECS(ITEM_TYPE, DESTROY_FN) ITEM_SPECS(ITEM_TYPE, DESTROY_FN)

struct Vector {
    VectorItemSpecs item_specs;
    byte* raw;
    u32 cap;
    u32 size;
};

// -- Creation --

Vector vector_create(u32 init_cap, VectorItemSpecs item_specs);

void vector_init(Vector* vec, u32 init_cap, VectorItemSpecs item_specs);

// -- Destruction --

void vector_destroy(Vector* vec);

void vector_clear(Vector* vec);

// -- Utilities --

u32 vector_capacity(const Vector* vec);

u32 vector_size(const Vector* vec);

bool is_vector_empty(const Vector* vec);

void vector_resize(Vector* vec, u32 new_cap);

// -- Insertion --

void vector_insert(Vector* vec, u32 idx, const void* items, u32 count);

void vector_push_front(Vector* vec, const void* item);

void vector_push_back(Vector* vec, const void* item);

void vector_extend(Vector* vec, const void* items, u32 count);

// -- Removal --

void vector_remove(Vector* vec, u32 idx);

void vector_pop_front(Vector* vec);

void vector_pop_back(Vector* vec);

// -- Access --

void* vector_at(const Vector* vec, u32 idx);

void* vector_at_front(const Vector* vec);

void* vector_at_back(const Vector* vec);

// -- Modification --

void vector_set(Vector* vec, u32 idx, const void* item);

// -- Search --

i32 vector_index_of(const Vector* vec, const void* item, item_cmp_fn cmp_fn);