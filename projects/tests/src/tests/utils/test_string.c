#include <utest/utest.h>

#include <vane/utils/string.h>

struct TestString {
    union {
        String s;
        String s1;
    };
    union {
        String tmp;
        String s2;
    };
    String s3;
};

UTEST_F_SETUP(TestString) {
    (void)utest_result;
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = STRING_EMPTY;
    utest_fixture->s3 = STRING_EMPTY;
}

UTEST_F_TEARDOWN(TestString) {
    (void)utest_result;
    string_destroy(&utest_fixture->s1);
    string_destroy(&utest_fixture->s2);
    string_destroy(&utest_fixture->s3);
}

// from cstr

UTEST_F(TestString, from_cstr_null) {
    utest_fixture->s = string_from_cstr(NULL);

    ASSERT_EQ((u64)0, utest_fixture->s.len);
    ASSERT_EQ(NULL, utest_fixture->s.text);
}

UTEST_F(TestString, from_cstr_empty) {
    utest_fixture->s = string_from_cstr("");

    ASSERT_EQ((u64)0, utest_fixture->s.len);
    ASSERT_EQ(NULL, utest_fixture->s.text);
}

UTEST_F(TestString, from_cstr) {
    const char* cstr = "abc";
    utest_fixture->s = string_from_cstr(cstr);

    ASSERT_EQ((u64)3, utest_fixture->s.len);
    ASSERT_STREQ(cstr, utest_fixture->s.text);
}

// clone

UTEST_F(TestString, clone_empty) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_clone(&utest_fixture->s1);

    ASSERT_EQ((u64)0, utest_fixture->s2.len);
    ASSERT_EQ(NULL, utest_fixture->s2.text);
}

UTEST_F(TestString, clone) {
    const char* cstr = "abc";
    utest_fixture->s1 = string_from_cstr(cstr);
    utest_fixture->s2 = string_clone(&utest_fixture->s1);

    ASSERT_EQ(utest_fixture->s1.len, utest_fixture->s2.len);
    ASSERT_STREQ(utest_fixture->s1.text, utest_fixture->s2.text);
}

// substr

UTEST_F(TestString, substr_empty_source) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_substr(&utest_fixture->s1, 0, 10);

    ASSERT_EQ((u64)0, utest_fixture->s2.len);
    ASSERT_EQ(NULL, utest_fixture->s2.text);
}

UTEST_F(TestString, substr_over_offset) {
    const char cstr[] = "unhappiness";
    const u64 cstr_len = sizeof(cstr) / sizeof(char) - 1;

    utest_fixture->s1 = string_from_cstr(cstr);
    utest_fixture->s2 = string_substr(&utest_fixture->s1, cstr_len, 10);

    ASSERT_EQ((u64)0, utest_fixture->s2.len);
    ASSERT_EQ(NULL, utest_fixture->s2.text);
}

UTEST_F(TestString, substr_zero_len) {
    const char cstr[] = "unhappiness";

    utest_fixture->s1 = string_from_cstr(cstr);
    utest_fixture->s2 = string_substr(&utest_fixture->s1, 0, 0);

    ASSERT_EQ((u64)0, utest_fixture->s2.len);
    ASSERT_EQ(NULL, utest_fixture->s2.text);
}

UTEST_F(TestString, substr) {
    const char cstr[] = "unhappiness";
    const u64 cstr_len = sizeof(cstr) / sizeof(char) - 1;

    utest_fixture->s1 = string_from_cstr(cstr);
    utest_fixture->s2 = string_substr(&utest_fixture->s1, 0, 25);

    ASSERT_EQ(cstr_len, utest_fixture->s2.len);
    ASSERT_STREQ(cstr, utest_fixture->s2.text);
}

// replace c

UTEST_F(TestString, replace_c_empty_source) {
    utest_fixture->s = string_from_cstr(NULL);
    string_replace_c(&utest_fixture->s, '#', '!');

    ASSERT_EQ((u64)0, utest_fixture->s.len);
    ASSERT_EQ(NULL, utest_fixture->s.text);
}

UTEST_F(TestString, replace_c_char_not_existing) {
    const char source[] = "unhappiness";

    const char expected[] = "unhappiness";
    const u64 expected_len = sizeof(expected) / sizeof(char) - 1;

    utest_fixture->s = string_from_cstr(source);
    string_replace_c(&utest_fixture->s, '#', '!');

    ASSERT_EQ(expected_len, utest_fixture->s.len);
    ASSERT_STREQ(expected, utest_fixture->s.text);
}

UTEST_F(TestString, replace_c) {
    const char source[] = "unhappiness";

    const char expected[] = "unhaPPiness";
    const u64 expected_len = sizeof(expected) / sizeof(char) - 1;

    utest_fixture->s = string_from_cstr(source);
    string_replace_c(&utest_fixture->s, 'p', 'P');

    ASSERT_EQ(expected_len, utest_fixture->s.len);
    ASSERT_STREQ(expected, utest_fixture->s.text);
}

// replace cstr

UTEST_F(TestString, replace_cstr_empty_source) {
    utest_fixture->s = string_from_cstr(NULL);
    string_replace_cstr(&utest_fixture->s, "abc", "def");

    ASSERT_EQ((u64)0, utest_fixture->s.len);
    ASSERT_EQ(NULL, utest_fixture->s.text);
}

UTEST_F(TestString, replace_cstr_not_existing) {
    const char source[] = "unhappiness";

    const char expected[] = "unhappiness";
    const u64 expected_len = sizeof(expected) / sizeof(char) - 1;

    utest_fixture->s = string_from_cstr(source);
    string_replace_cstr(&utest_fixture->s, "abc", "def");

    ASSERT_EQ(expected_len, utest_fixture->s.len);
    ASSERT_STREQ(expected, utest_fixture->s.text);
}

UTEST_F(TestString, replace_cstr_remove_char) {
    const char source[] = "unhappiness";

    const char expected[] = "unhainess";
    const u64 expected_len = sizeof(expected) / sizeof(char) - 1;

    utest_fixture->s = string_from_cstr(source);
    string_replace_cstr(&utest_fixture->s, "pp", NULL);

    ASSERT_EQ(expected_len, utest_fixture->s.len);
    ASSERT_STREQ(expected, utest_fixture->s.text);
}

UTEST_F(TestString, replace_cstr) {
    const char source[] = "unhappiness";

    const char expected[] = "unhaPPPPiness";
    const u64 expected_len = sizeof(expected) / sizeof(char) - 1;

    utest_fixture->s = string_from_cstr(source);
    string_replace_cstr(&utest_fixture->s, "pp", "PPPP");

    ASSERT_EQ(expected_len, utest_fixture->s.len);
    ASSERT_STREQ(expected, utest_fixture->s.text);
}

// replace str

UTEST_F(TestString, replace_str_empty_source) {
    utest_fixture->s = string_from_cstr(NULL);

    String find = (String){ .text = "abc", .len = 3 };
    String rep = (String){ .text = "def", .len = 3 };
    string_replace_str(&utest_fixture->s, &find, &rep);

    ASSERT_EQ((u64)0, utest_fixture->s.len);
    ASSERT_EQ(NULL, utest_fixture->s.text);
}

UTEST_F(TestString, replace_str_not_existing) {
    const char source[] = "unhappiness";

    const char expected[] = "unhappiness";
    const u64 expected_len = sizeof(expected) / sizeof(char) - 1;

    utest_fixture->s = string_from_cstr(source);

    String find = (String){ .text = "abc", .len = 3 };
    String rep = (String){ .text = "def", .len = 3 };
    string_replace_str(&utest_fixture->s, &find, &rep);

    ASSERT_EQ(expected_len, utest_fixture->s.len);
    ASSERT_STREQ(expected, utest_fixture->s.text);
}

UTEST_F(TestString, replace_str_remove_char) {
    const char source[] = "unhappiness";

    const char expected[] = "unhainess";
    const u64 expected_len = sizeof(expected) / sizeof(char) - 1;

    utest_fixture->s = string_from_cstr(source);

    String find = (String){ .text = "pp", .len = 2 };
    String rep = STRING_EMPTY;
    string_replace_str(&utest_fixture->s, &find, &rep);

    ASSERT_EQ(expected_len, utest_fixture->s.len);
    ASSERT_STREQ(expected, utest_fixture->s.text);
}

UTEST_F(TestString, replace_str) {
    const char source[] = "unhappiness";

    const char expected[] = "unhaPPPPiness";
    const u64 expected_len = sizeof(expected) / sizeof(char) - 1;

    utest_fixture->s = string_from_cstr(source);

    String find = (String){ .text = "pp", .len = 2 };
    String rep = (String){ .text = "PPPP", .len = 4 };
    string_replace_str(&utest_fixture->s, &find, &rep);

    ASSERT_EQ(expected_len, utest_fixture->s.len);
    ASSERT_STREQ(expected, utest_fixture->s.text);
}

// is empty

UTEST_F(TestString, is_empty_not_empty) {
    utest_fixture->s = string_from_cstr("unhappiness");

    ASSERT_FALSE(string_is_empty(&utest_fixture->s));
}

UTEST_F(TestString, is_empty) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_TRUE(string_is_empty(&utest_fixture->s));
}

// eq cstr

UTEST_F(TestString, eq_cstr_source_empty) {
    utest_fixture->s = STRING_EMPTY;

    const char* cstr = "abc";
    ASSERT_FALSE(string_eq_cstr(&utest_fixture->s, cstr));
}

UTEST_F(TestString, eq_cstr_null) {
    utest_fixture->s = string_from_cstr("abcdef");

    ASSERT_FALSE(string_eq_cstr(&utest_fixture->s, NULL));
}

UTEST_F(TestString, eq_cstr_not_equal) {
    utest_fixture->s = string_from_cstr("abcdef");

    const char* cstr = "abc";
    ASSERT_FALSE(string_eq_cstr(&utest_fixture->s, cstr));
}

UTEST_F(TestString, eq_cstr) {
    utest_fixture->s = string_from_cstr("abc");

    const char* cstr = "abc";
    ASSERT_TRUE(string_eq_cstr(&utest_fixture->s, cstr));
}

// eq str

UTEST_F(TestString, eq_str_source_empty) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_FALSE(string_eq_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, eq_str_empty) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_FALSE(string_eq_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, eq_str_not_equal) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = string_from_cstr("def");

    ASSERT_FALSE(string_eq_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, eq_str) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_TRUE(string_eq_str(&utest_fixture->s1, &utest_fixture->s2));
}

// has prefix cstr

UTEST_F(TestString, has_prefix_cstr_empty_source) {
    utest_fixture->s = STRING_EMPTY;

    const char* cstr = "abc";
    ASSERT_FALSE(string_has_prefix_cstr(&utest_fixture->s, cstr));
}

UTEST_F(TestString, has_prefix_cstr_null) {
    utest_fixture->s = string_from_cstr("unhappiness");

    const char* cstr = NULL;
    ASSERT_FALSE(string_has_prefix_cstr(&utest_fixture->s, cstr));
}

UTEST_F(TestString, has_prefix_cstr_not_valid) {
    utest_fixture->s = string_from_cstr("unhappiness");

    const char* cstr = "unhh";
    ASSERT_FALSE(string_has_prefix_cstr(&utest_fixture->s, cstr));
}

UTEST_F(TestString, has_prefix_cstr) {
    utest_fixture->s = string_from_cstr("unhappiness");

    const char* cstr = "un";
    ASSERT_TRUE(string_has_prefix_cstr(&utest_fixture->s, cstr));
}
// has prefix str

UTEST_F(TestString, has_prefix_str_empty_source) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_FALSE(string_has_prefix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, has_prefix_str_null) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_FALSE(string_has_prefix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, has_prefix_str_not_valid) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_from_cstr("unhh");

    ASSERT_FALSE(string_has_prefix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, has_prefix_str) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_from_cstr("un");

    ASSERT_TRUE(string_has_prefix_str(&utest_fixture->s1, &utest_fixture->s2));
}

// has suffix cstr

UTEST_F(TestString, has_suffix_cstr_empty_source) {
    utest_fixture->s = STRING_EMPTY;

    const char* cstr = "abc";
    ASSERT_FALSE(string_has_suffix_cstr(&utest_fixture->s, cstr));
}

UTEST_F(TestString, has_suffix_cstr_null) {
    utest_fixture->s = string_from_cstr("unhappiness");

    const char* cstr = NULL;
    ASSERT_FALSE(string_has_suffix_cstr(&utest_fixture->s, cstr));
}

UTEST_F(TestString, has_suffix_cstr_not_valid) {
    utest_fixture->s = string_from_cstr("unhappiness");

    const char* cstr = "nesss";
    ASSERT_FALSE(string_has_suffix_cstr(&utest_fixture->s, cstr));
}

UTEST_F(TestString, has_suffix_cstr) {
    utest_fixture->s = string_from_cstr("unhappiness");

    const char* cstr = "ness";
    ASSERT_TRUE(string_has_suffix_cstr(&utest_fixture->s, cstr));
}
// has suffix str

UTEST_F(TestString, has_suffix_str_empty_source) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_FALSE(string_has_suffix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, has_suffix_str_null) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_FALSE(string_has_suffix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, has_suffix_str_not_valid) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_from_cstr("nesss");

    ASSERT_FALSE(string_has_suffix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, has_suffix_str) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_from_cstr("ness");

    ASSERT_TRUE(string_has_suffix_str(&utest_fixture->s1, &utest_fixture->s2));
}

// find c

UTEST_F(TestString, find_c_empty_source) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_EQ(STRING_NPOS, string_find_c(&utest_fixture->s, 'a'));
}

UTEST_F(TestString, find_c_not_valid) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(STRING_NPOS, string_find_c(&utest_fixture->s, 'z'));
}

UTEST_F(TestString, find_c) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ((u64)0, string_find_c(&utest_fixture->s, 'a'));
}

// find cstr

UTEST_F(TestString, find_cstr_empty_source) {
    utest_fixture->s = STRING_EMPTY;

    const char* find = "abc";
    ASSERT_EQ(STRING_NPOS, string_find_cstr(&utest_fixture->s, find));
}

UTEST_F(TestString, find_cstr_null) {
    utest_fixture->s = string_from_cstr("abaacdef");

    const char* find = NULL;
    ASSERT_EQ(STRING_NPOS, string_find_cstr(&utest_fixture->s, find));
}

UTEST_F(TestString, find_cstr_not_valid) {
    utest_fixture->s = string_from_cstr("abaacdef");

    const char* find = "z";
    ASSERT_EQ(STRING_NPOS, string_find_cstr(&utest_fixture->s, find));
}

UTEST_F(TestString, find_cstr) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ((u64)2, string_find_cstr(&utest_fixture->s, "aa"));
}

// find str

UTEST_F(TestString, find_str_empty_source) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_EQ(STRING_NPOS, string_find_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, find_str_empty) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_EQ(STRING_NPOS, string_find_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, find_str_not_valid) {
    utest_fixture->s1 = string_from_cstr("abaacdef");
    utest_fixture->s2 = string_from_cstr("za");

    ASSERT_EQ(STRING_NPOS, string_find_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, find_str) {
    utest_fixture->s1 = string_from_cstr("abaacdef");
    utest_fixture->s2 = string_from_cstr("aa");

    ASSERT_EQ((u64)2, string_find_str(&utest_fixture->s1, &utest_fixture->s2));
}

// find last c

UTEST_F(TestString, find_last_c_empty_source) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_EQ(STRING_NPOS, string_find_last_c(&utest_fixture->s, 'a'));
}

UTEST_F(TestString, find_last_c_not_valid) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(STRING_NPOS, string_find_last_c(&utest_fixture->s, 'z'));
}

UTEST_F(TestString, find_last_c) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ((u64)3, string_find_last_c(&utest_fixture->s, 'a'));
}

// find last cstr

UTEST_F(TestString, find_last_cstr_empty_source) {
    utest_fixture->s = STRING_EMPTY;

    const char* find = "abc";
    ASSERT_EQ(STRING_NPOS, string_find_last_cstr(&utest_fixture->s, find));
}

UTEST_F(TestString, find_last_cstr_null) {
    utest_fixture->s = string_from_cstr("abaacdef");

    const char* find = NULL;
    ASSERT_EQ(STRING_NPOS, string_find_last_cstr(&utest_fixture->s, find));
}

UTEST_F(TestString, find_last_cstr_not_valid) {
    utest_fixture->s = string_from_cstr("abaacdef");

    const char* find = "z";
    ASSERT_EQ(STRING_NPOS, string_find_last_cstr(&utest_fixture->s, find));
}

UTEST_F(TestString, find_last_cstr) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ((u64)2, string_find_last_cstr(&utest_fixture->s, "aa"));
}

// find str

UTEST_F(TestString, find_last_str_empty_source) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_EQ(STRING_NPOS, string_find_last_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, find_last_str_empty) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_EQ(STRING_NPOS, string_find_last_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, find_last_str_not_valid) {
    utest_fixture->s1 = string_from_cstr("abaacdef");
    utest_fixture->s2 = string_from_cstr("za");

    ASSERT_EQ(STRING_NPOS, string_find_last_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, find_last_str) {
    utest_fixture->s1 = string_from_cstr("abaacdefaa");
    utest_fixture->s2 = string_from_cstr("aa");

    ASSERT_EQ((u64)8, string_find_last_str(&utest_fixture->s1, &utest_fixture->s2));
}