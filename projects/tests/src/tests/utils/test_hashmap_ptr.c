#include <utest/utest.h>

#include <vane/utils/hashmap.h>

#include "test_object.h"

typedef struct HashmapPtrPair HashmapPtrPair;

struct HashmapPtrPair {
    TestObject* key;
    TestObject* value;
};

struct TestHashmapPtr {
    Hashmap map;
};

UTEST_F_SETUP(TestHashmapPtr) {
    utest_fixture->map = hashmap_create(0,
        HASHMAP_KEY_SPECS(TestObject*, &get_test_object_hash, &test_object_eq, &test_object_destroy),
        HASHMAP_VALUE_SPECS(TestObject*, &test_object_destroy)
    );
}

UTEST_F_TEARDOWN(TestHashmapPtr) {
    hashmap_destroy(&utest_fixture->map);
}

UTEST_F(TestHashmapPtr, hashmap_put1) {
    HashmapPtrPair pair = { test_object_allocate(45, 34, 24), test_object_allocate(17, 12, 1) };

    hashmap_put(&utest_fixture->map, &pair.key, &pair.value);

    ASSERT_EQ(1, utest_fixture->map.size);
    ASSERT_EQ(pair.value, hashmap_at(&utest_fixture->map, &pair.key));
}

UTEST_F(TestHashmapPtr, hashmap_put2) {
    const HashmapPtrPair pairs[] = {
        { test_object_allocate(12, 33, 1), test_object_allocate(1489, 65, 14) },
        { test_object_allocate(75, 15, 3), test_object_allocate(612, 73, 13) },
        { test_object_allocate(15, 73, 6), test_object_allocate(6251, 0, 7) },
        { test_object_allocate(97, 1, 24), test_object_allocate(2677, 67, 9) },
        { test_object_allocate(34, 15, 3), test_object_allocate(7899, 73, 17) },
        { test_object_allocate(77, 0, 24), test_object_allocate(11, 15, 0) },
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

UTEST_F(TestHashmapPtr, hashmap_contains1) {
    const HashmapPtrPair pairs[] = {
        { test_object_allocate(12, 33, 1), test_object_allocate(1489, 65, 14) },
        { test_object_allocate(75, 15, 3), test_object_allocate(612, 73, 13) },
        { test_object_allocate(15, 73, 6), test_object_allocate(6251, 0, 7) },
        { test_object_allocate(97, 1, 24), test_object_allocate(2677, 67, 9) },
        { test_object_allocate(34, 15, 3), test_object_allocate(7899, 73, 17) },
        { test_object_allocate(77, 0, 24), test_object_allocate(11, 15, 0) },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapPtrPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    TestObject existing_obj = test_object_create(34, 15, 3);
    TestObject* existing_key = &existing_obj;
    ASSERT_TRUE(hashmap_contains(&utest_fixture->map, &existing_key));

    TestObject not_existing_obj = test_object_create(134, 251, 15);
    TestObject* not_existing_key = &not_existing_obj;
    ASSERT_FALSE(hashmap_contains(&utest_fixture->map, &not_existing_key));
}

UTEST_F(TestHashmapPtr, hashmap_it_next1) {
    HashmapIterator it = hashmap_get_it(&utest_fixture->map);

    ASSERT_FALSE(hashmap_it_next(&it));
}

UTEST_F(TestHashmapPtr, hashmap_it_next2) {
    const HashmapPtrPair pairs[] = {
        { test_object_allocate(12, 33, 1),  test_object_allocate(1489, 65, 14) },
        { test_object_allocate(75, 15, 3),   test_object_allocate(612, 73, 13) },
        { test_object_allocate(15, 73, 6),   test_object_allocate(6251, 0, 7) },
        { test_object_allocate(97, 1, 24),  test_object_allocate(2677, 67, 9) },
        { test_object_allocate(34, 15, 3),   test_object_allocate(7899, 73, 17) },
        { test_object_allocate(77, 0, 24), test_object_allocate(11, 15, 0) },
    };
    const u32 pairs_count = sizeof(pairs) / sizeof(HashmapPtrPair);

    for (u32 i = 0; i < pairs_count; ++i) {
        hashmap_put(&utest_fixture->map, &pairs[i].key, &pairs[i].value);
    }

    HashmapIterator it = hashmap_get_it(&utest_fixture->map);
    for (u32 i = 0; i < pairs_count; ++i) {
        ASSERT_TRUE(hashmap_it_next(&it));
    }
    ASSERT_FALSE(hashmap_it_next(&it));
}
