#include "vane/utils/vector.h"

#include <stdlib.h>
#include <string.h>

#pragma region UTILITIES

#define VECTOR_DEFAULT_CAPACITY 32
#define VECTOR_CAPACITY_MULT    1.5f

#pragma endregion

Vector vector_create(u32 init_cap, VectorItemSpecs item_specs) {
    Vector vec = { 0 };
    vector_init(&vec, init_cap, item_specs);
    return vec;
}

void vector_init(Vector* vec, u32 init_cap, VectorItemSpecs item_specs) {
    assert(vec != NULL && item_specs.size > 0);

    u32 cap = (init_cap > 0)
        ? init_cap
        : VECTOR_DEFAULT_CAPACITY;

    // DO NOT REMOVE
    if (cap == 1) {
        cap = 2;
    }

    vec->raw = malloc((u64)cap * item_specs.size);
    assert(vec->raw != NULL);

    vec->cap = cap;
    vec->size = 0;
    vec->item_specs = item_specs;
}

void vector_destroy(Vector* vec) {
    if (vec == NULL || vec->raw == NULL) {
        return;
    }

    vector_clear(vec);

    free(vec->raw);

    vec->raw = NULL;
    vec->size = 0;
}

void vector_clear(Vector* vec) {
    assert(vec != NULL);

    if (vec->item_specs.destroy_fn != NULL) {
        for (u32 i = 0; i < vec->size; ++i) {
            void* item = vector_at(vec, i);
            vec->item_specs.destroy_fn(item);
        }
    }

    vec->size = 0;
}

u32 vector_capacity(const Vector* vec) {
    assert(vec != NULL);
    return vec->cap;
}

u32 vector_size(const Vector* vec) {
    assert(vec != NULL);
    return vec->size;
}

bool is_vector_empty(const Vector* vec) {
    assert(vec != NULL);
    return vec->size == 0;
}

void vector_resize(Vector* vec, u32 new_cap) {
    assert(vec != NULL && new_cap > 0);

    if (new_cap == vec->cap) {
        return;
    }
    else if (new_cap < vec->cap && vec->item_specs.destroy_fn != NULL) {
        for (u32 i = 0; i < vec->size; ++i) {
            void* item = vector_at(vec, i);
            vec->item_specs.destroy_fn(item);
        }
        if (vec->size > new_cap) {
            vec->size = new_cap;
        }
    }
    else if (new_cap > vec->cap) {
        byte* raw = realloc(vec->raw, (u64)new_cap * vec->item_specs.size);
        assert(raw != NULL);

        vec->raw = raw;
    }
}

void vector_insert(Vector* vec, u32 idx, const void* items, u32 count) {
    assert(vec != NULL && idx <= vec->size);

    if (count == 0) {
        return;
    }

    assert((items != NULL || vec->item_specs.is_ptr) && "items can be NULL only if item type is ptr");

    u32 needed_size = vec->size + count;
    if (needed_size > vec->cap) {
        u32 new_cap = vec->cap;
        while (needed_size > new_cap) {
            new_cap = (u32)((f32)new_cap * VECTOR_CAPACITY_MULT);
        }
        vector_resize(vec, new_cap);
    }

    byte* insert_pos = vec->raw + idx * vec->item_specs.size;

    if (idx < vec->size) {
        u64 move_bytes = (u64)(vec->size - idx) * vec->item_specs.size;
        memmove(insert_pos + (u64)count * vec->item_specs.size, insert_pos, move_bytes);
    }

    if (items != NULL) {
        memcpy(insert_pos, items, (u64)count * vec->item_specs.size);
    }
    else {
        memset(insert_pos, 0, (u64)count * vec->item_specs.size);
    }

    vec->size += count;
}

void vector_push_front(Vector* vec, const void* item) {
    vector_insert(vec, 0, item, 1);
}

void vector_push_back(Vector* vec, const void* item) {
    vector_insert(vec, vec->size, item, 1);
}

void vector_extend(Vector* vec, const void* items, u32 count) {
    vector_insert(vec, vec->size, items, count);
}

void vector_remove(Vector* vec, u32 idx) {
    assert(vec != NULL && idx < vec->size);

    byte* item = vec->raw + idx * vec->item_specs.size;

    if (vec->item_specs.destroy_fn != NULL) {
        vec->item_specs.destroy_fn(ITEM_SPECS_CAST(vec->item_specs, item));
    }

    if (idx < vec->size - 1) {
        byte* next_item = item + vec->item_specs.size;
        u64 bytes_to_move = (u64)(vec->size - idx - 1) * vec->item_specs.size;
        memmove(item, next_item, bytes_to_move);
    }

    vec->size--;
}

void vector_pop_front(Vector* vec) {
    assert(vec != NULL && vec->size > 0);
    vector_remove(vec, 0);
}

void vector_pop_back(Vector* vec) {
    assert(vec != NULL && vec->size > 0);
    vector_remove(vec, vec->size - 1);
}

void* vector_at(const Vector* vec, u32 idx) {
    assert(vec != NULL && idx < vec->size);

    void* item = vec->raw + (u64)idx * vec->item_specs.size;
    return ITEM_SPECS_CAST(vec->item_specs, item);
}

void* vector_at_front(const Vector* vec) {
    assert(vec != NULL && vec->size > 0);
    return vector_at(vec, 0);
}

void* vector_at_back(const Vector* vec) {
    assert(vec != NULL && vec->size > 0);
    return vector_at(vec, vec->size - 1);
}

void vector_set(Vector* vec, u32 idx, const void* item) {
    assert(vec != NULL && idx < vec->size);
    assert((item != NULL || vec->item_specs.is_ptr) && "item can be NULL only if item type is ptr");

    void* dst = vec->raw + idx * vec->item_specs.size;

    if (vec->item_specs.destroy_fn != NULL) {
        vec->item_specs.destroy_fn(ITEM_SPECS_CAST(vec->item_specs, dst));
    }

    if (item != NULL) {
        memcpy(dst, item, vec->item_specs.size);
    }
    else {
        memset(dst, 0, vec->item_specs.size);
    }
}

i32 vector_index_of(const Vector* vec, const void* item, item_cmp_fn cmp_fn) {
    assert(vec != NULL && item != NULL && cmp_fn != NULL);

    for (u32 i = 0; i < vec->size; ++i) {
        void* item2 = vector_at(vec, i);
        if (!cmp_fn(item2, ITEM_SPECS_CAST(vec->item_specs, item))) {
            return (i32)i;
        }
    }

    return NPOS;
}