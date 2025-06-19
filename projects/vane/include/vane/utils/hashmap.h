#pragma once

#include <vane/utils/defines.h>
#include <vane/utils/collection_common.h>

typedef struct Hashmap Hashmap;
typedef struct HashmapKeySpecs HashmapKeySpecs;
typedef struct ItemSpecs HashmapValueSpecs;
typedef struct HashmapIterator HashmapIterator;
typedef struct Vector HashmapBucket;

#define HASHMAP_KEY_SPECS(KEY_TYPE, HASH_FN, EQ_FN, DESTROY_FN) ((HashmapKeySpecs) { \
    .common = ITEM_SPECS(KEY_TYPE, DESTROY_FN), \
    .hash_fn = (item_hash_fn)HASH_FN, \
    .eq_fn = (item_eq_fn)EQ_FN \
})

#define HASHMAP_VALUE_SPECS(VALUE_TYPE, DESTROY_FN) ITEM_SPECS(VALUE_TYPE, DESTROY_FN)

struct HashmapKeySpecs {
    ItemSpecs common;
    item_hash_fn hash_fn;
    item_eq_fn eq_fn;
};

struct HashmapIterator {
    const Hashmap* map;

    u32 next_bucket_idx;
    u32 next_entry_idx;

    const void* key;
    void* value;
};

struct Hashmap {
    HashmapKeySpecs key_specs;
    HashmapValueSpecs value_specs;

    HashmapBucket* buckets;
    u32 cap;
    u32 size;
};

// -- Creation --

Hashmap hashmap_create(u32 init_cap, HashmapKeySpecs key_specs, HashmapValueSpecs value_specs);

void hashmap_init(Hashmap* map, u32 init_cap, HashmapKeySpecs key_specs, HashmapValueSpecs value_specs);

// -- Destruction --

void hashmap_destroy(Hashmap* map);

void hashmap_clean(Hashmap* map);

// -- Utilities --

u32 hashmap_capacity(const Hashmap* map);

u32 hashmap_size(const Hashmap* map);

f64 hashmap_load_factor(const Hashmap* map);

bool is_hashmap_empty(const Hashmap* map);

void hashmap_resize(Hashmap* map, u32 new_cap);

// -- Insertion --

void hashmap_put(Hashmap* map, const void* key, const void* value);

// -- Retrieval --

void* hashmap_at(const Hashmap* map, const void* key);

const void* hashmap_key_at(const Hashmap* map, const void* key);

bool hashmap_contains(const Hashmap* map, const void* key);

// -- Removal --

bool hashmap_remove(Hashmap* map, const void* key);

// -- Iterator --

HashmapIterator hashmap_get_it(const Hashmap* map);

bool hashmap_it_next(HashmapIterator* it);