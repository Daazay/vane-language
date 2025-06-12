#include <utest/utest.h>

#include <vane/scanner/token_stream.h>

struct TestTokenStream {
    ReportCollector rc;
    FileContent fc;
    TokenStream ts;
};

UTEST_F_SETUP(TestTokenStream) {
    utest_fixture->rc = (ReportCollector){ 0 };
    utest_fixture->fc = (FileContent){ 0 };
    utest_fixture->ts = (TokenStream){ 0 };
}

UTEST_F_TEARDOWN(TestTokenStream) {
    file_content_destroy(&utest_fixture->fc);
    token_stream_destroy(&utest_fixture->ts);
}

// get curr

UTEST_F(TestTokenStream, get_curr_empty_source) {
    String tmp = STRING_EMPTY;
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    const Token* token = token_stream_get_curr(&utest_fixture->ts);

    ASSERT_EQ(NULL, token);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, get_curr) {
    String tmp = string_from_cstr("+");
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    const Token* token = token_stream_get_curr(&utest_fixture->ts);

    ASSERT_EQ(NULL, token);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

// peek next

UTEST_F(TestTokenStream, peek_next_empty_source) {
    String tmp = STRING_EMPTY;
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    const Token* token = token_stream_peek_next(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_EOF_, token->kind);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, peek_next) {
    String tmp = string_from_cstr("+");
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    const Token* token = token_stream_peek_next(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_PLUS, token->kind);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

// advance

UTEST_F(TestTokenStream, advance_empty_source) {
    String tmp = STRING_EMPTY;
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    const Token* token = token_stream_advance(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_EOF_, token->kind);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, advance_1) {
    String tmp = string_from_cstr("+");
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    const Token* token = token_stream_advance(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_PLUS, token->kind);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, advance_2) {
    String tmp = string_from_cstr("+");
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    const Token* token = token_stream_advance(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_PLUS, token->kind);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);

    token = token_stream_advance(&utest_fixture->ts);

    ASSERT_NE(NULL, token);
    ASSERT_EQ(TOKEN_EOF_, token->kind);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(1, utest_fixture->ts.idx);
    ASSERT_EQ(2, utest_fixture->ts.tokens.size);
}

// move forward

UTEST_F(TestTokenStream, move_forward_empty_source) {
    String tmp = STRING_EMPTY;
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    token_stream_move_forward(&utest_fixture->ts);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, move_forward_1) {
    String tmp = string_from_cstr("+");
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    token_stream_move_forward(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}

UTEST_F(TestTokenStream, move_forward_2) {
    String tmp = string_from_cstr("+");
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    token_stream_move_forward(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);

    token_stream_move_forward(&utest_fixture->ts);

    ASSERT_TRUE(utest_fixture->ts.done);
    ASSERT_EQ(1, utest_fixture->ts.idx);
    ASSERT_EQ(2, utest_fixture->ts.tokens.size);
}

// move back

UTEST_F(TestTokenStream, move_back) {
    String tmp = string_from_cstr("+");
    utest_fixture->fc.content = (byte*)tmp.text;
    utest_fixture->fc.size = tmp.len;
    utest_fixture->ts = token_stream_create(0, &utest_fixture->fc, &utest_fixture->rc);

    token_stream_move_forward(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(0, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);

    token_stream_move_back(&utest_fixture->ts);

    ASSERT_FALSE(utest_fixture->ts.done);
    ASSERT_EQ(-1, utest_fixture->ts.idx);
    ASSERT_EQ(1, utest_fixture->ts.tokens.size);
}
