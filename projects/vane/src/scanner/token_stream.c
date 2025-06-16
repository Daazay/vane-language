#include "vane/scanner/token_stream.h"

#include "vane/utils/string_builder.h"

#define TOKEN_STREAM_BEGIN_IDX -1

static inline  void token_stream_parse_next(TokenStream* ts) {
    if (!ts->done) {
        Token token = scanner_scan_next(&ts->scanner);
        vector_push_back(&ts->tokens, &token);

        if (token.kind == TOKEN_EOF_) {
            ts->done = true;
        }
    }
}

TokenStream token_stream_create(u32 init_tokens_size, const FileContent* fc, ReportCollector* rc) {
    assert(fc != NULL && rc != NULL);

    u32 cap = (init_tokens_size > 0)
        ? init_tokens_size
        : TOKEN_STREAM_DEFAULT_SIZE;

    return (TokenStream) {
        .scanner = scanner_create(fc, rc),
        .tokens  = vector_create(cap, VECTOR_ITEM_SPECS(Token, &token_destroy)),
        .idx     = TOKEN_STREAM_BEGIN_IDX,
        .done    = false,
    };
}

void token_stream_destroy(TokenStream* ts) {
    if (ts == NULL) {
        return;
    }

    vector_destroy(&ts->tokens);
    scanner_destroy(&ts->scanner);
}

bool token_stream_is_end(const TokenStream* ts) {
    assert(ts != NULL);

    if (ts->done && (ts->idx + 2 >= (i32)ts->tokens.size)) {
        return true;
    }
    return false;
}

void token_stream_move_forward(TokenStream* ts) {
    assert(ts != NULL);

    if (ts->idx + 1 == (i32)ts->tokens.size) {
        assert(!ts->done && "the end of token stream reached");
        token_stream_parse_next(ts);
    }
    ts->idx++;
}

void token_stream_move_back(TokenStream* ts) {
    assert(ts != NULL);

    assert(ts->idx != TOKEN_STREAM_BEGIN_IDX && "index of token stream must be >= -1");
    ts->idx--;
}

const Token* token_stream_get_curr(TokenStream* ts) {
    assert(ts != NULL);

    if (!ts->done && (ts->idx + 1 == (i32)ts->tokens.size)) {
        token_stream_parse_next(ts);
    }

    if (ts->idx < 0) {
        return NULL;
    }

    return vector_at(&ts->tokens, ts->idx);
}

const Token* token_stream_peek_next(TokenStream* ts) {
    assert(ts != NULL);

    if (ts->idx + 1 == (i32)ts->tokens.size) {
        if (ts->done) {
            return NULL;
        }
        token_stream_parse_next(ts);
    }
    return vector_at(&ts->tokens, ts->idx + 1);
}

const Token* token_stream_advance(TokenStream* ts) {
    assert(ts != NULL);

    const Token* token = token_stream_peek_next(ts);
    token_stream_move_forward(ts);

    return token;
}

const Token* token_stream_expect(TokenStream* ts, TokenKind expected) {
    assert(ts != NULL);

    const Token* token = token_stream_peek_next(ts);
    if (token->kind == expected) {
        return token;
    }

    REPORT_COLLECTOR_TRACE_SYNTAX(ts->scanner.rc, token->loc, "Expected any `%s`, but got `%s`", get_token_kind_value(expected), get_token_kind_value(token->kind));

    return token;
}

const Token* token_stream_expect_any_impl(TokenStream* ts, const TokenKind expected[], u32 expected_count) {
    assert(ts != NULL);

    const Token* token = token_stream_peek_next(ts);
    for (u32 i = 0; i < expected_count; ++i) {
        if (token->kind == expected[i]) {
            return token;
        }
    }

    StringBuilder sb = sb_create(32);

    for (u32 i = 0; i < expected_count; ++i) {
        sb_append_format(&sb, "`%s`", get_token_kind_value(expected[i]));
        if (i + 1 < expected_count) {
            sb_append_cstr(&sb, ", ");
        }
    }

    REPORT_COLLECTOR_TRACE_SYNTAX(ts->scanner.rc, token->loc, "Expected any [%.*s], but got `%s`", (i32)sb.len, sb.buf, get_token_kind_value(token->kind));

    sb_destroy(&sb);

    return token;
}

const Token* token_stream_advance_if(TokenStream* ts, TokenKind expected) {
    assert(ts != NULL);

    const Token* token = token_stream_peek_next(ts);
    if (token->kind == expected) {
        token_stream_move_forward(ts);
        // why token_stream_get_curr instead of just returning the token?
        // token_stream_move_forward can cause vector reallcation which would cause that pointer would reference to invalid memory.
        return token_stream_get_curr(ts);
    }

    REPORT_COLLECTOR_TRACE_SYNTAX(ts->scanner.rc, token->loc, "Expected any `%s`, but got `%s`", get_token_kind_value(expected), get_token_kind_value(token->kind));

    return token;
}

const Token* token_stream_advance_if_any_impl(TokenStream* ts, const TokenKind expected[], u32 expected_count) {
    assert(ts != NULL);

    const Token* token = token_stream_peek_next(ts);
    for (u32 i = 0; i < expected_count; ++i) {
        if (token->kind == expected[i]) {
            token_stream_move_forward(ts);
            // why token_stream_get_curr instead of just returning the token?
            // token_stream_move_forward can cause vector reallcation which would cause that pointer would reference to invalid memory.
            return token_stream_get_curr(ts);
        }
    }

    StringBuilder sb = sb_create(32);

    for (u32 i = 0; i < expected_count; ++i) {
        sb_append_format(&sb, "`%s`", get_token_kind_value(expected[i]));
        if (i + 1 < expected_count) {
            sb_append_cstr(&sb, ", ");
        }
    }

    REPORT_COLLECTOR_TRACE_SYNTAX(ts->scanner.rc, token->loc, "Expected any [%.*s], but got `%s`", (i32)sb.len, sb.buf, get_token_kind_value(token->kind));

    sb_destroy(&sb);

    return token;
}