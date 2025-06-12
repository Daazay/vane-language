#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"
#include "vane/utils/file_utils.h"

#include "vane/scanner/scanner.h"

#include "vane/diagnostic/report_collector.h"

#define TOKEN_STREAM_DEFAULT_SIZE 1000

typedef struct TokenStream TokenStream;

struct TokenStream {
    Scanner scanner;
    Vector tokens;
    i32 idx;
    bool done;
};

TokenStream token_stream_create(u32 init_tokens_size, const FileContent* fc, ReportCollector* rc);

void token_stream_destroy(TokenStream* ts);

bool token_stream_is_end(const TokenStream* ts);

void token_stream_move_forward(TokenStream* ts);

void token_stream_move_back(TokenStream* ts);

const Token* token_stream_get_curr(TokenStream* ts);

const Token* token_stream_peek_next(TokenStream* ts);

const Token* token_stream_advance(TokenStream* ts);

const Token* token_stream_expect(TokenStream* ts, TokenKind expected);

const Token* token_stream_expect_any_impl(TokenStream* ts, const TokenKind expected[], u32 expected_count);

#define token_stream_expect_any(TS, ...) \
token_stream_expect_any_impl(TS, \
((const TokenKind[]) { __VA_ARGS__ }), \
(sizeof(((const TokenKind[]) { __VA_ARGS__ })) / sizeof(TokenKind)))

const Token* token_stream_advance_if(TokenStream* ts, TokenKind expected);

const Token* token_stream_advance_if_any_impl(TokenStream* ts, const TokenKind expected[], u32 expected_count);

#define token_stream_advance_any(TS, ...) \
token_stream_advance_if_any_impl(TS, \
((const TokenKind[]) { __VA_ARGS__ }), \
(sizeof(((const TokenKind[]) { __VA_ARGS__ })) / sizeof(TokenKind)))
