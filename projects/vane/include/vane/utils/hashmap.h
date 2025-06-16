#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/collection_item_specs.h"

#define HASHMAP_KEY_SPECS(TYPE, HASH_FN, EQ_FN, DESTROY_FN) ((HashmapKeySpecs) { \
    .common = COLLECTION_ITEM_SPECS(TYPE, DESTROY_FN), \
    .hash_fn = (hashmap_key_hash_fn)HASH_FN, \
    .eq_fn = (hashmap_key_eq_fn)EQ_FN \
})

#define HASHMAP_VALUE_SPECS(TYPE, DESTROY_FN) COLLECTION_ITEM_SPECS(TYPE, DESTROY_FN)

typedef struct HashmapKeySpecs HashmapKeySpecs;
typedef u32(*hashmap_key_hash_fn)(const void* key);
typedef bool(*hashmap_key_eq_fn)(const void* key1, const void* key2);

typedef CollectionItemSpecs HashmapValueSpecs;

typedef struct HashmapEntry HashmapEntry;
typedef struct HashmapIterator HashmapIterator;
typedef struct Vector HashmapBucket;
typedef struct Hashmap Hashmap;

struct HashmapKeySpecs {
    CollectionItemSpecs common;
    hashmap_key_hash_fn hash_fn;
    hashmap_key_eq_fn eq_fn;
};

struct HashmapEntry {
    u32 hash;
    void* key;
    void* value;
};

struct HashmapIterator {
    u32 next_bucket_idx;
    u32 next_entry_idx;

    HashmapEntry entry;
};

struct Hashmap {
    HashmapKeySpecs key_specs;
    HashmapValueSpecs value_specs;

    HashmapBucket* buckets;
    u32 size;
    u32 cap;
};

Hashmap hashmap_create(u32 init_cap, HashmapKeySpecs key_specs, HashmapValueSpecs value_specs);

void hashmap_init(Hashmap* map, u32 init_cap, HashmapKeySpecs key_specs, HashmapValueSpecs value_specs);

void hashmap_destroy(Hashmap* map);

void hashmap_resize(Hashmap* map, u32 new_cap);

HashmapEntry hashmap_put(Hashmap* map, const void* key, const void* value);

void* hashmap_at(Hashmap* map, const void* key);

bool hashmap_contains(Hashmap* map, const void* key);

void hashmap_remove(Hashmap* map, const void* key);

bool hashmap_it_next(const Hashmap* map, HashmapIterator* it);

const void* hashmap_get_key_ref(const Hashmap* map, const void* key);

void* hashmap_get_value_ref(const Hashmap* map, const void* key);