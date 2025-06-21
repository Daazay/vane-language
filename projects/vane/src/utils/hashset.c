#include "vane/utils/hashset.h"

#include <stdlib.h>
#include <string.h>

#include "vane/utils/vector.h"

#pragma region UTILITIES

// >=
#define HASHSET_MAX_ENTRY_SIZE_NOT_ALLOCATED 64
#define HASHSET_INIT_BUCKET_SIZE             4
#define HASHSET_DEFAULT_CAPACITY             8
#define HASHSET_CAPACITY_MULT                2
#define HASHSET_LOAD_FACTOR                  2

static inline u64 get_entry_size(const Hashset* set) {
    return sizeof(u32) + set->item_specs.common.size;
}

static inline u32 get_entry_hash(const void* entry) {
    return *(const u32*)entry;
}

static inline void* get_entry_item(void* entry) {
    assert(entry != NULL);
    return (byte*)entry + sizeof(u32);
}

static inline void hashset_bucket_init(const Hashset* set, HashsetBucket* bucket) {
    assert(set != NULL && bucket != NULL);

    const u64 entry_size = get_entry_size(set);
    const bool is_ptr = entry_size >= HASHSET_MAX_ENTRY_SIZE_NOT_ALLOCATED;

    *bucket = vector_create(HASHSET_DEFAULT_CAPACITY, (ItemSpecs) {
        .destroy_fn = NULL, // deallocation anyway done manually
        .is_ptr = is_ptr,
        .size = (u32)(is_ptr ? sizeof(void*) : entry_size),
    });
}

static void hashset_entry_destroy(const Hashset* set, void* entry) {
    assert(set != NULL && entry != NULL);

    if (set->item_specs.common.destroy_fn != NULL) {
        void* item = get_entry_item(entry);
        set->item_specs.common.destroy_fn(ITEM_SPECS_CAST(set->item_specs.common, item));
    }

    const u64 entry_size = get_entry_size(set);
    if (entry_size >= HASHSET_MAX_ENTRY_SIZE_NOT_ALLOCATED) {
        free(entry);
    }
}

static void* hashset_get_entry(const Hashset* set, const void* item1_) {
    assert(set != NULL && item1_ != NULL);

    const void* item1 = ITEM_SPECS_CAST(set->item_specs.common, item1_);
    const u32 computed_hash = set->item_specs.hash_fn(item1);
    const u32 idx = computed_hash % set->cap;

    const HashsetBucket* bucket = &set->buckets[idx];

    if (bucket->raw == NULL) {
        return NULL;
    }

    for (u32 i = 0; i < bucket->size; ++i) {
        void* entry = vector_at(bucket, i);

        if (computed_hash != get_entry_hash(entry)) {
            continue;
        }

        const void* item2_ = get_entry_item(entry);
        const void* item2 = ITEM_SPECS_CAST(set->item_specs.common, item2_);

        if (set->item_specs.eq_fn(item1, item2)) {
            return entry;
        }
    }

    return NULL;
}

#pragma endregion

Hashset hashset_create(u32 init_cap, HashsetItemSpecs item_specs) {
    Hashset set = { 0 };
    hashset_init(&set, init_cap, item_specs);
    return set;
}

void hashset_init(Hashset* set, u32 init_cap, HashsetItemSpecs item_specs) {
    assert(set != NULL);
    assert(item_specs.common.size > 0 && item_specs.hash_fn != NULL && item_specs.eq_fn != NULL);

    u32 cap = init_cap > 0
        ? init_cap
        : HASHSET_DEFAULT_CAPACITY;

    HashsetBucket* buckets = calloc(cap, sizeof(HashsetBucket));
    assert(buckets != NULL);

    *set = (Hashset){
        .item_specs = item_specs,
        .buckets = buckets,
        .size = 0,
        .cap = cap,
    };
}

void hashset_destroy(Hashset* set) {
    if (set == NULL || set->buckets == NULL) {
        return;
    }

    hashset_clean(set);

    free(set->buckets);

    set->buckets = NULL;
    set->cap = 0;
}

void hashset_clean(Hashset* set) {
    assert(set != NULL);

    for (u32 i = 0; i < set->cap; ++i) {
        HashsetBucket* bucket = &set->buckets[i];

        if (bucket->raw == NULL) {
            continue;
        }

        for (u32 j = 0; j < bucket->size; ++j) {
            void* entry = vector_at(bucket, j);
            hashset_entry_destroy(set, entry);
        }

        vector_destroy(bucket);
    }

    set->size = 0;
}

u32 hashset_capacity(const Hashset* set) {
    assert(set != NULL);
    return set->cap;
}

u32 hashset_size(const Hashset* set) {
    assert(set != NULL);
    return set->size;
}

f64 hashset_load_factor(const Hashset* set) {
    assert(set != NULL);
    return (f64)set->size / (f64)set->cap;
}

bool is_hashset_empty(const Hashset* set) {
    assert(set != NULL);
    return set->size == 0;
}

void hashset_resize(Hashset* set, u32 new_cap) {
    assert(set != NULL);

    if (set->cap == new_cap) {
        return;
    }

    HashsetBucket* new_buckets = calloc(new_cap, sizeof(HashsetBucket));
    assert(new_buckets != NULL);

    for (u32 i = 0; i < set->cap; ++i) {
        HashsetBucket* bucket = &set->buckets[i];

        if (bucket->raw == NULL) {
            continue;
        }

        for (u32 j = 0; j < bucket->size; ++j) {
            void* entry = vector_at(bucket, j);
            const u32 new_idx = get_entry_hash(entry) % new_cap;

            HashsetBucket* new_bucket = &new_buckets[new_idx];

            if (new_bucket->raw == NULL) {
                hashset_bucket_init(set, new_bucket);
            }

            vector_push_back(new_bucket, entry);
        }

        vector_destroy(bucket);
    }

    free(set->buckets);

    set->buckets = new_buckets;
    set->cap = new_cap;
}

void hashset_put(Hashset* set, const void* item_) {
    assert(set != NULL && item_ != NULL);

    void* entry = hashset_get_entry(set, item_);
    if (entry != NULL) {
        if (set->item_specs.common.destroy_fn != NULL) {
            void* item = get_entry_item(entry);
            set->item_specs.common.destroy_fn(ITEM_SPECS_CAST(set->item_specs.common, item));
        }
        return;
    }

    if (set->size >= set->cap * HASHSET_LOAD_FACTOR) {
        hashset_resize(set, set->cap * HASHSET_CAPACITY_MULT);
    }

    const void* item = ITEM_SPECS_CAST(set->item_specs.common, item_);
    const u32 computed_hash = set->item_specs.hash_fn(item);
    const u32 idx = computed_hash % set->cap;

    HashsetBucket* bucket = &set->buckets[idx];

    if (bucket->raw == NULL) {
        hashset_bucket_init(set, bucket);
    }

    const u64 entry_size = get_entry_size(set);
    if (entry_size >= HASHSET_MAX_ENTRY_SIZE_NOT_ALLOCATED) {
        entry = malloc(entry_size);
        assert(entry != NULL);

        (*(u32*)entry) = computed_hash;
        memcpy(get_entry_item(entry), item_, set->item_specs.common.size);

        vector_push_back(bucket, &entry);
    }
    else {
        u8 buf[HASHSET_MAX_ENTRY_SIZE_NOT_ALLOCATED] = { 0 };

        (*(u32*)buf) = computed_hash;
        memcpy(get_entry_item((void*)buf), item_, set->item_specs.common.size);

        vector_push_back(bucket, buf);
    }

    set->size++;
}

const void* hashset_at(const Hashset* set, const void* item) {
    assert(set != NULL && item != NULL);

    void* entry = hashset_get_entry(set, item);
    return (entry != NULL) ? ITEM_SPECS_CAST(set->item_specs.common, get_entry_item(entry)) : NULL;
}

bool hashset_contains(const Hashset* set, const void* item) {
    assert(set != NULL && item != NULL);

    return hashset_get_entry(set, item) != NULL;
}

bool hashset_remove(Hashset* set, const void* item) {
    assert(set != NULL && item != NULL);

    const void* item1 = ITEM_SPECS_CAST(set->item_specs.common, item);
    const u32 computed_hash = set->item_specs.hash_fn(item1);
    const u32 idx = computed_hash % set->cap;

    HashsetBucket* bucket = &set->buckets[idx];

    if (bucket->raw == NULL) {
        return false;
    }

    for (u32 i = 0; i < bucket->size; ++i) {
        void* entry = vector_at(bucket, i);

        if (computed_hash != get_entry_hash(entry)) {
            continue;
        }

        const void* item2_ = get_entry_item(entry);
        const void* item2 = ITEM_SPECS_CAST(set->item_specs.common, item2_);

        if (!set->item_specs.eq_fn(item1, item2)) {
            continue;
        }

        hashset_entry_destroy(set, entry);
        vector_remove(bucket, i);

        set->size--;
        return true;
    }

    return true;
}

HashsetIterator hashset_get_it(const Hashset* set) {
    assert(set != NULL);

    return (HashsetIterator) {
        .set = set,
        .next_bucket_idx = 0,
        .next_entry_idx = 0,
        .item = NULL,
    };
}

bool hashset_it_next(HashsetIterator* it) {
    assert(it != NULL);
    assert(it->set != NULL);

    while (it->next_bucket_idx < it->set->cap) {
        HashsetBucket* bucket = &it->set->buckets[it->next_bucket_idx];

        if (bucket->raw != NULL && it->next_entry_idx < bucket->size) {
            void* entry = vector_at(bucket, it->next_entry_idx++);

            it->item = ITEM_SPECS_CAST(it->set->item_specs.common, get_entry_item(entry));

            return true;
        }

        it->next_bucket_idx++;
        it->next_entry_idx = 0;
    }

    return false;
}