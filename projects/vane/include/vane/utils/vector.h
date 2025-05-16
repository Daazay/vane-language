#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/collection_item_specs.h"

typedef CollectionItemSpecs VectorItemSpecs;
typedef struct Vector Vector;

struct Vector {
    byte* raw;
    u32 cap;
    u32 size;

    VectorItemSpecs item_specs;
};

Vector vector_create(u32 init_cap, VectorItemSpecs item_specs);

void vector_init(Vector* vector, u32 init_cap, VectorItemSpecs item_specs);

void vector_destroy(Vector* vector);

void vector_resize(Vector* vector, u32 new_cap);

void vector_push_front(Vector* vector, const void* item);

void vector_push_back(Vector* vector, const void* item);

void vector_insert(Vector* vector, u32 idx, const void* items, u32 count);

void vector_remove(Vector* vector, u32 idx);

void* vector_at(const Vector* vector, u32 idx);

void* vector_at_front(const Vector* vector);

void* vector_at_back(const Vector* vector);