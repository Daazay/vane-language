#include "vane/scanner/token.h"

Token token_create(TokenKind kind, String value, SourceLoc loc, bool first_in_line) {
    return (Token) {
        .kind = kind,
        .value = value,
        .loc = loc,
        .first_in_line = first_in_line,
    };
}

void token_destroy(Token* token) {
    if (token == NULL) {
        return;
    }

    string_destroy(&token->value);
}