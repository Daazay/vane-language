#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/collection_item_specs.h"

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

void hashmap_put(Hashmap* map, const void* key, const void* value);

void* hashmap_at(Hashmap* map, const void* key);

bool hashmap_contains(Hashmap* map, const void* key);

void hashmap_remove(Hashmap* map, const void* key);

bool hashmap_it_next(const Hashmap* map, HashmapIterator* it);