#include <utest/utest.h>

#include <vane/scanner/token_stream.h>

struct TestTokenStream {
    String path;
    String content;
    TokenStream ts;
};

UTEST_F_SETUP(TestTokenStream) {
    utest_fixture->path = STRING_EMPTY;
    utest_fixture->content = STRING_EMPTY;
    utest_fixture->ts = (TokenStream){ 0 };
}

UTEST_F_TEARDOWN(TestTokenStream) {
    string_destroy(&utest_fixture->content);
    ts_destroy(&utest_fixture->ts);
}

// get curr

UTEST_F(TestTokenStream, get_curr_empty_source) {
    utest_fixture->content = STRING_EMPTY;
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    const Token* token = ts_get_curr(&utest_fixture->ts);

    ASSERT_EQ(NULL, token);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, get_curr) {
    utest_fixture->content = string_from_cstr("+");
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    const Token* token = ts_get_curr(&utest_fixture->ts);

    ASSERT_EQ(NULL, token);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

// peek next

UTEST_F(TestTokenStream, peek_next_empty_source) {
    utest_fixture->content = STRING_EMPTY;
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    const Token* token = ts_peek_next(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_EOF_, token->kind);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, peek_next) {
    utest_fixture->content = string_from_cstr("+");
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    const Token* token = ts_peek_next(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_PLUS, token->kind);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

// advance

UTEST_F(TestTokenStream, advance_empty_source) {
    utest_fixture->content = STRING_EMPTY;
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    const Token* token = ts_advance(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_EOF_, token->kind);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, advance_1) {
    utest_fixture->content = string_from_cstr("+");
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    const Token* token = ts_advance(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_PLUS, token->kind);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, advance_2) {
    utest_fixture->content = string_from_cstr("+");
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    const Token* token = ts_advance(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_PLUS, token->kind);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);

    token = ts_advance(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_EOF_, token->kind);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(1, utest_fixture->ts.idx);
    ASSERT_EQ(2, utest_fixture->ts.tokens.size);
}

// move forward

UTEST_F(TestTokenStream, move_forward_empty_source) {
    utest_fixture->content = STRING_EMPTY;
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    ts_move_forward(&utest_fixture->ts);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, move_forward_1) {
    utest_fixture->content = string_from_cstr("+");
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    ts_move_forward(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, move_forward_2) {
    utest_fixture->content = string_from_cstr("+");
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    ts_move_forward(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);

    ts_move_forward(&utest_fixture->ts);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(1, utest_fixture->ts.idx);
    ASSERT_EQ(2, utest_fixture->ts.tokens.size);
}

// move back

UTEST_F(TestTokenStream, move_back_empty_source) {
    utest_fixture->content = STRING_EMPTY;
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    ts_move_back(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(0, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, move_back_1) {
    utest_fixture->content = string_from_cstr("+");
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    ts_move_back(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(0, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, move_back_2) {
    utest_fixture->content = string_from_cstr("+");
    utest_fixture->ts = ts_create(0, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len);

    ts_move_forward(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);

    ts_move_back(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}
