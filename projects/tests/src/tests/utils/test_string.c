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
        String tmp1;
    };
    union {
        String tmp2;
        String s3;
    };
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

// -- Creation --

UTEST_F(TestString, string_from_cstr1) {
    utest_fixture->s = string_from_cstr(NULL);
    ASSERT_TRUE(is_string_empty(&utest_fixture->s));
}

UTEST_F(TestString, string_from_cstr2) {
    utest_fixture->s = string_from_cstr("");
    ASSERT_TRUE(is_string_empty(&utest_fixture->s));
}

UTEST_F(TestString, string_from_cstr3) {
    utest_fixture->s = string_from_cstr("abc");

    ASSERT_EQ(3, utest_fixture->s.len);
    ASSERT_STREQ("abc", utest_fixture->s.text);
}

UTEST_F(TestString, string_from_format1) {
    utest_fixture->s = string_from_format("");
    ASSERT_TRUE(is_string_empty(&utest_fixture->s));
}

UTEST_F(TestString, string_from_format2) {
    utest_fixture->s = string_from_format("%s", "abc");

    ASSERT_EQ(3, utest_fixture->s.len);
    ASSERT_STREQ("abc", utest_fixture->s.text);
}

UTEST_F(TestString, string_clone1) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_clone(&utest_fixture->s1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->s2));
}

UTEST_F(TestString, string_clone2) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = string_clone(&utest_fixture->s1);

    ASSERT_EQ(utest_fixture->s1.len, utest_fixture->s2.len);
    ASSERT_STREQ(utest_fixture->s1.text, utest_fixture->s2.text);
}

UTEST_F(TestString, string_substr1) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_substr(&utest_fixture->s1, 0, 10);

    ASSERT_TRUE(is_string_empty(&utest_fixture->s2));
}

UTEST_F(TestString, string_substr2) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_substr(&utest_fixture->s1, 11, 10);

    ASSERT_TRUE(is_string_empty(&utest_fixture->s2));
}

UTEST_F(TestString, string_substr3) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_substr(&utest_fixture->s1, 0, 0);

    ASSERT_TRUE(is_string_empty(&utest_fixture->s2));
}

UTEST_F(TestString, string_substr4) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_substr(&utest_fixture->s1, 0, 25);

    ASSERT_EQ(11, utest_fixture->s2.len);
    ASSERT_STREQ("unhappiness", utest_fixture->s2.text);
}

UTEST_F(TestString, string_concat1) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = STRING_EMPTY;
    utest_fixture->s3 = string_concat(&utest_fixture->s1, &utest_fixture->s2);

    ASSERT_TRUE(is_string_empty(&utest_fixture->s2));
}

UTEST_F(TestString, string_concat2) {
    utest_fixture->s1 = string_from_cstr("unhap");
    utest_fixture->s2 = STRING_EMPTY;
    utest_fixture->s3 = string_concat(&utest_fixture->s1, &utest_fixture->s2);

    ASSERT_EQ(5, utest_fixture->s3.len);
    ASSERT_STREQ("unhap", utest_fixture->s3.text);
}

UTEST_F(TestString, string_concat3) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("piness");
    utest_fixture->s3 = string_concat(&utest_fixture->s1, &utest_fixture->s2);

    ASSERT_EQ(6, utest_fixture->s3.len);
    ASSERT_STREQ("piness", utest_fixture->s3.text);
}

UTEST_F(TestString, string_concat4) {
    utest_fixture->s1 = string_from_cstr("unhap");
    utest_fixture->s2 = string_from_cstr("piness");
    utest_fixture->s3 = string_concat(&utest_fixture->s1, &utest_fixture->s2);

    ASSERT_EQ(11, utest_fixture->s3.len);
    ASSERT_STREQ("unhappiness", utest_fixture->s3.text);
}

// -- Replacment --


UTEST_F(TestString, string_replace_c1) {
    utest_fixture->s = string_from_cstr(NULL);
    string_replace_c(&utest_fixture->s, '#', '!');

    ASSERT_TRUE(is_string_empty(&utest_fixture->s2));
}

UTEST_F(TestString, string_replace_c2) {
    utest_fixture->s = string_from_cstr("unhappiness");
    string_replace_c(&utest_fixture->s, '#', '!');

    ASSERT_EQ(11, utest_fixture->s.len);
    ASSERT_STREQ("unhappiness", utest_fixture->s.text);
}

UTEST_F(TestString, string_replace_c3) {
    utest_fixture->s = string_from_cstr("unhappiness");
    string_replace_c(&utest_fixture->s, 'p', 'P');

    ASSERT_EQ(11, utest_fixture->s.len);
    ASSERT_STREQ("unhaPPiness", utest_fixture->s.text);
}

UTEST_F(TestString, string_replace_cstr1) {
    utest_fixture->s = string_from_cstr(NULL);
    string_replace_cstr(&utest_fixture->s, "abc", "def");

    ASSERT_TRUE(is_string_empty(&utest_fixture->s2));
}

UTEST_F(TestString, string_replace_cstr2) {
    utest_fixture->s = string_from_cstr("unhappiness");
    string_replace_cstr(&utest_fixture->s, "abc", "def");

    ASSERT_EQ(11, utest_fixture->s.len);
    ASSERT_STREQ("unhappiness", utest_fixture->s.text);
}

UTEST_F(TestString, string_replace_cstr3) {
    utest_fixture->s = string_from_cstr("unhappiness");
    string_replace_cstr(&utest_fixture->s, "pp", NULL);

    ASSERT_EQ(9, utest_fixture->s.len);
    ASSERT_STREQ("unhainess", utest_fixture->s.text);
}

UTEST_F(TestString, string_replace_cstr4) {
    utest_fixture->s = string_from_cstr("unhappiness");
    string_replace_cstr(&utest_fixture->s, "pp", "PPPP");

    ASSERT_EQ(13, utest_fixture->s.len);
    ASSERT_STREQ("unhaPPPPiness", utest_fixture->s.text);
}


UTEST_F(TestString, string_replace_str1) {
    utest_fixture->s = string_from_cstr(NULL);

    String find = (String){ .text = "abc", .len = 3 };
    String rep = (String){ .text = "def", .len = 3 };

    string_replace_str(&utest_fixture->s, &find, &rep);

    ASSERT_TRUE(is_string_empty(&utest_fixture->s2));
}

UTEST_F(TestString, string_replace_str2) {
    utest_fixture->s = string_from_cstr("unhappiness");

    String find = (String){ .text = "abc", .len = 3 };
    String rep = (String){ .text = "def", .len = 3 };

    string_replace_str(&utest_fixture->s, &find, &rep);

    ASSERT_EQ(11, utest_fixture->s.len);
    ASSERT_STREQ("unhappiness", utest_fixture->s.text);
}

UTEST_F(TestString, string_replace_str3) {
    utest_fixture->s = string_from_cstr("unhappiness");

    String find = (String){ .text = "pp", .len = 2 };
    String rep = STRING_EMPTY;

    string_replace_str(&utest_fixture->s, &find, &rep);

    ASSERT_EQ(9, utest_fixture->s.len);
    ASSERT_STREQ("unhainess", utest_fixture->s.text);
}

UTEST_F(TestString, string_replace_str4) {
    utest_fixture->s = string_from_cstr("unhappiness");

    String find = (String){ .text = "pp", .len = 2 };
    String rep = (String){ .text = "PPPP", .len = 4 };

    string_replace_str(&utest_fixture->s, &find, &rep);

    ASSERT_EQ(13, utest_fixture->s.len);
    ASSERT_STREQ("unhaPPPPiness", utest_fixture->s.text);
}

// -- Comparison --

UTEST_F(TestString, string_eq_cstr1) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_FALSE(string_eq_cstr(&utest_fixture->s, "abc"));
}

UTEST_F(TestString, string_eq_cstr2) {
    utest_fixture->s = string_from_cstr("abcdef");

    ASSERT_FALSE(string_eq_cstr(&utest_fixture->s, NULL));
}

UTEST_F(TestString, string_eq_cstr3) {
    utest_fixture->s = string_from_cstr("abcdef");

    ASSERT_FALSE(string_eq_cstr(&utest_fixture->s, "abc"));
}

UTEST_F(TestString, string_eq_cstr4) {
    utest_fixture->s = string_from_cstr("abc");

    ASSERT_TRUE(string_eq_cstr(&utest_fixture->s, "abc"));
}

UTEST_F(TestString, string_eq_str1) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_FALSE(string_eq_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_eq_str2) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_FALSE(string_eq_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_eq_str3) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = string_from_cstr("def");

    ASSERT_FALSE(string_eq_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_eq_str4) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_TRUE(string_eq_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_has_prefix_cstr1) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_FALSE(string_has_prefix_cstr(&utest_fixture->s, "abc"));
}

UTEST_F(TestString, string_has_prefix_cstr2) {
    utest_fixture->s = string_from_cstr("unhappiness");

    ASSERT_FALSE(string_has_prefix_cstr(&utest_fixture->s, NULL));
}

UTEST_F(TestString, string_has_prefix_cstr3) {
    utest_fixture->s = string_from_cstr("unhappiness");

    ASSERT_FALSE(string_has_prefix_cstr(&utest_fixture->s, "unhh"));
}

UTEST_F(TestString, string_has_prefix_cstr4) {
    utest_fixture->s = string_from_cstr("unhappiness");

    ASSERT_TRUE(string_has_prefix_cstr(&utest_fixture->s, "un"));
}


UTEST_F(TestString, string_has_prefix_str1) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_FALSE(string_has_prefix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_has_prefix_str2) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_FALSE(string_has_prefix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_has_prefix_str3) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_from_cstr("unhh");

    ASSERT_FALSE(string_has_prefix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_has_prefix_str4) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_from_cstr("un");

    ASSERT_TRUE(string_has_prefix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_has_suffix_cstr1) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_FALSE(string_has_suffix_cstr(&utest_fixture->s, "abc"));
}

UTEST_F(TestString, string_has_suffix_cstr2) {
    utest_fixture->s = string_from_cstr("unhappiness");

    ASSERT_FALSE(string_has_suffix_cstr(&utest_fixture->s, NULL));
}

UTEST_F(TestString, string_has_suffix_cstr3) {
    utest_fixture->s = string_from_cstr("unhappiness");

    ASSERT_FALSE(string_has_suffix_cstr(&utest_fixture->s, "nesss"));
}

UTEST_F(TestString, string_has_suffix_cstr4) {
    utest_fixture->s = string_from_cstr("unhappiness");

    ASSERT_TRUE(string_has_suffix_cstr(&utest_fixture->s, "ness"));
}

UTEST_F(TestString, string_has_suffix_str1) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_FALSE(string_has_suffix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_has_suffix_str2) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_FALSE(string_has_suffix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_has_suffix_str3) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_from_cstr("nesss");

    ASSERT_FALSE(string_has_suffix_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_has_suffix_str4) {
    utest_fixture->s1 = string_from_cstr("unhappiness");
    utest_fixture->s2 = string_from_cstr("ness");

    ASSERT_TRUE(string_has_suffix_str(&utest_fixture->s1, &utest_fixture->s2));
}

// -- Search --

UTEST_F(TestString, string_find_c1) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_EQ(NPOS, string_find_c(&utest_fixture->s, 'a'));
}

UTEST_F(TestString, string_find_c2) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(NPOS, string_find_c(&utest_fixture->s, 'z'));
}

UTEST_F(TestString, string_find_c3) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(0, string_find_c(&utest_fixture->s, 'a'));
}

UTEST_F(TestString, string_find_cstr1) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_EQ(NPOS, string_find_cstr(&utest_fixture->s, "abc"));
}

UTEST_F(TestString, string_find_cstr2) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(NPOS, string_find_cstr(&utest_fixture->s, NULL));
}

UTEST_F(TestString, string_find_cstr3) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(NPOS, string_find_cstr(&utest_fixture->s, "z"));
}

UTEST_F(TestString, string_find_cstr4) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(2, string_find_cstr(&utest_fixture->s, "aa"));
}

UTEST_F(TestString, string_find_str1) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_EQ(NPOS, string_find_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_find_str2) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_EQ(NPOS, string_find_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_find_str3) {
    utest_fixture->s1 = string_from_cstr("abaacdef");
    utest_fixture->s2 = string_from_cstr("za");

    ASSERT_EQ(NPOS, string_find_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_find_str4) {
    utest_fixture->s1 = string_from_cstr("abaacdef");
    utest_fixture->s2 = string_from_cstr("aa");

    ASSERT_EQ(2, string_find_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_find_last_c1) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_EQ(NPOS, string_find_last_c(&utest_fixture->s, 'a'));
}

UTEST_F(TestString, string_find_last_c2) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(NPOS, string_find_last_c(&utest_fixture->s, 'z'));
}

UTEST_F(TestString, string_find_last_c3) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(3, string_find_last_c(&utest_fixture->s, 'a'));
}

UTEST_F(TestString, string_find_last_cstr1) {
    utest_fixture->s = STRING_EMPTY;

    ASSERT_EQ(NPOS, string_find_last_cstr(&utest_fixture->s, "abc"));
}

UTEST_F(TestString, string_find_last_cstr2) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(NPOS, string_find_last_cstr(&utest_fixture->s, NULL));
}

UTEST_F(TestString, string_find_last_cstr3) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(NPOS, string_find_last_cstr(&utest_fixture->s, "z"));
}

UTEST_F(TestString, string_find_last_cstr4) {
    utest_fixture->s = string_from_cstr("abaacdef");

    ASSERT_EQ(2, string_find_last_cstr(&utest_fixture->s, "aa"));
}

UTEST_F(TestString, string_find_last_str1) {
    utest_fixture->s1 = STRING_EMPTY;
    utest_fixture->s2 = string_from_cstr("abc");

    ASSERT_EQ(NPOS, string_find_last_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_find_last_str2) {
    utest_fixture->s1 = string_from_cstr("abc");
    utest_fixture->s2 = STRING_EMPTY;

    ASSERT_EQ(NPOS, string_find_last_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_find_last_str3) {
    utest_fixture->s1 = string_from_cstr("abaacdef");
    utest_fixture->s2 = string_from_cstr("za");

    ASSERT_EQ(NPOS, string_find_last_str(&utest_fixture->s1, &utest_fixture->s2));
}

UTEST_F(TestString, string_find_last_str4) {
    utest_fixture->s1 = string_from_cstr("abaacdefaa");
    utest_fixture->s2 = string_from_cstr("aa");

    ASSERT_EQ(8, string_find_last_str(&utest_fixture->s1, &utest_fixture->s2));
}