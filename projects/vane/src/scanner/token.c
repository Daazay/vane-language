#include "vane/scanner/token.h"

Token token_create(TokenKind kind, String value, SourceLoc loc) {
    return (Token) {
        .kind = kind,
        .value = value,
        .loc = loc,
    };
}

void token_destroy(Token* token) {
    if (token == NULL) {
        return;
    }

    string_destroy(&token->value);
}