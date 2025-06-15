#include "vane/scanner/token.h"

Token token_create(TokenKind kind, String value, SourceLoc loc) {
    return (Token) {
        .kind  = kind,
        .value = value,
        .loc   = loc,
        .first_in_line = false,
    };
}

void token_destroy(Token* token) {
    if (token == NULL) {
        return;
    }

    string_destroy(&token->value);
}