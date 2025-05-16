#include <utest/utest.h>

#include <vane/utils/hashmap.h>

#include "test_object.h"

struct TestHashmapBasic {
    Hashmap map;
};

struct TestHashmapStruct {
    Hashmap map;
};

struct TestHashmapPtr {
    Hashmap map;
};

typedef struct {
    u32 key;
    i64 value;
} HashmapBasicPair;

static u32 get_u32_hash(const u32* num) {
    return *num;
}

static bool u32_eq(const u32* num1, const u32* num2) {
    return *num1 == *num2;
}

typedef struct {
    TestObject key;
    TestObject value;
} HashmapStructPair;

static u32 get_test_object_hash(const TestObject* obj) {
    return (u32)(obj->a + obj->b);
}

typedef struct {
    TestObject* key;
    TestObject* value;
} HashmapPtrPair;

UTEST_F_SETUP(TestHashmapBasic) {
    utest_fixture->map = hashmap_create(0,
        HASHMAP_KEY_SPECS(u32, &get_u32_hash, &u32_eq, NULL),
        HASHMAP_VALUE_SPECS(i64, NULL)
    );
}

UTEST_F_TEARDOWN(TestHashmapBasic) {
    hashmap_destroy(&utest_fixture->map);
}

UTEST_F_SETUP(TestHashmapStruct) {
    utest_fixture->map = hashmap_create(0,
        HASHMAP_KEY_SPECS(TestObject, &get_test_object_hash, &test_object_eq, NULL),
        HASHMAP_VALUE_SPECS(TestObject, NULL)
    );
}

UTEST_F_TEARDOWN(TestHashmapStruct) {
    hashmap_destroy(&utest_fixture->map);
}

UTEST_F_SETUP(TestHashmapPtr) {
    utest_fixture->map = hashmap_create(0,
        HASHMAP_KEY_SPECS(TestObject*, &get_test_object_hash, &test_object_eq, &test_object_destroy),
        HASHMAP_VALUE_SPECS(TestObject*, &test_object_destroy)
    );
}

UTEST_F_TEARDOWN(TestHashmapPtr) {
    hashmap_destroy(&utest_fixture->map);
}

// put single

UTEST_F(TestHashmapBasic, put_single) {
    HashmapBasicPair pair = { 45, -21422413 };

    hashmap_put(&utest_fixture->map, &pair.key, &pair.value);

    ASSERT_EQ(1, utest_fixture->map.size);
    ASSERT_EQ(pair.value, *(i64*)hashmap_at(&utest_fixture->map, &pair.key));
}

UTEST_F(TestHashmapStruct, put_single) {
    HashmapStructPair pair = { OBJECT(45, -34.24f, -24), OBJECT(17, 12.03f, -1) };

    hashmap_put(&utest_fixture->map, &pair.key, &pair.value);

    ASSERT_EQ(1, utest_fixture->map.size);
    ASSERT_TRUE(test_object_eq(&pair.value, hashmap_at(&utest_fixture->map, &pair.key)));
}

UTEST_F(TestHashmapPtr, put_single) {
    HashmapPtrPair pair = { OBJECT_NEW(45, -34.24f, -24), OBJECT_NEW(17, 12.03f, -1) };

    hashmap_put(&utest_fixture->map, &pair.key, &pair.value);

    ASSERT_EQ(1, utest_fixture->map.size);
    ASSERT_EQ(pair.value, hashmap_at(&utest_fixture->map, &pair.key));
}

// put many

UTEST_F(TestHashmapBasic, put_many) {
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

UTEST_F(TestHashmapStruct, put_many) {
    const HashmapStructPair pairs[] = {
        { OBJECT(12, 333.5f, -1),  OBJECT(1489, -145.65f, 14) },
        { OBJECT(75, 115.7f, 3),   OBJECT(612, 1565.73f, -13) },
        { OBJECT(15, 783.5f, 6),   OBJECT(6251, 294.0f, 7) },
        { OBJECT(97, 1.544f, 24),  OBJECT(2677, -0.673f, 9) },
        { OBJECT(34, 115.0f, 3),   OBJECT(7899, 0.673f, -17) },
        { OBJECT(77, 0.333f, -24), OBJECT(11, -15.673f, 0) },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapStructPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    ASSERT_EQ(pairs_count, utest_fixture->map.size);

    for (u32 i = 0; i < pairs_count; ++i) {
        ASSERT_TRUE(test_object_eq(&pairs[i].value, hashmap_at(&utest_fixture->map, &pairs[i].key)));
    }
}

UTEST_F(TestHashmapPtr, put_many) {
    const HashmapPtrPair pairs[] = {
        { OBJECT_NEW(12, 333.5f, -1),  OBJECT_NEW(1489, -145.65f, 14) },
        { OBJECT_NEW(75, 115.7f, 3),   OBJECT_NEW(612, 1565.73f, -13) },
        { OBJECT_NEW(15, 783.5f, 6),   OBJECT_NEW(6251, 294.0f, 7) },
        { OBJECT_NEW(97, 1.544f, 24),  OBJECT_NEW(2677, -0.673f, 9) },
        { OBJECT_NEW(34, 115.0f, 3),   OBJECT_NEW(7899, 0.673f, -17) },
        { OBJECT_NEW(77, 0.333f, -24), OBJECT_NEW(11, -15.673f, 0) },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapPtrPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    ASSERT_EQ(pairs_count, utest_fixture->map.size);

    for (u32 i = 0; i < pairs_count; ++i) {
        ASSERT_TRUE(test_object_eq(pairs[i].value, hashmap_at(&utest_fixture->map, &pairs[i].key)));
    }
}

// contains

UTEST_F(TestHashmapBasic, contains) {
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

UTEST_F(TestHashmapStruct, contains) {
    const HashmapStructPair pairs[] = {
        { OBJECT(12, 333.5f, -1),  OBJECT(1489, -145.65f, 14) },
        { OBJECT(75, 115.7f, 3),   OBJECT(612, 1565.73f, -13) },
        { OBJECT(15, 783.5f, 6),   OBJECT(6251, 294.0f, 7) },
        { OBJECT(97, 1.544f, 24),  OBJECT(2677, -0.673f, 9) },
        { OBJECT(34, 115.0f, 3),   OBJECT(7899, 0.673f, -17) },
        { OBJECT(77, 0.333f, -24), OBJECT(11, -15.673f, 0) },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapStructPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    const TestObject existing_key = OBJECT(34, 115.0f, 3);
    ASSERT_TRUE(hashmap_contains(&utest_fixture->map, &existing_key));

    const TestObject not_existing_key = OBJECT(134, -645.251f, 15);
    ASSERT_FALSE(hashmap_contains(&utest_fixture->map, &not_existing_key));
}

UTEST_F(TestHashmapPtr, contains) {
    const HashmapPtrPair pairs[] = {
        { OBJECT_NEW(12, 333.5f, -1),  OBJECT_NEW(1489, -145.65f, 14) },
        { OBJECT_NEW(75, 115.7f, 3),   OBJECT_NEW(612, 1565.73f, -13) },
        { OBJECT_NEW(15, 783.5f, 6),   OBJECT_NEW(6251, 294.0f, 7) },
        { OBJECT_NEW(97, 1.544f, 24),  OBJECT_NEW(2677, -0.673f, 9) },
        { OBJECT_NEW(34, 115.0f, 3),   OBJECT_NEW(7899, 0.673f, -17) },
        { OBJECT_NEW(77, 0.333f, -24), OBJECT_NEW(11, -15.673f, 0) },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapPtrPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    TestObject existing_obj = OBJECT(34, 115.0f, 3);
    TestObject* existing_key = &existing_obj;
    ASSERT_TRUE(hashmap_contains(&utest_fixture->map, &existing_key));

    TestObject not_existing_obj = OBJECT(134, -645.251f, 15);
    TestObject* not_existing_key = &not_existing_obj;
    ASSERT_FALSE(hashmap_contains(&utest_fixture->map, &not_existing_key));
}

// iterate empty

UTEST_F(TestHashmapBasic, iterate_empty) {
    HashmapIterator it = { 0 };

    ASSERT_FALSE(hashmap_it_next(&utest_fixture->map, &it));
}

UTEST_F(TestHashmapStruct, iterate_empty) {
    HashmapIterator it = { 0 };

    ASSERT_FALSE(hashmap_it_next(&utest_fixture->map, &it));
}

UTEST_F(TestHashmapPtr, iterate_empty) {
    HashmapIterator it = { 0 };

    ASSERT_FALSE(hashmap_it_next(&utest_fixture->map, &it));
}

// iterate not empty

UTEST_F(TestHashmapBasic, iterate) {
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

    HashmapIterator it = { 0 };
    for (u32 i = 0; i < pairs_count; ++i) {
        ASSERT_TRUE(hashmap_it_next(&utest_fixture->map, &it));
    }
    ASSERT_FALSE(hashmap_it_next(&utest_fixture->map, &it));
}

UTEST_F(TestHashmapStruct, iterate) {
    const HashmapStructPair pairs[] = {
        { OBJECT(12, 333.5f, -1),  OBJECT(1489, -145.65f, 14) },
        { OBJECT(75, 115.7f, 3),   OBJECT(612, 1565.73f, -13) },
        { OBJECT(15, 783.5f, 6),   OBJECT(6251, 294.0f, 7) },
        { OBJECT(97, 1.544f, 24),  OBJECT(2677, -0.673f, 9) },
        { OBJECT(34, 115.0f, 3),   OBJECT(7899, 0.673f, -17) },
        { OBJECT(77, 0.333f, -24), OBJECT(11, -15.673f, 0) },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapStructPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    HashmapIterator it = { 0 };
    for (u32 i = 0; i < pairs_count; ++i) {
        ASSERT_TRUE(hashmap_it_next(&utest_fixture->map, &it));
    }
    ASSERT_FALSE(hashmap_it_next(&utest_fixture->map, &it));
}

UTEST_F(TestHashmapPtr, iterate) {
    const HashmapPtrPair pairs[] = {
        { OBJECT_NEW(12, 333.5f, -1),  OBJECT_NEW(1489, -145.65f, 14) },
        { OBJECT_NEW(75, 115.7f, 3),   OBJECT_NEW(612, 1565.73f, -13) },
        { OBJECT_NEW(15, 783.5f, 6),   OBJECT_NEW(6251, 294.0f, 7) },
        { OBJECT_NEW(97, 1.544f, 24),  OBJECT_NEW(2677, -0.673f, 9) },
        { OBJECT_NEW(34, 115.0f, 3),   OBJECT_NEW(7899, 0.673f, -17) },
        { OBJECT_NEW(77, 0.333f, -24), OBJECT_NEW(11, -15.673f, 0) },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapPtrPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    HashmapIterator it = { 0 };
    for (u32 i = 0; i < pairs_count; ++i) {
        ASSERT_TRUE(hashmap_it_next(&utest_fixture->map, &it));
    }
    ASSERT_FALSE(hashmap_it_next(&utest_fixture->map, &it));
}