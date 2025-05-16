#include <utest/utest.h>

#include <vane/utils/vector.h>

#include "test_object.h"

struct TestVectorBasic {
    Vector vec; // u32
};

UTEST_F_SETUP(TestVectorBasic) {
    utest_fixture->vec = vector_create(0, VECTOR_ITEM_SPECS(u32, NULL));
}

UTEST_F_TEARDOWN(TestVectorBasic) {
    vector_destroy(&utest_fixture->vec);
}

struct TestVectorStruct {
    Vector vec; // TestObject
};

UTEST_F_SETUP(TestVectorStruct) {
    utest_fixture->vec = vector_create(0, VECTOR_ITEM_SPECS(TestObject, NULL));
}

UTEST_F_TEARDOWN(TestVectorStruct) {
    vector_destroy(&utest_fixture->vec);
}

struct TestVectorPtr {
    Vector vec; // TestObject*
};

UTEST_F_SETUP(TestVectorPtr) {
    utest_fixture->vec = vector_create(0, VECTOR_ITEM_SPECS(TestObject*, &test_object_destroy));
}
UTEST_F_TEARDOWN(TestVectorPtr) {
    vector_destroy(&utest_fixture->vec);
}

// push front single

UTEST_F(TestVectorBasic, push_front_single) {
    const u32 value = 5;
    vector_push_front(&utest_fixture->vec, &value);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, 0));
}

UTEST_F(TestVectorStruct, push_front_single) {
    TestObject value = OBJECT(45, -34.24f, -24);
    vector_push_front(&utest_fixture->vec, &value);

    ASSERT_TRUE(test_object_eq(&value, vector_at(&utest_fixture->vec, 0)));
}

UTEST_F(TestVectorPtr, push_front_single) {
    TestObject* value = OBJECT_NEW(45, -34.24f, -24);
    vector_push_front(&utest_fixture->vec, &value);

    ASSERT_EQ(value, vector_at(&utest_fixture->vec, 0));
}

// push back single

UTEST_F(TestVectorBasic, push_back_single) {
    const u32 value = 5;
    vector_push_back(&utest_fixture->vec, &value);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, 0));
}

UTEST_F(TestVectorStruct, push_back_single) {
    TestObject value = OBJECT(45, -34.24f, -24);
    vector_push_back(&utest_fixture->vec, &value);

    ASSERT_TRUE(test_object_eq(&value, vector_at(&utest_fixture->vec, 0)));
}

UTEST_F(TestVectorPtr, push_back_single) {
    TestObject* value = OBJECT_NEW(45, -34.24f, -24);
    vector_push_back(&utest_fixture->vec, &value);

    ASSERT_EQ(value, vector_at(&utest_fixture->vec, 0));
}

// push front multiple

UTEST_F(TestVectorBasic, push_front_multiple) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    for (u32 i = 0; i < values_size; ++i) {
        vector_push_front(&utest_fixture->vec, &values[i]);
    }

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[values_size - i - 1], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorStruct, push_front_multiple) {
    const TestObject values[] = {
        OBJECT(45,   78.15f,  16),
        OBJECT(15,  -3.001f,  -24),
        OBJECT(445, -26.97f,  16),
        OBJECT(11,  -13.05f,  3),
        OBJECT(96,   1.13f,   5),
        OBJECT(22,   0.03f,   6),
        OBJECT(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    for (u32 i = 0; i < values_size; ++i) {
        vector_push_front(&utest_fixture->vec, &values[i]);
    }

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_TRUE(test_object_eq(&values[values_size - i - 1], vector_at(&utest_fixture->vec, i)));
    }
}

UTEST_F(TestVectorPtr, push_front_multiple) {
    const TestObject* values[] = {
        OBJECT_NEW(45,   78.15f,  16),
        OBJECT_NEW(15,  -3.001f,  -24),
        OBJECT_NEW(445, -26.97f,  16),
        OBJECT_NEW(11,  -13.05f,  3),
        OBJECT_NEW(96,   1.13f,   5),
        OBJECT_NEW(22,   0.03f,   6),
        OBJECT_NEW(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    for (u32 i = 0; i < values_size; ++i) {
        vector_push_front(&utest_fixture->vec, &values[i]);
    }

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[values_size - i - 1], vector_at(&utest_fixture->vec, i));
    }
}

// push back multiple

UTEST_F(TestVectorBasic, push_back_multiple) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    for (u32 i = 0; i < values_size; ++i) {
        vector_push_back(&utest_fixture->vec, &values[i]);
    }

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorStruct, push_back_multiple) {
    const TestObject values[] = {
        OBJECT(45,   78.15f,  16),
        OBJECT(15,  -3.001f,  -24),
        OBJECT(445, -26.97f,  16),
        OBJECT(11,  -13.05f,  3),
        OBJECT(96,   1.13f,   5),
        OBJECT(22,   0.03f,   6),
        OBJECT(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    for (u32 i = 0; i < values_size; ++i) {
        vector_push_back(&utest_fixture->vec, &values[i]);
    }

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_TRUE(test_object_eq(&values[i], vector_at(&utest_fixture->vec, i)));
    }
}

UTEST_F(TestVectorPtr, push_back_multiple) {
    const TestObject* values[] = {
        OBJECT_NEW(45,   78.15f,  16),
        OBJECT_NEW(15,  -3.001f,  -24),
        OBJECT_NEW(445, -26.97f,  16),
        OBJECT_NEW(11,  -13.05f,  3),
        OBJECT_NEW(96,   1.13f,   5),
        OBJECT_NEW(22,   0.03f,   6),
        OBJECT_NEW(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    for (u32 i = 0; i < values_size; ++i) {
        vector_push_back(&utest_fixture->vec, &values[i]);
    }

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i], vector_at(&utest_fixture->vec, i));
    }
}

// vector insert single in empty

UTEST_F(TestVectorBasic, insert_single_in_empty) {
    const u32 value = 5;
    vector_insert(&utest_fixture->vec, 0, &value, 1);

    ASSERT_EQ(value, *(u32*)vector_at(&utest_fixture->vec, 0));
}

UTEST_F(TestVectorStruct, insert_single_in_empty) {
    TestObject value = OBJECT(45, -34.24f, -24);
    vector_insert(&utest_fixture->vec, 0, &value, 1);

    ASSERT_TRUE(test_object_eq(&value, vector_at(&utest_fixture->vec, 0)));
}

UTEST_F(TestVectorPtr, insert_single_in_empty) {
    TestObject* value = OBJECT_NEW(45, -34.24f, -24);
    vector_insert(&utest_fixture->vec, 0, &value, 1);

    ASSERT_EQ(value, vector_at(&utest_fixture->vec, 0));
}

// vector insert multiple in empty

UTEST_F(TestVectorBasic, insert_multiple_in_empty) {
    const u32 values[] = { 5, 12, 9, 22, 15 };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i], *(u32*)vector_at(&utest_fixture->vec, i));
    }
}

UTEST_F(TestVectorStruct, insert_multiple_in_empty) {
    const TestObject values[] = {
        OBJECT(45,   78.15f,  16),
        OBJECT(15,  -3.001f,  -24),
        OBJECT(445, -26.97f,  16),
        OBJECT(11,  -13.05f,  3),
        OBJECT(96,   1.13f,   5),
        OBJECT(22,   0.03f,   6),
        OBJECT(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    for (u32 i = 0; i < utest_fixture->vec.size; ++i) {
        ASSERT_TRUE(test_object_eq(&values[i], vector_at(&utest_fixture->vec, i)));
    }
}

UTEST_F(TestVectorPtr, insert_multiple_in_empty) {
    const TestObject* values[] = {
        OBJECT_NEW(45,   78.15f,  16),
        OBJECT_NEW(15,  -3.001f,  -24),
        OBJECT_NEW(445, -26.97f,  16),
        OBJECT_NEW(11,  -13.05f,  3),
        OBJECT_NEW(96,   1.13f,   5),
        OBJECT_NEW(22,   0.03f,   6),
        OBJECT_NEW(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    for (u32 i = 0; i < values_size; ++i) {
        ASSERT_EQ(values[i], vector_at(&utest_fixture->vec, i));
    }
}

// vector insert single at front

UTEST_F(TestVectorBasic, insert_single_at_front) {
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

UTEST_F(TestVectorStruct, insert_single_at_front) {
    const TestObject values[] = {
        OBJECT(45,   78.15f,  16),
        OBJECT(15,  -3.001f,  -24),
        OBJECT(445, -26.97f,  16),
        OBJECT(11,  -13.05f,  3),
        OBJECT(96,   1.13f,   5),
        OBJECT(22,   0.03f,   6),
        OBJECT(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const TestObject value = OBJECT(14, 14.14f, 14);
    vector_insert(&utest_fixture->vec, 0, &value, 1);

    ASSERT_TRUE(test_object_eq(&value, vector_at(&utest_fixture->vec, 0)));
    for (u32 i = 1; i < utest_fixture->vec.size; ++i) {
        ASSERT_TRUE(test_object_eq(&values[i - 1], vector_at(&utest_fixture->vec, i)));
    }
}

UTEST_F(TestVectorPtr, insert_single_at_front) {
    const TestObject* values[] = {
        OBJECT_NEW(45,   78.15f,  16),
        OBJECT_NEW(15,  -3.001f,  -24),
        OBJECT_NEW(445, -26.97f,  16),
        OBJECT_NEW(11,  -13.05f,  3),
        OBJECT_NEW(96,   1.13f,   5),
        OBJECT_NEW(22,   0.03f,   6),
        OBJECT_NEW(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const TestObject* value = OBJECT_NEW(14, 14.14f, 14);
    vector_insert(&utest_fixture->vec, 0, &value, 1);

    ASSERT_EQ(value, vector_at(&utest_fixture->vec, 0));
    for (u32 i = 1; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i - 1], vector_at(&utest_fixture->vec, i));
    }
}

// vector insert single at back

UTEST_F(TestVectorBasic, insert_single_at_back) {
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

UTEST_F(TestVectorStruct, insert_single_at_back) {
    const TestObject values[] = {
        OBJECT(45,   78.15f,  16),
        OBJECT(15,  -3.001f,  -24),
        OBJECT(445, -26.97f,  16),
        OBJECT(11,  -13.05f,  3),
        OBJECT(96,   1.13f,   5),
        OBJECT(22,   0.03f,   6),
        OBJECT(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const TestObject value = OBJECT(14, 14.14f, 14);
    vector_insert(&utest_fixture->vec, values_size, &value, 1);

    ASSERT_TRUE(test_object_eq(&value, vector_at(&utest_fixture->vec, values_size)));
    for (u32 i = 0; i < utest_fixture->vec.size - 1; ++i) {
        ASSERT_TRUE(test_object_eq(&values[i], vector_at(&utest_fixture->vec, i)));
    }
}

UTEST_F(TestVectorPtr, insert_single_at_back) {
    const TestObject* values[] = {
        OBJECT_NEW(45,   78.15f,  16),
        OBJECT_NEW(15,  -3.001f,  -24),
        OBJECT_NEW(445, -26.97f,  16),
        OBJECT_NEW(11,  -13.05f,  3),
        OBJECT_NEW(96,   1.13f,   5),
        OBJECT_NEW(22,   0.03f,   6),
        OBJECT_NEW(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const TestObject* value = OBJECT_NEW(14, 14.14f, 14);
    vector_insert(&utest_fixture->vec, values_size, &value, 1);

    ASSERT_EQ(value, vector_at(&utest_fixture->vec, values_size));
    for (u32 i = 0; i < utest_fixture->vec.size - 1; ++i) {
        ASSERT_EQ(values[i], vector_at(&utest_fixture->vec, i));
    }
}

// vector insert single at middle

UTEST_F(TestVectorBasic, insert_single_at_middle) {
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

UTEST_F(TestVectorStruct, insert_single_at_middle) {
    const TestObject values[] = {
        OBJECT(45,   78.15f,  16),
        OBJECT(15,  -3.001f,  -24),
        OBJECT(445, -26.97f,  16),
        OBJECT(11,  -13.05f,  3),
        OBJECT(96,   1.13f,   5),
        OBJECT(22,   0.03f,   6),
        OBJECT(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const TestObject value = OBJECT(14, 14.14f, 14);
    vector_insert(&utest_fixture->vec, 2, &value, 1);

    ASSERT_TRUE(test_object_eq(&value, vector_at(&utest_fixture->vec, 2)));
    for (u32 i = 0; i < 2; ++i) {
        ASSERT_TRUE(test_object_eq(&values[i], vector_at(&utest_fixture->vec, i)));
    }
    for (u32 i = 3; i < utest_fixture->vec.size; ++i) {
        ASSERT_TRUE(test_object_eq(&values[i - 1], vector_at(&utest_fixture->vec, i)));
    }
}

UTEST_F(TestVectorPtr, insert_single_at_middle) {
    const TestObject* values[] = {
        OBJECT_NEW(45,   78.15f,  16),
        OBJECT_NEW(15,  -3.001f,  -24),
        OBJECT_NEW(445, -26.97f,  16),
        OBJECT_NEW(11,  -13.05f,  3),
        OBJECT_NEW(96,   1.13f,   5),
        OBJECT_NEW(22,   0.03f,   6),
        OBJECT_NEW(1,    4.246f,  -7),
    };
    const u32 values_size = ARR_SIZE(values);

    vector_insert(&utest_fixture->vec, 0, &values, values_size);

    const TestObject* value = OBJECT_NEW(14, 14.14f, 14);
    vector_insert(&utest_fixture->vec, 2, &value, 1);

    ASSERT_EQ(value, vector_at(&utest_fixture->vec, 2));
    for (u32 i = 0; i < 2; ++i) {
        ASSERT_EQ(values[i], vector_at(&utest_fixture->vec, i));
    }
    for (u32 i = 3; i < utest_fixture->vec.size; ++i) {
        ASSERT_EQ(values[i - 1], vector_at(&utest_fixture->vec, i));
    }
}
