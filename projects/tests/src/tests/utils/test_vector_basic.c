#include <utest/utest.h>

#include <vane/utils/vector.h>

struct TestVectorBasic {
    Vector vec;
};

UTEST_F_SETUP(TestVectorBasic) {
    utest_fixture->vec = vector_create(0, VECTOR_ITEM_SPECS(u32, NULL));
}

UTEST_F_TEARDOWN(TestVectorBasic) {
    vector_destroy(&utest_fixture->vec);
}

// -- Insertion --

UTEST_F(TestVectorBasic, vector_push_front1) {
    const u32 value = 5;
    vector_push_front(&utest_fixture->vec, &value);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, 0));
}

UTEST_F(TestVectorBasic, vector_push_back1) {
    const u32 value = 5;
    vector_push_back(&utest_fixture->vec, &value);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, 0));
}

UTEST_F(TestVectorBasic, vector_push_front2) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    for (u32 i = 0; i < values_size; ++i) {
        vector_push_front(&utest_fixture->vec, &values[i]);
    }

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[values_size - i - 1], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorBasic, vector_push_back2) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    for (u32 i = 0; i < values_size; ++i) {
        vector_push_back(&utest_fixture->vec, &values[i]);
    }

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorBasic, vector_insert1) {
    const u32 value = 5;
    vector_insert(&utest_fixture->vec, 0, &value, 1);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, 0));
}

UTEST_F(TestVectorBasic, vector_insert2) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorBasic, vector_insert3) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const u32 value = 17;
    vector_insert(&utest_fixture->vec, 0, &value, 1);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, 0));
    for (u32 i = 1; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i - 1], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorBasic, vector_insert4) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const u32 value = 17;
    vector_insert(&utest_fixture->vec, values_size, &value, 1);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, values_size));
    for (u32 i = 0; i < utest_fixture->vec.size - 1; ++i) {
        ASSERT_EQ(values[i], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorBasic, vector_insert5) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const u32 value = 17;
    vector_insert(&utest_fixture->vec, 2, &value, 1);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, 2));
    for (u32 i = 0; i < 2; ++i) {
        ASSERT_EQ(values[i], *(u32*)vector_at(&utest_fixture->vec, i));
    }
    for (u32 i = 3; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i - 1], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorBasic, vector_extend1) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    vector_extend(&utest_fixture->vec, values, values_size);

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorBasic, vector_extend2) {
    const u32 values[] = { 5, 12, 9, 22, 15 };

    vector_extend(&utest_fixture->vec, values, 2);
    vector_extend(&utest_fixture->vec, values + 2, 3);

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

// -- Removal --

UTEST_F(TestVectorBasic, vector_remove1) {
    const u32 values[] = { 5, 12, 9 };

    vector_extend(&utest_fixture->vec, values, ARR_SIZE(values));

    vector_remove(&utest_fixture->vec, 0);

    ASSERT_EQ(2, vector_size(&utest_fixture->vec));
    ASSERT_EQ(values[1], *(u32*)vector_at(&utest_fixture->vec, 0));
    ASSERT_EQ(values[2], *(u32*)vector_at(&utest_fixture->vec, 1));
}

UTEST_F(TestVectorBasic, vector_remove2) {
    const u32 values[] = { 5, 12, 9 };

    vector_extend(&utest_fixture->vec, values, ARR_SIZE(values));

    vector_remove(&utest_fixture->vec, 2);

    ASSERT_EQ(2, vector_size(&utest_fixture->vec));
    ASSERT_EQ(values[0], *(u32*)vector_at(&utest_fixture->vec, 0));
    ASSERT_EQ(values[1], *(u32*)vector_at(&utest_fixture->vec, 1));
}

UTEST_F(TestVectorBasic, vector_remove3) {
    const u32 values[] = { 5, 12, 9 };

    vector_extend(&utest_fixture->vec, values, ARR_SIZE(values));

    vector_remove(&utest_fixture->vec, 1);

    ASSERT_EQ(2, vector_size(&utest_fixture->vec));
    ASSERT_EQ(values[0], *(u32*)vector_at(&utest_fixture->vec, 0));
    ASSERT_EQ(values[2], *(u32*)vector_at(&utest_fixture->vec, 1));
}

UTEST_F(TestVectorBasic, vector_pop_front1) {
    const u32 values[] = { 5, 12, 9 };

    vector_extend(&utest_fixture->vec, values, ARR_SIZE(values));

    vector_pop_front(&utest_fixture->vec);

    ASSERT_EQ(2, vector_size(&utest_fixture->vec));
    ASSERT_EQ(values[1], *(u32*)vector_at_front(&utest_fixture->vec));
}

UTEST_F(TestVectorBasic, vector_pop_back1) {
    const u32 values[] = { 5, 12, 9 };

    vector_extend(&utest_fixture->vec, values, ARR_SIZE(values));

    vector_pop_back(&utest_fixture->vec);

    ASSERT_EQ(2, vector_size(&utest_fixture->vec));
    ASSERT_EQ(values[1], *(u32*)vector_at_back(&utest_fixture->vec));
}

// -- Modification --

UTEST_F(TestVectorBasic, vector_set1) {
    const u32 values[] = { 5, 12, 9 };
    vector_extend(&utest_fixture->vec, values, ARR_SIZE(values));

    const u32 new_val = 20;
    vector_set(&utest_fixture->vec, 1, &new_val);
    ASSERT_EQ(new_val, *(u32*)vector_at(&utest_fixture->vec, 1));
}

// -- Search --

static i32 cmp_u32(const void* a, const void* b) {
    u32 va = *(const u32*)a;
    u32 vb = *(const u32*)b;

    return !(va == vb);
}

UTEST_F(TestVectorBasic, vector_index_of1) {
    const u32 values[] = { 5, 12, 9, 22 };

    vector_extend(&utest_fixture->vec, values, ARR_SIZE(values));

    const u32 target = 9;
    i32 idx = vector_index_of(&utest_fixture->vec, &target, &cmp_u32);

    ASSERT_EQ(2, idx);
}

UTEST_F(TestVectorBasic, vector_index_of2) {
    const u32 values[] = { 5, 12, 9, 22 };

    vector_extend(&utest_fixture->vec, values, ARR_SIZE(values));

    const u32 target = 30;
    i32 idx = vector_index_of(&utest_fixture->vec, &target, &cmp_u32);

    ASSERT_EQ(NPOS, idx);
}