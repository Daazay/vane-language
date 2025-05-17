#include <utest/utest.h>

#include <vane/scanner/scanner.h>

struct TestScanner {
    String path;
    String content;
    Token token;
};

UTEST_F_SETUP(TestScanner) {
    utest_fixture->path = STRING_EMPTY;
    utest_fixture->content = STRING_EMPTY;
    utest_fixture->token = (Token){ 0 };
}

UTEST_F_TEARDOWN(TestScanner) {
    token_destroy(&utest_fixture->token);
    string_destroy(&utest_fixture->path);
    string_destroy(&utest_fixture->content);
}


UTEST_F(TestScanner, scan_empty_source) {
    utest_fixture->content = STRING_EMPTY;
    Scanner scanner = scanner_create(&utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);
    utest_fixture->token = scanner_scan_next(&scanner);

    ASSERT_EQ(TOKEN_EOF_, utest_fixture->token.kind);
}

// PUNCTUATORS

UTEST_F(TestScanner, is_all_punctuators_implemented) {
    const Token expected[] = {
        #define TOKEN_PUNC(ID, NAME, VALUE) \
        (Token){ .kind = TOKEN_##ID, .value = STRING_EMPTY, },
#include "vane/scanner/token_kind.def"
    };
    const u32 expected_len = sizeof(expected) / sizeof(Token);

    utest_fixture->content = string_from_cstr(""
#define TOKEN_PUNC(ID, NAME, VALUE) VALUE" "
#include "vane/scanner/token_kind.def"
    );

    Scanner scanner = scanner_create(&utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    for (u32 i = 0; i < expected_len; ++i) {
        utest_fixture->token = scanner_scan_next(&scanner);

        ASSERT_EQ_MSG(expected[i].kind, utest_fixture->token.kind, get_token_kind_name(expected[i].kind));
    }
}

// KEYWORDS

UTEST_F(TestScanner, is_all_keywords_implemented) {
    const Token expected[] = {
        #define TOKEN_KEYWORD(ID, NAME) \
        (Token){ .kind = TOKEN_KEYWORD_##ID, .value = STRING_EMPTY, },
#include "vane/scanner/token_kind.def"
    };
    const u32 expected_len = sizeof(expected) / sizeof(Token);

    utest_fixture->content = string_from_cstr(""
#define TOKEN_KEYWORD(ID, NAME) NAME" "
#include "vane/scanner/token_kind.def"
    );

    Scanner scanner = scanner_create(&utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    for (u32 i = 0; i < expected_len; ++i) {
        utest_fixture->token = scanner_scan_next(&scanner);

        ASSERT_EQ_MSG(expected[i].kind, utest_fixture->token.kind, get_token_kind_name(expected[i].kind));
    }
}

// LITERALS

UTEST_F(TestScanner, is_all_literals_implemented) {
    const Token expected[] = {
        token_create(TOKEN_LITERAL_STRING, (String) { .text = "hello, world", .len = 12 }, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_STRING, (String) { .text = "hello,\n world", .len = 13 }, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_CHAR, (String) { .text = "a", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_DEC, (String) { .text = "12345", .len = 5 }, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_HEX, (String) { .text = "0x1aF", .len = 5 }, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_BIN, (String) { .text = "0b1010", .len = 6 }, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_BOOL, (String) { .text = "true", .len = 4 }, (SourceLoc) { 0 }),
    };
    const u32 expected_len = sizeof(expected) / sizeof(Token);

    utest_fixture->content = string_from_cstr(""
        "\"hello, world\" "
        "`hello,\n world` "
        "\'a\' "
        "12345 "
        "0x1aF "
        "0b1010 "
        "true "
    );

    Scanner scanner = scanner_create(&utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    for (u32 i = 0; i < expected_len; ++i) {
        utest_fixture->token = scanner_scan_next(&scanner);

        ASSERT_EQ_MSG(expected[i].kind, utest_fixture->token.kind, get_token_kind_name(expected[i].kind));
        ASSERT_TRUE_MSG(string_eq_str(&expected[i].value, &utest_fixture->token.value), get_token_kind_name(expected[i].kind));

        token_destroy(&utest_fixture->token);
    }
}

// IDENTIFIER

UTEST_F(TestScanner, identifier) {
    const Token expected[] = {
       token_create(TOKEN_IDENTIFIER, (String) { .text = "variable_name", .len = 13 }, (SourceLoc) { 0 }),
       token_create(TOKEN_IDENTIFIER, (String) { .text = "_var", .len = 4 }, (SourceLoc) { 0 }),
       token_create(TOKEN_IDENTIFIER, (String) { .text = "_", .len = 1 }, (SourceLoc) { 0 }),
       token_create(TOKEN_IDENTIFIER, (String) { .text = "var123", .len = 6 }, (SourceLoc) { 0 }),
       token_create(TOKEN_INVALID, (String) { .text = "123var", .len = 6 }, (SourceLoc) { 0 }),
       token_create(TOKEN_UNKNOWN, (String) { .text = "@#", .len = 2 }, (SourceLoc) { 0 }),
    };
    const u32 expected_len = sizeof(expected) / sizeof(Token);

    utest_fixture->content = string_from_cstr(""
        "variable_name "
        "_var "
        "_ "
        "var123 "
        "123var "
        "@# "
    );

    Scanner scanner = scanner_create(&utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    for (u32 i = 0; i < expected_len; ++i) {
        utest_fixture->token = scanner_scan_next(&scanner);

        ASSERT_EQ_MSG(expected[i].kind, utest_fixture->token.kind, get_token_kind_name(expected[i].kind));
        ASSERT_TRUE_MSG(string_eq_str(&expected[i].value, &utest_fixture->token.value), get_token_kind_name(expected[i].kind));

        token_destroy(&utest_fixture->token);
    }
}

// COMMENTS

UTEST_F(TestScanner, comments) {
    utest_fixture->content = string_from_cstr(""
        "// line comment\n"
        "/* block\n"
        "/* nested block */"
        "// nested line\n"
        "comment */"
    );

    Scanner scanner = scanner_create(&utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);
    utest_fixture->token = scanner_scan_next(&scanner);

    ASSERT_EQ(TOKEN_EOF_, utest_fixture->token.kind);
}

// COMPLEX

UTEST_F(TestScanner, complex1) {
    utest_fixture->content = string_from_cstr(""
        "a += (b[i] * 0x10) >> 3 \n"
        "if x <= 0xFFFF && y != nil\n"
        "func(a, b...) || (c && d) \n"
    );

    const Token expected[] = {
        token_create(TOKEN_IDENTIFIER, (String) { .text = "a", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_PLUS_EQUAL, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_L_BRACE, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "b", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_L_BRACKET, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "i", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_R_BRACKET, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_STAR, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_HEX, (String) { .text = "0x10", .len = 4 }, (SourceLoc) { 0 }),
        token_create(TOKEN_R_BRACE, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_GREATER_GREATER, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_DEC, (String) { .text = "3", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_KEYWORD_IF, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "x", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_LESS_EQUAL, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_LITERAL_HEX, (String) { .text = "0xFFFF", .len = 6 }, (SourceLoc) { 0 }),
        token_create(TOKEN_AMP_AMP, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "y", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_EXCLAIM_EQUAL, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_KEYWORD_NIL, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "func", .len = 4 }, (SourceLoc) { 0 }),
        token_create(TOKEN_L_BRACE, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "a", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_COMMA, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "b", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_DOT, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_DOT, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_DOT, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_R_BRACE, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_PIPE_PIPE, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_L_BRACE, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "c", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_AMP_AMP, STRING_EMPTY, (SourceLoc) { 0 }),
        token_create(TOKEN_IDENTIFIER, (String) { .text = "d", .len = 1 }, (SourceLoc) { 0 }),
        token_create(TOKEN_R_BRACE, STRING_EMPTY, (SourceLoc) { 0 }),
    };

    const u32 expected_len = sizeof(expected) / sizeof(Token);

    Scanner scanner = scanner_create(&utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    for (u32 i = 0; i < expected_len; ++i) {
        utest_fixture->token = scanner_scan_next(&scanner);

        ASSERT_EQ_MSG(expected[i].kind, utest_fixture->token.kind, get_token_kind_name(expected[i].kind));
        if (string_is_empty(&expected[i].value)) {
            ASSERT_TRUE_MSG(string_is_empty(&utest_fixture->token.value), get_token_kind_name(expected[i].kind));
        }
        else {
            ASSERT_TRUE_MSG(string_eq_str(&expected[i].value, &utest_fixture->token.value), get_token_kind_name(expected[i].kind));
        }

        token_destroy(&utest_fixture->token);
    }
}