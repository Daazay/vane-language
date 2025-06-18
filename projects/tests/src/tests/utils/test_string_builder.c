#include <utest/utest.h>

#include <vane/utils/string_builder.h>

struct TestStringBuilder {
    StringBuilder sb;
    String s;
};

UTEST_F_SETUP(TestStringBuilder) {
    (void)utest_result;
    utest_fixture->sb = string_builder_create(16);
    utest_fixture->s = STRING_EMPTY;
}

UTEST_F_TEARDOWN(TestStringBuilder) {
    (void)utest_result;
    string_builder_destroy(&utest_fixture->sb);
    string_destroy(&utest_fixture->s);
}

UTEST_F(TestStringBuilder, string_builder_append_left_c1) {
    string_builder_append_left_c(&utest_fixture->sb, 'a');

    ASSERT_EQ(1, utest_fixture->sb.len);
    ASSERT_EQ('a', utest_fixture->sb.buf[0]);
}

UTEST_F(TestStringBuilder, string_builder_append_right_c1) {
    string_builder_append_right_c(&utest_fixture->sb, 'a');

    ASSERT_EQ(1, utest_fixture->sb.len);
    ASSERT_EQ('a', utest_fixture->sb.buf[0]);
}

UTEST_F(TestStringBuilder, string_builder_append_left_c2) {
    string_builder_append_left_c(&utest_fixture->sb, 'a');
    string_builder_append_left_c(&utest_fixture->sb, 'b');
    string_builder_append_left_c(&utest_fixture->sb, 'c');

    ASSERT_EQ(3, utest_fixture->sb.len);
    ASSERT_EQ('c', utest_fixture->sb.buf[0]);
    ASSERT_EQ('b', utest_fixture->sb.buf[1]);
    ASSERT_EQ('a', utest_fixture->sb.buf[2]);
}

UTEST_F(TestStringBuilder, string_builder_append_right_c2) {
    string_builder_append_right_c(&utest_fixture->sb, 'a');
    string_builder_append_right_c(&utest_fixture->sb, 'b');
    string_builder_append_right_c(&utest_fixture->sb, 'c');

    ASSERT_EQ(3, utest_fixture->sb.len);
    ASSERT_EQ('a', utest_fixture->sb.buf[0]);
    ASSERT_EQ('b', utest_fixture->sb.buf[1]);
    ASSERT_EQ('c', utest_fixture->sb.buf[2]);
}

UTEST_F(TestStringBuilder, string_builder_append_left_cstr1) {
    string_builder_append_left_cstr(&utest_fixture->sb, "abc");

    ASSERT_EQ(3, utest_fixture->sb.len);
    ASSERT_STREQ("abc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, string_builder_append_right_cstr1) {
    string_builder_append_right_cstr(&utest_fixture->sb, "abc");

    ASSERT_EQ(3, utest_fixture->sb.len);
    ASSERT_STREQ("abc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, string_builder_append_left_cstr2) {
    string_builder_append_left_cstr(&utest_fixture->sb, "abc");
    string_builder_append_left_cstr(&utest_fixture->sb, "def");
    string_builder_append_left_cstr(&utest_fixture->sb, "ghi");

    ASSERT_EQ(9, utest_fixture->sb.len);
    ASSERT_STREQ("ghidefabc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, string_builder_append_right_cstr2) {
    string_builder_append_right_cstr(&utest_fixture->sb, "abc");
    string_builder_append_right_cstr(&utest_fixture->sb, "def");
    string_builder_append_right_cstr(&utest_fixture->sb, "ghi");

    ASSERT_EQ(9, utest_fixture->sb.len);
    ASSERT_STREQ("abcdefghi", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, string_builder_append_left_str1) {
    String s = (String){ .text = "abc",.len = 3 };
    string_builder_append_left_str(&utest_fixture->sb, &s);

    ASSERT_EQ(3, utest_fixture->sb.len);
    ASSERT_STREQ("abc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, string_builder_append_right_str1) {
    String s = (String){ .text = "abc",.len = 3 };
    string_builder_append_right_str(&utest_fixture->sb, &s);

    ASSERT_EQ(3, utest_fixture->sb.len);
    ASSERT_STREQ("abc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, string_builder_append_left_str2) {
    String s1 = (String){ .text = "abc",.len = 3 };
    String s2 = (String){ .text = "def",.len = 3 };
    String s3 = (String){ .text = "ghi",.len = 3 };

    string_builder_append_left_str(&utest_fixture->sb, &s1);
    string_builder_append_left_str(&utest_fixture->sb, &s2);
    string_builder_append_left_str(&utest_fixture->sb, &s3);

    ASSERT_EQ(9, utest_fixture->sb.len);
    ASSERT_STREQ("ghidefabc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, string_builder_append_right_str2) {
    String s1 = (String){ .text = "abc",.len = 3 };
    String s2 = (String){ .text = "def",.len = 3 };
    String s3 = (String){ .text = "ghi",.len = 3 };

    string_builder_append_right_str(&utest_fixture->sb, &s1);
    string_builder_append_right_str(&utest_fixture->sb, &s2);
    string_builder_append_right_str(&utest_fixture->sb, &s3);

    ASSERT_EQ(9, utest_fixture->sb.len);
    ASSERT_STREQ("abcdefghi", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, string_builder_get_str1) {
    String s = string_builder_get_str(&utest_fixture->sb);

    ASSERT_TRUE(is_string_empty(&s));
}

UTEST_F(TestStringBuilder, string_builder_get_str2) {
    string_builder_append_left_cstr(&utest_fixture->sb, "abc");
    utest_fixture->s = string_builder_get_str(&utest_fixture->sb);

    ASSERT_EQ(3, utest_fixture->s.len);
    ASSERT_STREQ("abc", utest_fixture->s.text);
}