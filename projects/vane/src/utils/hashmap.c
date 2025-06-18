#include "vane/utils/hashmap.h"

#include <stdlib.h>
#include <string.h>

#include "vane/utils/vector.h"

#pragma region UTILITIES

#define HASHMAP_INIT_BUCKET_SIZE 4
#define HASHMAP_DEFAULT_CAPACITY 8
#define HASHMAP_CAPACITY_MULT    2
#define HASHMAP_LOAD_FACTOR      2

static inline u64 get_entry_size(const Hashmap* map) {
    assert(map != NULL);
    return sizeof(u32) + map->key_specs.common.size + map->value_specs.size;
}

static inline u32 get_entry_hash(const void* entry) {
    assert(entry != NULL);
    return *(const u32*)entry;
}

static inline void* get_entry_key(void* entry) {
    assert(entry != NULL);
    return (byte*)entry + sizeof(u32);
}

static inline void* get_entry_value(const Hashmap* map, void* entry) {
    assert(map != NULL && entry != NULL);
    return (byte*)entry + sizeof(u32) + map->key_specs.common.size;
}

static inline void hashmap_bucket_init(HashmapBucket* bucket) {
    assert(bucket != NULL);
    *bucket = vector_create(HASHMAP_DEFAULT_CAPACITY, VECTOR_ITEM_SPECS(void*, NULL));
}

static void hashmap_entry_destroy(const Hashmap* map, void* entry) {
    assert(map != NULL && entry != NULL);

    if (map->key_specs.common.destroy_fn != NULL) {
        void* key = get_entry_key(entry);
        map->key_specs.common.destroy_fn(ITEM_SPECS_CAST(map->key_specs.common, key));
    }
    if (map->value_specs.destroy_fn != NULL) {
        void* value = get_entry_value(map, entry);
        map->value_specs.destroy_fn(ITEM_SPECS_CAST(map->value_specs, value));
    }

    free(entry);
}

static void* hashmap_get_entry(const Hashmap* map, const void* _key1) {
    assert(map != NULL && _key1 != NULL);

    const void* key1 = ITEM_SPECS_CAST(map->key_specs.common, _key1);
    const u32 computed_hash = map->key_specs.hash_fn(key1);
    const u32 idx = computed_hash % map->cap;

    const HashmapBucket* bucket = &map->buckets[idx];

    if (bucket->raw == NULL) {
        return NULL;
    }

    for (u32 i = 0; i < bucket->size; ++i) {
        void* entry = vector_at(bucket, i);

        if (computed_hash != get_entry_hash(entry)) {
            continue;
        }

        const void* key2_ = get_entry_key(entry);
        const void* key2 = ITEM_SPECS_CAST(map->key_specs.common, key2_);

        if (map->key_specs.eq_fn(key1, key2)) {
            return entry;
        }
    }

    return NULL;
}

#pragma endregion

Hashmap hashmap_create(u32 init_cap, HashmapKeySpecs key_specs, HashmapValueSpecs value_specs) {
    Hashmap map = { 0 };
    hashmap_init(&map, init_cap, key_specs, value_specs);
    return map;
}

void hashmap_init(Hashmap* map, u32 init_cap, HashmapKeySpecs key_specs, HashmapValueSpecs value_specs) {
    assert(map != NULL);
    assert(key_specs.common.size > 0 && key_specs.hash_fn != NULL && key_specs.eq_fn != NULL);
    assert(value_specs.size > 0);

    u32 cap = init_cap > 0
        ? init_cap
        : HASHMAP_DEFAULT_CAPACITY;

    HashmapBucket* buckets = calloc(cap, sizeof(HashmapBucket));
    assert(buckets != NULL);

    *map = (Hashmap){
        .key_specs = key_specs,
        .value_specs = value_specs,
        .buckets = buckets,
        .cap = cap,
        .size = 0,
    };
}

void hashmap_destroy(Hashmap* map) {
    if (map == NULL || map->buckets == NULL) {
        return;
    }

    for (u32 i = 0; i < map->cap; ++i) {
        HashmapBucket* bucket = &map->buckets[i];

        if (bucket->raw == NULL) {
            continue;
        }

        for (u32 j = 0; j < bucket->size; ++j) {
            void* entry = vector_at(bucket, j);
            hashmap_entry_destroy(map, entry);
        }

        vector_destroy(bucket);
    }

    free(map->buckets);

    map->buckets = NULL;
    map->size = 0;
    map->cap = 0;
}

void hashmap_clean(Hashmap* map) {
    assert(map != NULL);

    for (u32 i = 0; i < map->cap; ++i) {
        HashmapBucket* bucket = &map->buckets[i];

        if (bucket->raw == NULL) {
            continue;
        }

        for (u32 j = 0; j < bucket->size; ++j) {
            void* entry = vector_at(bucket, j);
            hashmap_entry_destroy(map, entry);
        }
    }

    map->size = 0;
}

u32 hashmap_capacity(const Hashmap* map) {
    assert(map != NULL);
    return map->cap;
}

u32 hashmap_size(const Hashmap* map) {
    assert(map != NULL);
    return map->size;
}

f64 hashmap_load_factor(const Hashmap* map) {
    assert(map != NULL);
    return (f64)map->size / (f64)map->cap;
}

bool is_hashmap_empty(const Hashmap* map) {
    assert(map != NULL);
    return map->size == 0;
}

void hashmap_resize(Hashmap* map, u32 new_cap) {
    assert(map != NULL);

    if (new_cap == map->cap) {
        return;
    }

    HashmapBucket* new_buckets = calloc(new_cap, sizeof(HashmapBucket));
    assert(new_buckets != NULL);

    for (u32 i = 0; i < map->cap; ++i) {
        HashmapBucket* bucket = &map->buckets[i];

        if (bucket->raw == NULL) {
            continue;
        }

        for (u32 j = 0; j < bucket->size; ++j) {
            void* entry = vector_at(bucket, j);
            const u32 new_idx = get_entry_hash(entry) % new_cap;

            HashmapBucket* new_bucket = &new_buckets[new_idx];

            if (new_bucket->raw == NULL) {
                hashmap_bucket_init(new_bucket);
            }

            vector_push_back(new_bucket, &entry);
        }
        vector_destroy(bucket);
    }

    free(map->buckets);

    map->buckets = new_buckets;
    map->cap = new_cap;
}

void hashmap_put(Hashmap* map, const void* key1_, const void* value) {
    assert(map != NULL && key1_ != NULL);
    assert((value != NULL || map->value_specs.is_ptr) && "value can be NULL only if item type is ptr");

    void* entry = hashmap_get_entry(map, key1_);
    if (entry != NULL) {
        void* entry_value = get_entry_value(map, entry);
        if (map->value_specs.destroy_fn != NULL) {
            map->value_specs.destroy_fn(ITEM_SPECS_CAST(map->value_specs, entry_value));
        }

        if (value != NULL) {
            memcpy(entry_value, value, map->value_specs.size);
        }
        else {
            memset(entry_value, 0, map->value_specs.size);
        }

        return;
    }

    if (map->size >= map->cap * HASHMAP_LOAD_FACTOR) {
        hashmap_resize(map, map->cap * HASHMAP_CAPACITY_MULT);
    }

    const void* key1 = ITEM_SPECS_CAST(map->key_specs.common, key1_);
    const u32 computed_hash = map->key_specs.hash_fn(key1);
    const u32 idx = computed_hash % map->cap;

    HashmapBucket* bucket = &map->buckets[idx];

    if (bucket->raw == NULL) {
        hashmap_bucket_init(bucket);
    }

    const u64 size = get_entry_size(map);

    entry = malloc(size);
    assert(entry != NULL);

    (*(u32*)entry) = computed_hash;
    memcpy(get_entry_key(entry), key1_, map->key_specs.common.size);

    if (value != NULL) {
        memcpy(get_entry_value(map, entry), value, map->value_specs.size);
    }
    else {
        memset(get_entry_value(map, entry), 0, map->value_specs.size);
    }

    vector_push_back(bucket, &entry);
    map->size++;
}

void* hashmap_at(const Hashmap* map, const void* key) {
    assert(map != NULL && key != NULL);

    void* entry = hashmap_get_entry(map, key);
    return entry ? ITEM_SPECS_CAST(map->value_specs, get_entry_value(map, entry)) : NULL;
}

const void* hashmap_key_at(const Hashmap* map, const void* key) {
    assert(map != NULL && key != NULL);

    void* entry = hashmap_get_entry(map, key);
    return entry ? ITEM_SPECS_CAST(map->key_specs.common, get_entry_key(entry)) : NULL;
}

bool hashmap_contains(const Hashmap* map, const void* key) {
    assert(map != NULL && key != NULL);

    return hashmap_get_entry(map, key) != NULL;
}

bool hashmap_remove(Hashmap* map, const void* key1_) {
    assert(map != NULL && key1_ != NULL);

    const void* key1 = ITEM_SPECS_CAST(map->key_specs.common, key1_);
    const u32 computed_hash = map->key_specs.hash_fn(key1);
    const u32 idx = computed_hash % map->cap;

    HashmapBucket* bucket = &map->buckets[idx];

    if (bucket->raw == NULL) {
        return false;
    }

    for (u32 i = 0; i < bucket->size; ++i) {
        void* entry = vector_at(bucket, i);

        if (computed_hash != get_entry_hash(entry)) {
            continue;
        }

        const void* key2_ = get_entry_key(entry);
        const void* key2 = ITEM_SPECS_CAST(map->key_specs.common, key2_);

        if (!map->key_specs.eq_fn(key1, key2)) {
            continue;
        }

        hashmap_entry_destroy(map, entry);
        vector_remove(bucket, i);
        map->size--;
        return true;
    }
    return false;
}

HashmapIterator hashmap_get_it(const Hashmap* map) {
    assert(map != NULL);

    return (HashmapIterator) {
        .map = map,
        .next_bucket_idx = 0,
        .next_entry_idx = 0,
        .key = NULL,
        .value = NULL,
    };
}

bool hashmap_it_next(HashmapIterator* it) {
    assert(it != NULL);
    assert(it->map != NULL);

    while (it->next_bucket_idx < it->map->cap) {
        HashmapBucket* bucket = &it->map->buckets[it->next_bucket_idx];

        if (bucket->raw != NULL && it->next_entry_idx < bucket->size) {
            void* entry = vector_at(bucket, it->next_entry_idx++);

            it->key = ITEM_SPECS_CAST(it->map->key_specs.common, get_entry_key(entry));
            it->value = ITEM_SPECS_CAST(it->map->value_specs, get_entry_value(it->map, entry));

            return true;
        }

        it->next_bucket_idx++;
        it->next_entry_idx = 0;
    }

    return false;
}