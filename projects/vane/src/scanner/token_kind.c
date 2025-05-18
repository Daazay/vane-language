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

bool is_token_kind_an_op(TokenKind kind) {
    switch (kind) {
#define TOKEN_OP(ID, NAME, VALUE, PREC) case TOKEN_##ID: return true;
#include "vane/scanner/token_kind.def"
    default:
        return false;
    }
}

bool is_token_kind_a_binop(TokenKind kind) {
    switch (kind) {
#define TOKEN_BINOP(ID, NAME, VALUE, PREC) case TOKEN_##ID: return true;
#include "vane/scanner/token_kind.def"
    default:
        return false;
    }
}

bool is_token_kind_a_prefix_unop(TokenKind kind) {
    switch (kind) {
    case TOKEN_PLUS:  return true;
    case TOKEN_MINUS: return true;
    case TOKEN_CARET: return true;
    case TOKEN_AMP:   return true;
#define TOKEN_UNOP(ID, NAME, VALUE, PREC) case TOKEN_##ID: return true;
#include "vane/scanner/token_kind.def"
    default:
        return false;
    }
}

bool is_token_kind_an_infix_unop(TokenKind kind) {
    switch (kind) {
    case TOKEN_PLUS:  return true;
    case TOKEN_MINUS: return true;
    case TOKEN_CARET: return true;
    case TOKEN_AMP:   return true;
#define TOKEN_UNOP(ID, NAME, VALUE, PREC) case TOKEN_##ID: return true;
#include "vane/scanner/token_kind.def"
    default:
        return false;
    }
}

bool is_token_kind_a_keyword(TokenKind kind) {
    switch (kind) {
    case TOKEN_PLUS_PLUS:   return true;
    case TOKEN_MINUS_MINUS: return true;
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

OpPrecedence get_op_precedence(TokenKind kind) {
    switch (kind) {
#define TOKEN_OP(ID, NAME, VAUE, PREC) case TOKEN_##ID: return OP_PREC_##PREC;
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