#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"

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

TokenStream ts_create(u32 init_size, const String* path, byte* data, u64 size, ReportCollector* rc);

void ts_destroy(TokenStream* ts);

bool ts_is_end(const TokenStream* ts);

const Token* ts_get_curr(TokenStream* ts);

const Token* ts_peek_next(TokenStream* ts);

const Token* ts_advance(TokenStream* ts);

void ts_move_forward(TokenStream* ts);

void ts_move_back(TokenStream* ts);