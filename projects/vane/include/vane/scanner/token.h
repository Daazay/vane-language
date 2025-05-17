#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/source_loc.h"

#include "vane/scanner/token_kind.h"

typedef struct Token Token;
struct Token {
    TokenKind kind;
    String value;
    SourceLoc loc;
};

Token token_create(TokenKind kind, String value, SourceLoc loc);

void token_destroy(Token* token);