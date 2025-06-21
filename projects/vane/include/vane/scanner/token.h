#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

#include "vane/diagnostic/source_loc.h"

#include "vane/scanner/token_kind.h"

typedef struct Token Token;

struct Token {
    TokenKind kind;
    String value;
    SourceLoc loc;
    bool first_in_line;
};

Token token_create(TokenKind kind, String value, SourceLoc loc, bool first_in_line);

void token_destroy(Token* token);