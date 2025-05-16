#include <utest/utest.h>

#include <vane/utils/string_builder.h>

struct TestStringBuilder {
    StringBuilder sb;
    String s;
};

UTEST_F_SETUP(TestStringBuilder) {
    (void)utest_result;
    utest_fixture->sb = sb_create(16);
    utest_fixture->s = STRING_EMPTY;
}

UTEST_F_TEARDOWN(TestStringBuilder) {
    (void)utest_result;
    sb_destroy(&utest_fixture->sb);
    string_destroy(&utest_fixture->s);
}

// append single

UTEST_F(TestStringBuilder, append_left_c_single) {
    sb_append_left_c(&utest_fixture->sb, 'a');

    ASSERT_EQ((u64)1, utest_fixture->sb.len);
    ASSERT_EQ('a', utest_fixture->sb.buf[0]);
}

UTEST_F(TestStringBuilder, append_right_c_single) {
    sb_append_right_c(&utest_fixture->sb, 'a');

    ASSERT_EQ((u64)1, utest_fixture->sb.len);
    ASSERT_EQ('a', utest_fixture->sb.buf[0]);
}

// append multiple

UTEST_F(TestStringBuilder, append_left_c_multiple) {
    sb_append_left_c(&utest_fixture->sb, 'a');
    sb_append_left_c(&utest_fixture->sb, 'b');
    sb_append_left_c(&utest_fixture->sb, 'c');

    ASSERT_EQ((u64)3, utest_fixture->sb.len);
    ASSERT_EQ('c', utest_fixture->sb.buf[0]);
    ASSERT_EQ('b', utest_fixture->sb.buf[1]);
    ASSERT_EQ('a', utest_fixture->sb.buf[2]);
}

UTEST_F(TestStringBuilder, append_right_c_multiple) {
    sb_append_right_c(&utest_fixture->sb, 'a');
    sb_append_right_c(&utest_fixture->sb, 'b');
    sb_append_right_c(&utest_fixture->sb, 'c');

    ASSERT_EQ((u64)3, utest_fixture->sb.len);
    ASSERT_EQ('a', utest_fixture->sb.buf[0]);
    ASSERT_EQ('b', utest_fixture->sb.buf[1]);
    ASSERT_EQ('c', utest_fixture->sb.buf[2]);
}

// append cstr single

UTEST_F(TestStringBuilder, append_left_cstr_single) {
    sb_append_left_cstr(&utest_fixture->sb, "abc");

    ASSERT_EQ((u64)3, utest_fixture->sb.len);
    ASSERT_STREQ("abc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, append_right_cstr_single) {
    sb_append_right_cstr(&utest_fixture->sb, "abc");

    ASSERT_EQ((u64)3, utest_fixture->sb.len);
    ASSERT_STREQ("abc", utest_fixture->sb.buf);
}

// append cstr multiple

UTEST_F(TestStringBuilder, append_left_cstr_multiple) {
    sb_append_left_cstr(&utest_fixture->sb, "abc");
    sb_append_left_cstr(&utest_fixture->sb, "def");
    sb_append_left_cstr(&utest_fixture->sb, "ghi");

    ASSERT_EQ((u64)9, utest_fixture->sb.len);
    ASSERT_STREQ("ghidefabc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, append_right_cstr_multiple) {
    sb_append_right_cstr(&utest_fixture->sb, "abc");
    sb_append_right_cstr(&utest_fixture->sb, "def");
    sb_append_right_cstr(&utest_fixture->sb, "ghi");

    ASSERT_EQ((u64)9, utest_fixture->sb.len);
    ASSERT_STREQ("abcdefghi", utest_fixture->sb.buf);
}

// append str single

UTEST_F(TestStringBuilder, append_left_str_single) {
    String s = (String){ .text = "abc",.len = 3 };
    sb_append_left_str(&utest_fixture->sb, &s);

    ASSERT_EQ((u64)3, utest_fixture->sb.len);
    ASSERT_STREQ("abc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, append_right_str_single) {
    String s = (String){ .text = "abc",.len = 3 };
    sb_append_right_str(&utest_fixture->sb, &s);

    ASSERT_EQ((u64)3, utest_fixture->sb.len);
    ASSERT_STREQ("abc", utest_fixture->sb.buf);
}

// append str multiple

UTEST_F(TestStringBuilder, append_left_str_multiple) {
    String s1 = (String){ .text = "abc",.len = 3 };
    String s2 = (String){ .text = "def",.len = 3 };
    String s3 = (String){ .text = "ghi",.len = 3 };

    sb_append_left_str(&utest_fixture->sb, &s1);
    sb_append_left_str(&utest_fixture->sb, &s2);
    sb_append_left_str(&utest_fixture->sb, &s3);

    ASSERT_EQ((u64)9, utest_fixture->sb.len);
    ASSERT_STREQ("ghidefabc", utest_fixture->sb.buf);
}

UTEST_F(TestStringBuilder, append_right_str_multiple) {
    String s1 = (String){ .text = "abc",.len = 3 };
    String s2 = (String){ .text = "def",.len = 3 };
    String s3 = (String){ .text = "ghi",.len = 3 };

    sb_append_right_str(&utest_fixture->sb, &s1);
    sb_append_right_str(&utest_fixture->sb, &s2);
    sb_append_right_str(&utest_fixture->sb, &s3);

    ASSERT_EQ((u64)9, utest_fixture->sb.len);
    ASSERT_STREQ("abcdefghi", utest_fixture->sb.buf);
}

// get str

UTEST_F(TestStringBuilder, get_str_empty) {
    String s = sb_get_str(&utest_fixture->sb);

    ASSERT_TRUE(string_is_empty(&s));
}

UTEST_F(TestStringBuilder, get_str) {
    sb_append_left_cstr(&utest_fixture->sb, "abc");
    utest_fixture->s = sb_get_str(&utest_fixture->sb);

    ASSERT_EQ((u64)3, utest_fixture->s.len);
    ASSERT_STREQ("abc", utest_fixture->s.text);
}