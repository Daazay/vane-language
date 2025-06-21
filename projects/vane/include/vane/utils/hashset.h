#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/collection_common.h"

typedef struct Hashset Hashset;
typedef struct Vector HashsetBucket;
typedef struct HashItemSpecs HashsetItemSpecs;
typedef struct HashsetIterator HashsetIterator;

#define HASHSET_ITEM_SPECS(ITEM_TYPE, HASH_FN, EQ_FN, DESTROY_FN) HASH_ITEM_SPECS(ITEM_TYPE, HASH_FN, EQ_FN, DESTROY_FN)

struct HashsetIterator {
    const Hashset* set;
    u32 next_bucket_idx;
    u32 next_entry_idx;

    const void* item;
};

struct Hashset {
    HashsetItemSpecs item_specs;
    HashsetBucket* buckets;
    u32 cap;
    u32 size;
};

// -- Creation --

Hashset hashset_create(u32 init_cap, HashsetItemSpecs item_specs);

void hashset_init(Hashset* set, u32 init_cap, HashsetItemSpecs item_specs);

// -- Destruction --

void hashset_destroy(Hashset* set);

void hashset_clean(Hashset* set);

// -- Utilities --

u32 hashset_capacity(const Hashset* set);

u32 hashset_size(const Hashset* set);

f64 hashset_load_factor(const Hashset* set);

bool is_hashset_empty(const Hashset* set);

void hashset_resize(Hashset* set, u32 new_cap);

// -- Insertion --

void hashset_put(Hashset* set, const void* item);

// -- Retrieval --

const void* hashset_at(const Hashset* set, const void* item);

bool hashset_contains(const Hashset* set, const void* item);

// -- Removal --

bool hashset_remove(Hashset* set, const void* item);

// -- Iterator --

HashsetIterator hashset_get_it(const Hashset* set);

bool hashset_it_next(HashsetIterator* it);