#include "vane/utils/hashmap.h"

#include <stdlib.h>
#include <string.h>

#include "vane/utils/vector.h"

#pragma region UTILITIES

#define HASHMAP_INIT_BUCKET_SIZE 4
#define HASHMAP_DEFAULT_CAPACITY 8
#define HASHMAP_CAPACITY_MULT    2
#define HASHMAP_GROWTH_FACTOR    2

#define GET_ENTRY_SIZE(MAP) (sizeof(u32) + MAP->key_specs.common.size + MAP->value_specs.size)
#define GET_ENTRY_HASH(ENTRY) (*(u32*)ENTRY)
#define GET_ENTRY_KEY_P(ENTRY) ((byte*)ENTRY + sizeof(u32))
#define GET_ENTRY_VALUE_P(MAP, ENTRY) (GET_ENTRY_KEY_P(ENTRY) + MAP->key_specs.common.size)

static inline u64 get_entry_size(const Hashmap* map) {
    return sizeof(u32) + map->key_specs.common.size + map->value_specs.size;
}

static inline u32 get_entry_hash(const void* entry) {
    return *(const u32*)entry;
}

static inline void* get_entry_key(void* entry) {
    return (byte*)entry + sizeof(u32);
}

static inline void* get_entry_value(const Hashmap* map, void* entry) {
    return (byte*)entry + sizeof(u32) + map->key_specs.common.size;
}

static inline void hashmap_bucket_init(const Hashmap* map, HashmapBucket* bucket) {
    *bucket = vector_create(HASHMAP_INIT_BUCKET_SIZE, (VectorItemSpecs) {
        .size = (u32)get_entry_size(map),
        .is_ptr = true,
        .destroy_fn = NULL,
    });
}

static void hashmap_entry_destroy(const Hashmap* map, void* entry) {
    if (entry == NULL) {
        return;
    }

    if (map->key_specs.common.destroy_fn != NULL) {
        void* key = get_entry_key(entry);
        map->key_specs.common.destroy_fn(COLLECTION_ITEM_CAST(map->key_specs.common, key));
    }
    if (map->value_specs.destroy_fn != NULL) {
        void* value = get_entry_value(map, entry);
        map->value_specs.destroy_fn(COLLECTION_ITEM_CAST(map->value_specs, value));
    }

    free(entry);
}

static void* hashmap_get_entry(const Hashmap* map, const void* key_) {
    const void* key1 = COLLECTION_ITEM_CAST(map->key_specs.common, key_);

    const u32 computed_hash = map->key_specs.hash_fn(key1);
    const u32 idx = computed_hash % map->cap;

    const HashmapBucket* bucket = &map->buckets[idx];
    if (bucket->raw != NULL) {
        for (u32 i = 0; i < bucket->size; ++i) {
            void* entry = vector_at(bucket, i);

            if (computed_hash == get_entry_hash(entry)) {
                const void* key2_ = get_entry_key(entry);
                const void* key2 = COLLECTION_ITEM_CAST(map->key_specs.common, key2_);

                if (map->key_specs.eq_fn(key1, key2)) {
                    return entry;
                }
            }
        }
    }

    return NULL;
}

static void hashmap_it_fill(const Hashmap* map, HashmapIterator* it) {
    void* entry = vector_at(&map->buckets[it->next_bucket_idx], it->next_entry_idx);

    it->entry.hash = get_entry_hash(entry);

    void* entry_key = get_entry_key(entry);
    void* entry_value = get_entry_value(map, entry);

    it->entry.key = COLLECTION_ITEM_CAST(map->key_specs.common, entry_key);
    it->entry.value = COLLECTION_ITEM_CAST(map->value_specs, entry_value);
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

    *map = (Hashmap) {
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
        if (bucket->raw != NULL) {
            for (u32 j = 0; j < bucket->size; ++j) {
                void* entry = vector_at(bucket, j);
                hashmap_entry_destroy(map, entry);
            }
            vector_destroy(bucket);
        }
    }

    free(map->buckets);

    map->buckets = NULL;
    map->size = 0;
    map->cap = 0;
}

void hashmap_resize(Hashmap* map, u32 new_cap) {
    assert(map != NULL && new_cap > 0);

    if (new_cap == map->cap) {
        return;
    }

    HashmapBucket* buckets = calloc(new_cap, sizeof(HashmapBucket));
    assert(buckets != NULL);

    for (u32 i = 0; i < map->cap; ++i) {
        HashmapBucket* bucket = &map->buckets[i];

        if (bucket->raw != NULL) {
            for (u32 j = 0; j < bucket->size; ++j) {
                void* entry = vector_at(bucket, j);
                const u32 new_idx = get_entry_hash(entry) % new_cap;

                if (buckets[new_idx].raw == NULL) {
                    hashmap_bucket_init(map, &buckets[new_idx]);
                }

                vector_push_back(&bucket[new_idx], &entry);
            }
            vector_destroy(bucket);
        }
    }

    free(map->buckets);
    map->buckets = buckets;

    map->cap = new_cap;
}

void hashmap_put(Hashmap* map, const void* key, const void* value) {
    assert(map != NULL && key != NULL);

    void* entry = hashmap_get_entry(map, key);
    if (entry != NULL) {
        void* entry_value = get_entry_value(map, entry);
        if (map->value_specs.destroy_fn != NULL) {
            map->value_specs.destroy_fn(COLLECTION_ITEM_CAST(map->value_specs, entry_value));
        }
        if (value == NULL) {
            memset(entry_value, 0, map->value_specs.size);
        }
        else {
            memcpy(entry_value, value, map->value_specs.size);
        }
        return;
    }

    if (map->size >= map->cap * HASHMAP_GROWTH_FACTOR) {
        hashmap_resize(map, map->size * HASHMAP_CAPACITY_MULT);
    }

    const void* key1 = COLLECTION_ITEM_CAST(map->key_specs.common, key);

    const u32 computed_hash = map->key_specs.hash_fn(key1);
    const u32 idx = computed_hash % map->cap;

    if (map->buckets[idx].raw == NULL) {
        hashmap_bucket_init(map, &map->buckets[idx]);
    }

    entry = malloc(get_entry_size(map));
    assert(entry != NULL);

    *(u32*)entry = computed_hash;
    memcpy(get_entry_key(entry), key, map->key_specs.common.size);
    memcpy(get_entry_value(map, entry), value, map->value_specs.size);

    vector_push_back(&map->buckets[idx], &entry);

    map->size++;
}

void* hashmap_at(Hashmap* map, const void* key) {
    assert(map != NULL && key != NULL);

    void* entry = hashmap_get_entry(map, key);
    if (entry == NULL) {
        return NULL;
    }

    void* entry_value = get_entry_value(map, entry);
    return COLLECTION_ITEM_CAST(map->value_specs, entry_value);
}

bool hashmap_contains(Hashmap* map, const void* key) {
    assert(map != NULL && key != NULL);

    return hashmap_get_entry(map, key) != NULL;
}

void hashmap_remove(Hashmap* map, const void* key) {
    assert(map != NULL && key != NULL);

    const void* key1 = COLLECTION_ITEM_CAST(map->key_specs.common, key);

    const u32 computed_hash = map->key_specs.hash_fn(key1);
    const u32 idx = computed_hash % map->cap;

    HashmapBucket* bucket = &map->buckets[idx];

    if (bucket->raw != NULL) {
        for (u32 i = 0; i < bucket->size; ++i) {
            void* entry = vector_at(bucket, i);

            if (computed_hash == get_entry_hash(entry)) {
                const void* entry_key = get_entry_key(entry);
                const void* key2 = COLLECTION_ITEM_CAST(map->key_specs.common, entry_key);

                if (map->key_specs.eq_fn(key1, key2)) {
                    hashmap_entry_destroy(map, entry);
                    vector_remove(bucket, i);
                    map->size--;
                    return;
                }
            }
        }
    }
}

bool hashmap_it_next(const Hashmap* map, HashmapIterator* it) {
    assert(map != NULL && it != NULL);

    if (it->next_bucket_idx >= map->cap) {
        return false;
    }

    const HashmapBucket* bucket = &map->buckets[it->next_bucket_idx];
    if ((bucket->raw != NULL) && (it->next_entry_idx < bucket->size)) {
        hashmap_it_fill(map, it);
        it->next_entry_idx++;
        return true;
    }

    it->next_bucket_idx++;
    it->next_entry_idx = 0;

    while (it->next_bucket_idx < map->cap) {
        bucket = &map->buckets[it->next_bucket_idx];
        if (bucket->raw != NULL && bucket->size > 0) {
            hashmap_it_fill(map, it);
            it->next_entry_idx++;
            return true;
        }
        it->next_bucket_idx++;
    }

    return false;
}