#include "vane/scanner/token_stream.h"

#pragma region UTILITIES

static inline  void ts_parse_next(TokenStream* ts) {
    if (!ts->done) {
        Token token = scanner_scan_next(&ts->scanner);
        vector_push_back(&ts->tokens, &token);

        if (token.kind == TOKEN_EOF_) {
            ts->done = true;
        }
    }
}

#pragma endregion


TokenStream ts_create(u32 init_size, const String* path, byte* data, u64 size, ReportCollector* rc) {
    u32 cap = (init_size > 0)
        ? init_size
        : TOKEN_STREAM_DEFAULT_SIZE;

    return (TokenStream) {
        .scanner = scanner_create(path, data, size, rc),
        .tokens = vector_create(cap, VECTOR_ITEM_SPECS(Token, &token_destroy)),
        .idx = -1,
        .done = false,
    };
}

void ts_destroy(TokenStream* ts) {
    if (ts == NULL) {
        return;
    }

    vector_destroy(&ts->tokens);
    scanner_destroy(&ts->scanner);
}

bool ts_is_end(const TokenStream* ts) {
    assert(ts != NULL);

    if (ts->done && (ts->idx + 1 == (i32)ts->tokens.size)) {
        return true;
    }
    return false;
}

const Token* ts_get_curr(TokenStream* ts) {
    assert(ts != NULL);

    if (!ts->done && (ts->idx + 1 == (i32)ts->tokens.size)) {
        ts_parse_next(ts);
    }

    if (ts->idx < 0) {
        return NULL;
    }
    return vector_at(&ts->tokens, ts->idx);
}

const Token* ts_peek_next(TokenStream* ts) {
    assert(ts != NULL);

    if (ts->idx + 1 == (i32)ts->tokens.size) {
        if (ts->done) {
            return NULL;
        }
        ts_parse_next(ts);
    }
    return vector_at(&ts->tokens, ts->idx + 1);
}

const Token* ts_advance(TokenStream* ts) {
    assert(ts != NULL);

    const Token* token = ts_peek_next(ts);
    ts_move_forward(ts);

    return token;
}

void ts_move_forward(TokenStream* ts) {
    assert(ts != NULL);

    if (ts->idx + 1 == (i32)ts->tokens.size) {
        if (ts->done) {
            return;
        }
        ts_parse_next(ts);
    }
    ts->idx++;
}

void ts_move_back(TokenStream* ts) {
    assert(ts != NULL);

    if (ts->idx == -1) {
        return;
    }
    ts->idx--;
}