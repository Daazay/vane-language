#include "vane/utils/vector.h"

#include <stdlib.h>
#include <string.h>

#pragma region UTILITIES

#define VECTOR_DEFAULT_CAPACITY 32
#define VECTOR_CAPACITY_MULT    1.5

#pragma endregion

Vector vector_create(u32 init_cap, VectorItemSpecs item_specs) {
    Vector vector = { 0 };
    vector_init(&vector, init_cap, item_specs);

    return vector;
}

void vector_init(Vector* vector, u32 init_cap, VectorItemSpecs item_specs) {
    assert(vector != NULL && item_specs.size > 0);

    u32 cap = init_cap > 0
        ? init_cap
        : VECTOR_DEFAULT_CAPACITY;

    byte* raw = malloc((u64)cap * item_specs.size);
    assert(raw != NULL);

    *vector = (Vector) {
        .raw = raw,
        .cap = cap,
        .size = 0,
        .item_specs = item_specs,
    };
}

void vector_destroy(Vector* vector) {
    if (vector == NULL || vector->raw == NULL) {
        return;
    }

    if (vector->item_specs.destroy_fn != NULL) {
        for (u32 i = 0; i < vector->size; ++i) {
            void* item = vector_at(vector, i);
            vector->item_specs.destroy_fn(item);
        }
    }

    free(vector->raw);

    vector->raw = NULL;
    vector->size = 0;
}

void vector_resize(Vector* vector, u32 new_cap) {
    assert(vector != NULL && vector->raw != NULL && new_cap > 0);

    if (new_cap == vector->cap) {
        return;
    }
    else if (new_cap < vector->cap && vector->item_specs.destroy_fn != NULL) {
        for (u32 i = new_cap; i < vector->size; ++i) {
            void* item = vector_at(vector, i);
            vector->item_specs.destroy_fn(item);
        }
        vector->size = new_cap;
    }
    else if (new_cap > vector->cap) {
        byte* raw = realloc(vector->raw, (u64)new_cap * vector->item_specs.size);
        assert(raw != NULL);

        vector->raw = raw;
    }

    vector->cap = new_cap;
}

void vector_push_front(Vector* vector, const void* item) {
    assert(vector != NULL && vector->raw != NULL && item != NULL);

    vector_insert(vector, 0, item, 1);
}

void vector_push_back(Vector* vector, const void* item) {
    assert(vector != NULL && vector->raw != NULL && item != NULL);

    vector_insert(vector, vector->size, item, 1);
}

void vector_insert(Vector* vector, u32 idx, const void* items, u32 count) {
    assert(vector != NULL && vector->raw != NULL && idx <= vector->size && items != NULL);

    if (count == 0) {
        return;
    }

    if (vector->size + count > vector->cap) {
        u32 new_cap = vector->cap;
        while (vector->size + count > new_cap) {
            new_cap = (u32)((f32)new_cap * VECTOR_CAPACITY_MULT);
        }
        vector_resize(vector, new_cap);
    }

    byte* ins_p = vector->raw + idx * vector->item_specs.size;
    if (idx < vector->size && vector->size > 0) {
        u64 move_size = (u64)(vector->size - idx) * vector->item_specs.size;
        byte* where = vector->raw + (idx + count) * vector->item_specs.size;
        memmove(where, ins_p, move_size);
    }

    memcpy(ins_p, items, (u64)count * vector->item_specs.size);
    vector->size = vector->size + count;
}

void vector_remove(Vector* vector, u32 idx) {
    assert(vector != NULL && vector->raw != NULL && idx < vector->size);

    byte* item = vector->raw + idx * vector->item_specs.size;
    if (vector->item_specs.destroy_fn != NULL) {
        vector->item_specs.destroy_fn(COLLECTION_ITEM_CAST(vector->item_specs, item));
    }
    if (vector->size > 1) {
        const byte* last = vector->raw + (vector->size - 1) * vector->item_specs.size;
        memcpy(item, last, vector->item_specs.size);
    }

    vector->size--;
}

void* vector_at(const Vector* vector, u32 idx) {
    assert(vector != NULL && vector->raw != NULL && idx < vector->size);

    void* item = vector->raw + (u64)idx * vector->item_specs.size;
    return COLLECTION_ITEM_CAST(vector->item_specs, item);
}

void* vector_at_front(const Vector* vector) {
    assert(vector != NULL && vector->size > 0);

    return vector_at(vector, 0);
}

void* vector_at_back(const Vector* vector) {
    assert(vector != NULL && vector->size > 0);

    return vector_at(vector, vector->size - 1);
}