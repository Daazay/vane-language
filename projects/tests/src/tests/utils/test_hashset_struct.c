#include <utest/utest.h>

#include <vane/utils/hashset.h>

#include "test_object.h"

struct TestHashsetStruct {
    Hashset set;
};

UTEST_F_SETUP(TestHashsetStruct) {
    utest_fixture->set = hashset_create(0,
        HASHSET_ITEM_SPECS(TestObject, &get_test_object_hash, &test_object_eq, NULL)
    );
}

UTEST_F_TEARDOWN(TestHashsetStruct) {
    hashset_destroy(&utest_fixture->set);
}

UTEST_F(TestHashsetStruct, hashset_put1) {
    TestObject item = test_object_create(12, 33, 1);
    hashset_put(&utest_fixture->set, &item);

    ASSERT_EQ(1, utest_fixture->set.size);
    ASSERT_TRUE(test_object_eq(&item, hashset_at(&utest_fixture->set, &item)));
}

UTEST_F(TestHashsetStruct, hashset_put2) {
    const TestObject items[] = {
        test_object_create(12, 33, 1),
        test_object_create(75, 15, 3),
        test_object_create(15, 73, 6),
        test_object_create(97, 1, 24),
        test_object_create(34, 15, 3),
        test_object_create(77, 0, 24),
    };
    const u32 items_count = sizeof(items) / sizeof(TestObject);

    for (u32 i = 0; i < items_count; ++i) {
        hashset_put(&utest_fixture->set, &items[i]);
    }

    ASSERT_EQ(items_count, utest_fixture->set.size);

    for (u32 i = 0; i < items_count; ++i) {
        ASSERT_TRUE(test_object_eq(&items[i], hashset_at(&utest_fixture->set, &items[i])));
    }
}

UTEST_F(TestHashsetStruct, hashset_contains1) {
    const TestObject items[] = {
        test_object_create(12, 33, 1),
        test_object_create(75, 15, 3),
        test_object_create(15, 73, 6),
        test_object_create(97, 1, 24),
        test_object_create(34, 15, 3),
        test_object_create(77, 0, 24),
    };
    const u32 items_count = sizeof(items) / sizeof(TestObject);

    for (u32 i = 0; i < items_count; ++i) {
        hashset_put(&utest_fixture->set, &items[i]);
    }

    const TestObject existing_item = test_object_create(34, 15, 3);
    ASSERT_TRUE(hashset_contains(&utest_fixture->set, &existing_item));

    const TestObject not_existing_item = test_object_create(134, 45, 15);
    ASSERT_FALSE(hashset_contains(&utest_fixture->set, &not_existing_item));
}

UTEST_F(TestHashsetStruct, hashset_it_next1) {
    HashsetIterator it = hashset_get_it(&utest_fixture->set);

    ASSERT_FALSE(hashset_it_next(&it));
}

UTEST_F(TestHashsetStruct, hashset_it_next2) {
    const TestObject items[] = {
            test_object_create(12, 33, 1),
            test_object_create(75, 15, 3),
            test_object_create(15, 73, 6),
            test_object_create(97, 1, 24),
            test_object_create(34, 15, 3),
            test_object_create(77, 0, 24),
    };
    const u32 items_count = sizeof(items) / sizeof(TestObject);

    for (u32 i = 0; i < items_count; ++i) {
        hashset_put(&utest_fixture->set, &items[i]);
    }

    HashsetIterator it = hashset_get_it(&utest_fixture->set);
    for (u32 i = 0; i < items_count; ++i) {
        ASSERT_TRUE(hashset_it_next(&it));
    }
    ASSERT_FALSE(hashset_it_next(&it));
}