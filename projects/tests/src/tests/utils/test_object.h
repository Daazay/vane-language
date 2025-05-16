#pragma once

#include <vane/utils/defines.h>

#include <stdlib.h>
#include <math.h>

#define EPSILON (1e-9)

typedef struct TestObject TestObject;

struct TestObject {
    u32 a;
    f64 b;
    i8 c;
};

static TestObject* test_object_create(u32 a, f64 b, i8 c) {
    TestObject* obj = malloc(sizeof(TestObject));
    assert(obj != NULL);

    obj->a = a;
    obj->b = b;
    obj->c = c;

    return obj;
}

static void test_object_destroy(TestObject* obj) {
    free(obj);
}

static bool test_object_eq(const TestObject* obj1, const TestObject* obj2) {
    return
        (obj1->a == obj2->a) &&
        (obj1->c == obj2->c) &&
        (fabs(obj1->b - obj2->b) <= EPSILON);
}

#define OBJECT(A, B, C) (TestObject) { .a = A, .b = B, .c = C }
#define OBJECT_NEW(A, B, C) test_object_create(A, B, C)