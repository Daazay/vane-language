#include <utest/utest.h>

#include <vane/utils/hashset.h>

static u32 get_u32_hash(const u32* num) {
    return *num;
}

static bool u32_eq(const u32* num1, const u32* num2) {
    return *num1 == *num2;
}

struct TestHashsetBasic {
    Hashset set;
};

UTEST_F_SETUP(TestHashsetBasic) {
    utest_fixture->set = hashset_create(0, HASHSET_ITEM_SPECS(u32, &get_u32_hash, &u32_eq, NULL));
}

UTEST_F_TEARDOWN(TestHashsetBasic) {
    hashset_destroy(&utest_fixture->set);
}

UTEST_F(TestHashsetBasic, hashset_put1) {
    u32 item = 32;
    hashset_put(&utest_fixture->set, &item);

    ASSERT_EQ(1, utest_fixture->set.size);
    ASSERT_EQ(item, *(u32*)hashset_at(&utest_fixture->set, &item));
}

UTEST_F(TestHashsetBasic, hashset_put2) {
    const u32 items[] = { 45, 111, 1255, 2467, 57 };
    const u32 items_count = sizeof(items) / sizeof(u32);

    for (u32 i = 0; i < items_count; ++i) {
        hashset_put(&utest_fixture->set, &items[i]);
    }

    ASSERT_EQ(items_count, utest_fixture->set.size);

    for (u32 i = 0; i < items_count; ++i) {
        ASSERT_EQ(items[i], *(u32*)hashset_at(&utest_fixture->set, &items[i]));
    }
}

UTEST_F(TestHashsetBasic, hashset_contains1) {
    const u32 items[] = { 45, 111, 1255, 2467, 57 };
    const u32 items_count = sizeof(items) / sizeof(u32);

    for (u32 i = 0; i < items_count; ++i) {
        hashset_put(&utest_fixture->set, &items[i]);
    }

    const u32 existing_item = 111;
    ASSERT_TRUE(hashset_contains(&utest_fixture->set, &existing_item));

    const u32 not_existing_item = 124567;
    ASSERT_FALSE(hashset_contains(&utest_fixture->set, &not_existing_item));
}

UTEST_F(TestHashsetBasic, hashset_it_next1) {
    HashsetIterator it = hashset_get_it(&utest_fixture->set);

    ASSERT_FALSE(hashset_it_next(&it));
}

UTEST_F(TestHashsetBasic, hashset_it_next2) {
    const u32 items[] = { 45, 111, 1255, 2467, 57 };
    const u32 items_count = sizeof(items) / sizeof(u32);

    for (u32 i = 0; i < items_count; ++i) {
        hashset_put(&utest_fixture->set, &items[i]);
    }

    HashsetIterator it = hashset_get_it(&utest_fixture->set);
    for (u32 i = 0; i < items_count; ++i) {
        ASSERT_TRUE(hashset_it_next(&it));
    }
    ASSERT_FALSE(hashset_it_next(&it));
}