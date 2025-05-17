#include "vane/scanner/token_kind.h"

const char* get_token_kind_name(TokenKind kind) {
    switch (kind) {
#define TOKEN(KIND, NAME, VALUE) case TOKEN_##KIND: return NAME;
#include "vane/scanner/token_kind.def"
    default: return NULL;
    }
}

const char* get_token_kind_value(TokenKind kind) {
    switch (kind) {
#define TOKEN_PUNC(KIND, NAME, VALUE) case TOKEN_##KIND: return VALUE;
#include "vane/scanner/token_kind.def"
    default: return get_token_kind_name(kind);
    }
}

bool is_token_kind_a_misc(TokenKind kind) {
    switch (kind) {
#define TOKEN_MISC(KIND, NAME) case TOKEN_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default: return false;
    }
}

bool is_token_kind_a_punc(TokenKind kind) {
    switch (kind) {
#define TOKEN_PUNC(KIND, NAME, VALUE) case TOKEN_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default: return false;
    }
}

bool is_token_kind_a_keyword(TokenKind kind) {
    switch (kind) {
#define TOKEN_KEYWORD(KIND, NAME) case TOKEN_KEYWORD_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default: return false;
    }
}

bool is_token_kind_a_builtin(TokenKind kind) {
    switch (kind) {
#define TOKEN_BUILTIN(KIND, NAME) case TOKEN_KEYWORD_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default: return false;
    }
}

bool is_token_kind_a_builtin_type(TokenKind kind) {
    switch (kind) {
#define TOKEN_BUILTIN_TYPE(KIND, NAME) case TOKEN_KEYWORD_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default: return false;
    }
}

bool is_token_kind_a_literal(TokenKind kind) {
    switch (kind) {
#define TOKEN_LITERAL(KIND, NAME) case TOKEN_LITERAL_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default: return false;
    }
}