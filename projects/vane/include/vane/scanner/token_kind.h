#pragma once

#include "vane/utils/defines.h"

typedef enum TokenKind TokenKind;
enum TokenKind {
#define TOKEN(KIND, NAME, VALUE) TOKEN_##KIND,
#include "vane/scanner/token_kind.def"
};

const char* get_token_kind_name(TokenKind kind);

const char* get_token_kind_value(TokenKind kind);

bool is_token_kind_a_misc(TokenKind kind);

bool is_token_kind_a_punc(TokenKind kind);

bool is_token_kind_a_keyword(TokenKind kind);

bool is_token_kind_a_builtin(TokenKind kind);

bool is_token_kind_a_builtin_type(TokenKind kind);

bool is_token_kind_a_literal(TokenKind kind);