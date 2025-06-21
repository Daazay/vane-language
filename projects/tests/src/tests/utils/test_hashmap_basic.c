#include <utest/utest.h>

#include <vane/utils/hashmap.h>

typedef struct HashmapBasicPair HashmapBasicPair;

struct HashmapBasicPair {
    u32 key;
    i64 value;
};

static u32 get_u32_hash(const u32* num) {
    return *num;
}

static bool u32_eq(const u32* num1, const u32* num2) {
    return *num1 == *num2;
}

struct TestHashmapBasic {
    Hashmap map;
};

UTEST_F_SETUP(TestHashmapBasic) {
    utest_fixture->map = hashmap_create(0,
        HASHMAP_KEY_SPECS(u32, &get_u32_hash, &u32_eq, NULL),
        HASHMAP_VALUE_SPECS(i64, NULL)
    );
}

UTEST_F_TEARDOWN(TestHashmapBasic) {
    hashmap_destroy(&utest_fixture->map);
}

UTEST_F(TestHashmapBasic, hashmap_put1) {
    HashmapBasicPair pair = { 45, -21422413 };

    hashmap_put(&utest_fixture->map, &pair.key, &pair.value);

    ASSERT_EQ(1, utest_fixture->map.size);
    ASSERT_EQ(pair.value, *(i64*)hashmap_at(&utest_fixture->map, &pair.key));
}

UTEST_F(TestHashmapBasic, hashmap_put2) {
    const HashmapBasicPair pairs[] = {
        { 45, -21422413 },
        { 111, 1255 },
        { 1255, 13 },
        { 2467, 1 },
        { 57, 900 },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapBasicPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    ASSERT_EQ(pairs_count, utest_fixture->map.size);

    for (u32 i = 0; i < pairs_count; ++i) {
        ASSERT_EQ(pairs[i].value, *(i64*)hashmap_at(&utest_fixture->map, &pairs[i].key));
    }
}

UTEST_F(TestHashmapBasic, hashmap_contains1) {
    const HashmapBasicPair pairs[] = {
        { 45, -21422413 },
        { 111, 1255 },
        { 1255, 13 },
        { 2467, 1 },
        { 57, 900 },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapBasicPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    const u32 existing_key = 111;
    ASSERT_TRUE(hashmap_contains(&utest_fixture->map, &existing_key));

    const u32 not_existing_key = 124567;
    ASSERT_FALSE(hashmap_contains(&utest_fixture->map, &not_existing_key));
}

UTEST_F(TestHashmapBasic, hashmap_it_next1) {
    HashmapIterator it = hashmap_get_it(&utest_fixture->map);

    ASSERT_FALSE(hashmap_it_next(&it));
}

UTEST_F(TestHashmapBasic, hashmap_it_next2) {
    const HashmapBasicPair pairs[] = {
        { 45, -21422413 },
        { 111, 1255 },
        { 1255, 13 },
        { 2467, 1 },
        { 57, 900 },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapBasicPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    HashmapIterator it = hashmap_get_it(&utest_fixture->map);
    for (u32 i = 0; i < pairs_count; ++i) {
        ASSERT_TRUE(hashmap_it_next(&it));
    }
    ASSERT_FALSE(hashmap_it_next(&it));
}