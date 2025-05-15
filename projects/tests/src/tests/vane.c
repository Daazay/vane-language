#include <utest/utest.h>

#include "vane/vane.h"

UTEST(Test, sum) {
    ASSERT_EQ(5, sum(2,3));
}