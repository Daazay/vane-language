#include "vane/ast/ast_parser.h"

#pragma region UTILITIES

static inline const Token* ast_parser_expect_token(ASTParser* ast_parser, bool advance, TokenKind kind) {
    const Token* token = ts_peek_next(ast_parser->ts);
    if (token->kind == kind) {
        if (advance) {
            ts_move_forward(ast_parser->ts);
        }
        return ts_get_curr(ast_parser->ts);
    }

    return token;
}

#define EXPECT(EXPECTED) ast_parser_expect_token(ast_parser, false, EXPECTED)

#define ADVANCE(EXPECTED) ast_parser_expect_token(ast_parser, true, EXPECTED)

static inline const Token* ast_parser_expect_any_token(ASTParser* ast_parser, bool advance, u32 count, const TokenKind kinds[]) {
    const Token* token = ts_peek_next(ast_parser->ts);

    for (u32 i = 0; i < count; ++i) {
        if (token->kind == kinds[i]) {
            if (advance) {
                ts_move_forward(ast_parser->ts);
            }
            return ts_get_curr(ast_parser->ts);
        }
    }

    return token;
}

#define EXPECT_ANY(...) ast_parser_expect_any_token(ast_parser, false, ARR_SIZE(ARR(TokenKind, __VA_ARGS__)), ARR(TokenKind, __VA_ARGS__))

#define ADVANCE_ANY(...) ast_parser_expect_token(ast_parser, true, ARR_SIZE(ARR(TokenKind, __VA_ARGS__)), ARR(TokenKind, __VA_ARGS__))

#pragma endregion

ASTParser ast_parser_create(TokenStream* ts) {
    return (ASTParser) {
        .ts = ts,
    };
}

void ast_parser_destroy(ASTParser* ast_parser) {
    if (ast_parser == NULL) {
        return;
    }
}

ASTNode* ast_parser_parse_identifier(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_IDENTIFIER);
    if (token->kind != TOKEN_IDENTIFIER) {
        return NULL;
    }

    return ast_node_identifier_create(string_clone(&token->value), token->loc);
}

ASTNode* ast_parser_parse_type(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);

    switch (token->kind) {
    case TOKEN_CARET:       return ast_parser_parse_type_ptr(ast_parser);
    case TOKEN_L_BRACKET:   return ast_parser_parse_type_arr(ast_parser);
    default:
        if (is_token_kind_a_builtin_type(token->kind) || (token->kind == TOKEN_IDENTIFIER)) {
            return ast_parser_parse_type_basic(ast_parser);
        }
        return NULL;
    }
}

ASTNode* ast_parser_parse_type_basic(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);

    if (is_token_kind_a_builtin_type(token->kind)) {
        ts_move_forward(ast_parser->ts);
        token = ts_get_curr(ast_parser->ts);

        return ast_node_type_builtin_create(token->kind, token->loc);
    }
    else if (token->kind == TOKEN_IDENTIFIER) {
        ts_move_forward(ast_parser->ts);
        token = ts_get_curr(ast_parser->ts);

        return ast_node_type_custom_create(string_clone(&token->value), token->loc);
    }

    return NULL;
}

ASTNode* ast_parser_parse_type_ptr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_CARET);
    if (token->kind != TOKEN_CARET) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    ASTNode* type = ast_parser_parse_type(ast_parser);
    if (type == NULL) {
        return NULL;
    }

    loc.end = type->loc.end;

    return ast_node_type_ptr_create(type, loc);
}

ASTNode* ast_parser_parse_type_arr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_L_BRACKET);
    if (token->kind != TOKEN_L_BRACKET) {
        return NULL;
    }

    SourceLoc loc = token->loc;
    ASTNode* expr = NULL;

    token = ts_peek_next(ast_parser->ts);
    if (token->kind != TOKEN_R_BRACKET) {
        expr = ast_parser_parse_expr(ast_parser);
        if (expr == NULL) {
            return NULL;
        }
    }

    if ((token = ADVANCE(TOKEN_R_BRACKET))->kind != TOKEN_R_BRACKET) {
        ast_node_destroy(expr);
        return NULL;
    }

    ASTNode* type = ast_parser_parse_type(ast_parser);
    if (type == NULL) {
        ast_node_destroy(expr);
        return NULL;
    }

    loc.end = type->loc.end;

    return ast_node_type_arr_create(type, expr, loc);
}

ASTNode* ast_parser_parse_expr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    return ast_parser_parse_expr_with_prec(ast_parser, OP_PREC_NONE);
}

ASTNode* ast_parser_parse_expr_with_prec(ASTParser* ast_parser, OpPrecedence prec) {
    assert(ast_parser != NULL);

    ASTNode* node = ast_parser_parse_expr_nud(ast_parser);
    if (node == NULL) {
        return NULL;
    }

    while (!ts_is_end(ast_parser->ts)) {
        OpPrecedence new_prec = get_op_precedence(ts_peek_next(ast_parser->ts)->kind);
        if (new_prec <= prec) {
            break;
        }

        node = ast_parser_parse_expr_led(ast_parser, node, new_prec);
        if (node == NULL) {
            return NULL;
        }
    }
    return node;
}

ASTNode* ast_parser_parse_expr_nud(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);
    switch (token->kind) {
    case TOKEN_IDENTIFIER:  return ast_parser_parse_expr_place(ast_parser);
    case TOKEN_L_BRACE:     return ast_parser_parse_expr_braces(ast_parser);
    case TOKEN_L_CURLY:     return ast_parser_parse_expr_init_list(ast_parser);
    case TOKEN_KEYWORD_NIL: return ast_parser_parse_expr_nil(ast_parser);
    default:
        if (is_token_kind_a_literal(token->kind)) {
            return ast_parser_parse_expr_literal(ast_parser);
        }
        else if (is_token_kind_a_prefix_unop(token->kind)) {
            return ast_parser_parse_expr_prefix_unary(ast_parser);
        }
        else if (is_token_kind_a_builtin_type(token->kind)) {
            return ast_parser_parse_expr_cast(ast_parser);
        }

        return NULL;
    }
}

ASTNode* ast_parser_parse_expr_prefix_unary(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);
    if (!is_token_kind_a_prefix_unop(token->kind)) {
        return NULL;
    }

    SourceLoc loc = token->loc;
    TokenKind op = token->kind;

    ts_move_forward(ast_parser->ts);

    ASTNode* rhs = ast_parser_parse_expr(ast_parser);
    if (rhs == NULL) {
        return NULL;
    }

    loc.end = rhs->loc.end;

    return ast_node_expr_unary_create(op, NULL, rhs, loc);
}

ASTNode* ast_parser_parse_expr_place(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_IDENTIFIER);
    if (token->kind != TOKEN_IDENTIFIER) {
        return NULL;
    }

    return ast_node_expr_place_create(string_clone(&token->value), token->loc);
}

ASTNode* ast_parser_parse_expr_braces(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_L_BRACE);
    if (token->kind != TOKEN_L_BRACE) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    ASTNode* expr = ast_parser_parse_expr(ast_parser);
    if (expr == NULL) {
        return NULL;
    }

    if ((token = ADVANCE(TOKEN_R_BRACE))->kind != TOKEN_R_BRACE) {
        ast_node_destroy(expr);
        return NULL;
    }

    loc.end = token->loc.end;

    return ast_node_expr_braces_create(expr, loc);
}

ASTNode* ast_parser_parse_expr_init_list(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_L_CURLY);
    if (token->kind != TOKEN_L_CURLY) {
        return NULL;
    }

    SourceLoc loc = token->loc;

    bool is_good = true;
    Vector items = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    while (!ts_is_end(ast_parser->ts) && (ts_peek_next(ast_parser->ts)->kind != TOKEN_R_CURLY)) {
        ASTNode* item = ast_parser_parse_expr(ast_parser);
        if (item == NULL) {
            is_good = false;
            break;
        }

        vector_push_back(&items, &item);

        token = ts_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            ts_move_forward(ast_parser->ts);
            continue;
        }
        else if (token->kind != TOKEN_R_CURLY) {
            is_good = false;
        }
        break;
    }

    if (!is_good || ((token = ADVANCE(TOKEN_R_CURLY))->kind != TOKEN_R_CURLY)) {
        vector_destroy(&items);
        return NULL;
    }

    loc.end = token->loc.end;

    return ast_node_expr_init_list_create(items, loc);
}

ASTNode* ast_parser_parse_expr_nil(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_KEYWORD_NIL);
    if (token->kind != TOKEN_KEYWORD_NIL) {
        return NULL;
    }

    return ast_node_expr_nil_create(token->loc);
}

ASTNode* ast_parser_parse_expr_literal(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);
    if (!is_token_kind_a_literal(token->kind)) {
        return NULL;
    }

    ts_move_forward(ast_parser->ts);
    token = ts_get_curr(ast_parser->ts);

    return ast_node_expr_literal_create(token->kind, string_clone(&token->value), token->loc);
}

ASTNode* ast_parser_parse_expr_cast(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* type = ast_parser_parse_type(ast_parser);
    if (type == NULL) {
        return NULL;
    }

    SourceLoc loc = type->loc;

    const Token* token = ADVANCE(TOKEN_L_CURLY);
    if (token->kind != TOKEN_L_CURLY) {
        ast_node_destroy(type);
        return NULL;
    }

    ASTNode* expr = ast_parser_parse_expr(ast_parser);
    if (expr == NULL) {
        ast_node_destroy(type);
        return NULL;
    }

    token = ADVANCE(TOKEN_R_CURLY);
    if (token->kind != TOKEN_R_CURLY) {
        ast_node_destroy(type);
        ast_node_destroy(expr);
        return NULL;
    }

    loc.end = token->loc.end;

    return ast_node_expr_cast_create(type, expr, loc);
}

ASTNode* ast_parser_parse_expr_led(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec) {
    assert(ast_parser != NULL && lhs != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);
    switch (token->kind) {
    case TOKEN_L_BRACE:    return ast_parser_parse_expr_call(ast_parser, lhs);
    case TOKEN_L_BRACKET:  return ast_parser_parse_expr_index(ast_parser, lhs);
    case TOKEN_DOT:        return ast_parser_parse_expr_member(ast_parser, lhs);
    default:
        if (is_token_kind_an_infix_unop(token->kind)) {
            return ast_parser_parse_expr_infix_unary(ast_parser, lhs);
        }
        else if (is_token_kind_a_binop(token->kind)) {
            return ast_parser_parse_expr_binary(ast_parser, lhs, prec);
        }

        return NULL;
    }
}

ASTNode* ast_parser_parse_expr_binary(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec) {
    assert(ast_parser != NULL && lhs != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);
    if (!is_token_kind_a_binop(token->kind)) {
        ast_node_destroy(lhs);
        return NULL;
    }

    SourceLoc loc = lhs->loc;
    TokenKind op = token->kind;

    ts_move_forward(ast_parser->ts);

    ASTNode* rhs = (get_op_associativity(prec) == OP_ASSOC_LEFT)
        ? ast_parser_parse_expr_with_prec(ast_parser, prec)
        : ast_parser_parse_expr_with_prec(ast_parser, prec - 1);

    if (rhs == NULL) {
        ast_node_destroy(lhs);
        return NULL;
    }

    loc.end = rhs->loc.end;

    return ast_node_expr_binary_create(op, lhs, rhs, loc);
}

ASTNode* ast_parser_parse_expr_infix_unary(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);
    if (!is_token_kind_an_infix_unop(token->kind)) {
        ast_node_destroy(lhs);
        return NULL;
    }

    TokenKind op = token->kind;

    SourceLoc loc = lhs->loc;
    loc.end = token->loc.end;

    return ast_node_expr_unary_create(op, lhs, NULL, loc);
}

ASTNode* ast_parser_parse_expr_call(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    const Token* token = ADVANCE(TOKEN_L_BRACE);
    if (token->kind != TOKEN_L_BRACE) {
        ast_node_destroy(lhs);
        return NULL;
    }

    SourceLoc loc = lhs->loc;

    bool is_good = true;
    Vector items = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    token = ts_peek_next(ast_parser->ts);
    while (!ts_is_end(ast_parser->ts)) {
        if (items.size == 0 && token->kind == TOKEN_R_BRACE) {
            break;
        }

        ASTNode* item = ast_parser_parse_expr(ast_parser);
        if (item == NULL) {
            is_good = false;
            break;
        }

        vector_push_back(&items, &item);

        token = ts_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            ts_move_forward(ast_parser->ts);
            continue;
        }
        else if (token->kind != TOKEN_R_BRACE) {
            is_good = false;
        }
        break;
    }

    if (!is_good || ((token = ADVANCE(TOKEN_R_BRACE))->kind != TOKEN_R_BRACE)) {
        ast_node_destroy(lhs);
        vector_destroy(&items);
        return NULL;
    }

    loc.end = token->loc.end;

    return ast_node_expr_call_create(lhs, items, loc);
}

ASTNode* ast_parser_parse_expr_index(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    const Token* token = ADVANCE(TOKEN_L_BRACKET);
    if (token->kind != TOKEN_L_BRACKET) {
        ast_node_destroy(lhs);
        return NULL;
    }

    SourceLoc loc = lhs->loc;

    bool is_good = true;
    Vector items = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    token = ts_peek_next(ast_parser->ts);
    while (!ts_is_end(ast_parser->ts)) {
        if (items.size == 0 && token->kind == TOKEN_R_BRACKET) {
            break;
        }

        ASTNode* item = ast_parser_parse_expr(ast_parser);
        if (item == NULL) {
            is_good = false;
            break;
        }

        token = ts_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            ts_move_forward(ast_parser->ts);
            continue;
        }
        else if (token->kind != TOKEN_R_BRACKET) {
            is_good = false;
        }
        break;
    }

    if (!is_good || ((token = ADVANCE(TOKEN_R_BRACKET))->kind != TOKEN_R_BRACKET)) {
        ast_node_destroy(lhs);
        vector_destroy(&items);
        return NULL;
    }

    loc.end = token->loc.end;

    return ast_node_expr_call_create(lhs, items, loc);
}

ASTNode* ast_parser_parse_expr_member(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    const Token* token = ADVANCE(TOKEN_DOT);
    if (token->kind != TOKEN_DOT) {
        ast_node_destroy(lhs);
        return NULL;
    }

    SourceLoc loc = lhs->loc;

    ASTNode* member = ast_parser_parse_expr(ast_parser);
    if (member == NULL) {
        ast_node_destroy(lhs);
        return NULL;
    }

    loc.end = member->loc.end;

    return ast_node_expr_member_create(lhs, member, loc);
}