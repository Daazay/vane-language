#include "vane/scanner/token_kind.h"

const char* get_token_kind_name(TokenKind kind) {
    switch (kind) {
#define TOKEN(KIND, NAME, VALUE) case TOKEN_##KIND: return NAME;
#include "vane/scanner/token_kind.def"
    default:
        unreachable();
        return NULL;
    }
}

const char* get_token_kind_value(TokenKind kind) {
    switch (kind) {
#define TOKEN(KIND, NAME, VALUE) case TOKEN_##KIND: return VALUE;
#include "vane/scanner/token_kind.def"
    default:
        unreachable();
        return NULL;
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

bool is_token_kind_an_op(TokenKind kind) {
    switch (kind) {
#define TOKEN_OP(KIND, NAME, VALUE, PREC) case TOKEN_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default:
        return false;
    }
}

bool is_token_kind_a_binop(TokenKind kind) {
    switch (kind) {
#define TOKEN_BINOP(KIND, NAME, VALUE, PREC) case TOKEN_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default:
        return false;
    }
}

bool is_token_kind_an_unop(TokenKind kind) {
    return is_token_kind_a_prefix_unop(kind) || is_token_kind_a_postfix_unop(kind);
}

bool is_token_kind_a_prefix_unop(TokenKind kind) {
    switch (kind) {
    case TOKEN_PLUS:  return true;
    case TOKEN_MINUS: return true;
    case TOKEN_CARET: return true;
    case TOKEN_AMP:   return true;
#define TOKEN_UNOP(KIND, NAME, VALUE, PREC) case TOKEN_##KIND: return true;
#include "vane/scanner/token_kind.def"
    default:
        return false;
    }
}

bool is_token_kind_a_postfix_unop(TokenKind kind) {
    switch (kind) {
    case TOKEN_PLUS_PLUS:   return true;
    case TOKEN_MINUS_MINUS: return true;
    case TOKEN_CARET:       return true; // deref
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

OpPrecedence get_token_kind_precedence(TokenKind kind) {
    switch (kind) {
#define TOKEN_OP(KIND, NAME, VAUE, PREC) case TOKEN_##KIND: return OP_PREC_##PREC;
#include "vane/scanner/token_kind.def"
    default: return OP_PREC_NONE;
    }
}

OpAssociativity get_op_associativity(OpPrecedence prec) {
    switch (prec) {
    case OP_PREC_ASSIGNMENT: return OP_ASSOC_RIGHT;
    case OP_PREC_UNARY:      return OP_ASSOC_RIGHT;
    default:                 return OP_ASSOC_LEFT;
    }
}

bool is_token_kind_a_beginning_of_stmt(TokenKind kind) {
    return
        (kind == TOKEN_KEYWORD_BEGIN) ||
        (kind == TOKEN_KEYWORD_VAR) ||
        (kind == TOKEN_KEYWORD_IF) ||
        (kind == TOKEN_KEYWORD_WHILE) ||
        (kind == TOKEN_KEYWORD_DO) ||
        (kind == TOKEN_KEYWORD_BREAK) ||
        (kind == TOKEN_KEYWORD_CONTINUE) ||
        (kind == TOKEN_KEYWORD_RETURN) ||
        (kind == TOKEN_SEMICOLON) ||
        is_token_kind_a_beginning_of_expr(kind);
}

bool is_token_kind_a_beginning_of_expr(TokenKind kind) {
    return
        (kind == TOKEN_IDENTIFIER) ||
        (kind == TOKEN_L_BRACE) ||
        is_token_kind_a_literal(kind) ||
        is_token_kind_a_prefix_unop(kind);
}