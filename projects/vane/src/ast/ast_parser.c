#include "vane/ast/ast_parser.h"

#include "vane/utils/string_builder.h"

#pragma region DIAGNOSTIC

#define TRACE(LOC, FORMAT, ...) do { \
    if (ast_parser->rc != NULL) { \
        String msg = string_from_fmt(FORMAT, ##__VA_ARGS__); \
        report_collector_append_report_trace(ast_parser->rc, msg, LOC); \
    } \
} while (false)

#define REPORT(SEVERITY, LOC, FORMAT, ...) do { \
    if (ast_parser->rc != NULL) { \
        String msg = string_from_fmt(FORMAT, ##__VA_ARGS__); \
        report_collector_append_report(ast_parser->rc, SEVERITY, msg, LOC); \
    } \
} while (false)

#define TRACE_PARSE_FAILED(LOC, STR) TRACE(LOC, "failed to parse `%s`", STR)
#define TRACE_PARSE_FAILED_AST(LOC, AST) TRACE_PARSE_FAILED(LOC, get_ast_node_kind_name(AST))

#define REPORT_INFO(LOC, FORMAT, ...)  REPORT(DIAG_SEVERITY_INFO, LOC, FORMAT, ##__VA_ARGS__)
#define REPORT_WARN(LOC, FORMAT, ...)  REPORT(DIAG_SEVERITY_WARN, LOC, FORMAT, ##__VA_ARGS__)
#define REPORT_ERROR(LOC, FORMAT, ...) REPORT(DIAG_SEVERITY_ERROR, LOC, FORMAT, ##__VA_ARGS__)

#pragma endregion

#pragma region UTILITIES

static inline const Token* ast_parser_expect_token(ASTParser* ast_parser, bool advance, TokenKind kind) {
    const Token* token = ts_peek_next(ast_parser->ts);
    if (token->kind == kind) {
        if (advance) {
            ts_move_forward(ast_parser->ts);
        }
        // why ts_get_curr instead of token?
        // ts_move_forward can cause vector reallcation which would lead to reference to invalid memory.
        return ts_get_curr(ast_parser->ts);
    }

    TRACE(token->loc, "Expected `%s`, but got `%s`",
        get_token_kind_value(kind),
        get_token_kind_value(token->kind)
    );

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

    StringBuilder sb = sb_create(16);

    for (u32 i = 0; i < count; ++i) {
        sb_append_format(&sb, "`%s`", get_token_kind_value(kinds[i]));
        if (i + 1 < count) {
            sb_append_cstr(&sb, ", ");
        }
    }

    TRACE(
        token->loc,
        "expected any [%.*s], but got `%s`",
        (i32)sb.len, sb.buf,
        get_token_kind_value(token->kind)
    );

    sb_destroy(&sb);

    return token;
}

#define EXPECT_ANY(...) ast_parser_expect_any_token(ast_parser, false, ARR_SIZE(ARR(TokenKind, __VA_ARGS__)), ARR(TokenKind, __VA_ARGS__))

#define ADVANCE_ANY(...) ast_parser_expect_token(ast_parser, true, ARR_SIZE(ARR(TokenKind, __VA_ARGS__)), ARR(TokenKind, __VA_ARGS__))

#pragma endregion

ASTParser ast_parser_create(TokenStream* ts, ReportCollector* rc) {
    return (ASTParser) {
        .ts = ts,
        .rc = rc,
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
        TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_IDENTIFIER);
        return ast_node_error_create(NULL, token->loc);
    }

    return ast_node_identifier_create(string_clone(&token->value), token->loc);
}

static ASTNode* ast_parser_parse_typeref_inner(ASTParser* ast_parser) {
    const Token* token = ts_peek_next(ast_parser->ts);

    switch (token->kind) {
    case TOKEN_CARET:       return ast_parser_parse_typeref_ptr(ast_parser);
    case TOKEN_L_BRACKET:   return ast_parser_parse_typeref_arr(ast_parser);
    default:
        if (is_token_kind_a_builtin_type(token->kind) || (token->kind == TOKEN_IDENTIFIER)) {
            return ast_parser_parse_typeref_basic(ast_parser);
        }
    }

    TRACE(token->loc, "Expected beginning of `typeref`, but got `%s`", get_token_kind_value(token->kind));
    return ast_node_error_create(NULL, token->loc);
}

ASTNode* ast_parser_parse_typeref(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* typeref = ast_parser_parse_typeref_inner(ast_parser);
    if (typeref->kind == AST_NODE_ERROR) {
        TRACE_PARSE_FAILED(typeref->loc, "typeref");
    }

    return typeref;
}

ASTNode* ast_parser_parse_typeref_basic(ASTParser* ast_parser) {
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

    TRACE_PARSE_FAILED(token->loc, "typeref basic");
    return ast_node_error_create(NULL, token->loc);
}

ASTNode* ast_parser_parse_typeref_ptr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_CARET);
    if (token->kind != TOKEN_CARET) {
        TRACE_PARSE_FAILED(token->loc, "typeref ptr");
        return ast_node_error_create(NULL, token->loc);
    }

    SourceLoc loc = token->loc;
    ASTNode* type = ast_parser_parse_typeref(ast_parser);

    loc.end = type->loc.end;

    if (type->kind == AST_NODE_ERROR) {
        TRACE_PARSE_FAILED(loc, "typeref ptr");
        return ast_node_error_create(type, loc);
    }

    return ast_node_type_ptr_create(type, loc);
}

ASTNode* ast_parser_parse_typeref_arr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_L_BRACKET);
    if (token->kind != TOKEN_L_BRACKET) {
        TRACE_PARSE_FAILED(token->loc, "typeref arr");
        return ast_node_error_create(NULL, token->loc);
    }

    SourceLoc loc = token->loc;
    ASTNode* expr = NULL;

    token = ts_peek_next(ast_parser->ts);
    if (token->kind != TOKEN_R_BRACKET) {
        expr = ast_parser_parse_expr(ast_parser);
        loc.end = expr->loc.end;

        if (expr->kind == AST_NODE_ERROR) {
            TRACE_PARSE_FAILED(loc, "typeref arr");
            return ast_node_error_create(expr, loc);
        }
    }

    token = ADVANCE(TOKEN_R_BRACKET);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_BRACKET) {
        ast_node_destroy(expr);
        TRACE_PARSE_FAILED(loc, "typeref arr");
        return ast_node_error_create(NULL, loc);
    }

    ASTNode* type = ast_parser_parse_typeref(ast_parser);
    loc.end = type->loc.end;

    if (type->kind == AST_NODE_ERROR) {
        ast_node_destroy(expr);
        TRACE_PARSE_FAILED(loc, "typeref arr");
        return ast_node_error_create(type, loc);
    }

    return ast_node_type_arr_create(type, expr, loc);
}

ASTNode* ast_parser_parse_expr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* expr = ast_parser_parse_expr_with_prec(ast_parser, OP_PREC_NONE);
    if (expr->kind == AST_NODE_ERROR) {
        TRACE_PARSE_FAILED(expr->loc, "expr");
    }
    return expr;
}

ASTNode* ast_parser_parse_expr_with_prec(ASTParser* ast_parser, OpPrecedence prec) {
    assert(ast_parser != NULL);

    ASTNode* node = ast_parser_parse_expr_nud(ast_parser);
    if (node->kind == AST_NODE_ERROR) {
        return node;
    }

    SourceLoc loc = node->loc;

    while (!ts_is_end(ast_parser->ts)) {
        OpPrecedence new_prec = get_op_precedence(ts_peek_next(ast_parser->ts)->kind);
        if (new_prec <= prec) {
            break;
        }

        node = ast_parser_parse_expr_led(ast_parser, node, new_prec);
        loc.end = node->loc.end;

        if (node->kind == AST_NODE_ERROR) {
            return ast_node_error_create(node, loc);
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
    }

    TRACE(token->loc, "Expected beginning of `nud expr`, but got `%s`", get_token_kind_value(token->kind));
    return ast_node_error_create(NULL, token->loc);
}

ASTNode* ast_parser_parse_expr_prefix_unary(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);
    if (!is_token_kind_a_prefix_unop(token->kind)) {
        TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_EXPR_PREFIX_UNARY);
        return ast_node_error_create(NULL, token->loc);
    }

    SourceLoc loc = token->loc;
    TokenKind op = token->kind;

    ts_move_forward(ast_parser->ts);

    ASTNode* rhs = ast_parser_parse_expr(ast_parser);
    loc.end = rhs->loc.end;

    if (rhs->kind == AST_NODE_ERROR) {
        TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_EXPR_PREFIX_UNARY);
        return ast_node_error_create(rhs, loc);
    }

    return ast_node_expr_prefix_unary_create(op, rhs, loc);
}

ASTNode* ast_parser_parse_expr_place(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_IDENTIFIER);
    if (token->kind != TOKEN_IDENTIFIER) {
        TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_EXPR_PLACE);
        return ast_node_error_create(NULL, token->loc);
    }

    return ast_node_expr_place_create(string_clone(&token->value), token->loc);
}

ASTNode* ast_parser_parse_expr_braces(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_L_BRACE);
    if (token->kind != TOKEN_L_BRACE) {
        TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_EXPR_BRACES);
        return ast_node_error_create(NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* expr = ast_parser_parse_expr(ast_parser);
    loc.end = expr->loc.end;

    if (expr->kind == AST_NODE_ERROR) {
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_BRACES);
        return ast_node_error_create(expr, loc);
    }

    token = ADVANCE(TOKEN_R_BRACE);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_BRACE) {
        ast_node_destroy(expr);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_BRACES);
        return ast_node_error_create(NULL, loc);
    }


    return ast_node_expr_braces_create(expr, loc);
}

ASTNode* ast_parser_parse_expr_init_list(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_L_CURLY);
    if (token->kind != TOKEN_L_CURLY) {
        TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_EXPR_INIT_LIST);
        return ast_node_error_create(NULL, token->loc);
    }

    SourceLoc loc = token->loc;
    Vector items = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    while (!ts_is_end(ast_parser->ts) && (ts_peek_next(ast_parser->ts)->kind != TOKEN_R_CURLY)) {
        ASTNode* item = ast_parser_parse_expr(ast_parser);
        loc.end = item->loc.end;

        if (item ->kind == AST_NODE_ERROR) {
            vector_destroy(&items);
            TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_EXPR_INIT_LIST);
            return ast_node_error_create(item, token->loc);
        }

        vector_push_back(&items, &item);

        token = ts_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            ts_move_forward(ast_parser->ts);
            continue;
        }
        break;
    }

    token = ADVANCE(TOKEN_R_CURLY);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_CURLY) {
        vector_destroy(&items);
        TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_EXPR_INIT_LIST);
        return ast_node_error_create(NULL, loc);
    }

    return ast_node_expr_init_list_create(items, loc);
}

ASTNode* ast_parser_parse_expr_nil(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ADVANCE(TOKEN_KEYWORD_NIL);
    if (token->kind != TOKEN_KEYWORD_NIL) {
        TRACE_PARSE_FAILED_AST(token->loc, AST_NODE_EXPR_NIL);
        return ast_node_error_create(NULL, token->loc);
    }

    return ast_node_expr_nil_create(token->loc);
}

ASTNode* ast_parser_parse_expr_literal(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = ts_peek_next(ast_parser->ts);
    if (!is_token_kind_a_literal(token->kind)) {
        TRACE(token->loc, "Expected literal, but got `%s`", get_token_kind_value(token->kind));
        TRACE_PARSE_FAILED(token->loc, "failed to parse `literal`");
        return ast_node_error_create(NULL, token->loc);
    }

    // Why so ugly?
    // If we do ts_move_forward and try inline creation and return,
    // we will catch the moment when the array of tokens is reallocated, which means
    // that the reference to the token will be invalid.
    ASTNode* node = ast_node_expr_literal_create(token->kind, string_clone(&token->value), token->loc);
    ts_move_forward(ast_parser->ts);
    return node;
}

ASTNode* ast_parser_parse_expr_cast(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* type = ast_parser_parse_typeref(ast_parser);
    SourceLoc loc = type->loc;

    if (type->kind == AST_NODE_ERROR) {
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_CAST);
        return ast_node_error_create(NULL, loc);
    }

    const Token* token = ADVANCE(TOKEN_L_CURLY);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_L_CURLY) {
        ast_node_destroy(type);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_CAST);
        return ast_node_error_create(NULL, loc);
    }

    ASTNode* expr = ast_parser_parse_expr(ast_parser);
    loc.end = expr->loc.end;

    if (expr->kind == AST_NODE_ERROR) {
        ast_node_destroy(type);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_CAST);
        return ast_node_error_create(expr, loc);
    }

    token = ADVANCE(TOKEN_R_CURLY);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_CURLY) {
        ast_node_destroy(type);
        ast_node_destroy(expr);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_CAST);
        return ast_node_error_create(NULL, loc);
    }

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
        if (is_token_kind_an_postfix_unop(token->kind)) {
            return ast_parser_parse_expr_postfix_unary(ast_parser, lhs);
        }
        else if (is_token_kind_a_binop(token->kind)) {
            return ast_parser_parse_expr_binary(ast_parser, lhs, prec);
        }
    }

    TRACE(token->loc, "Expected beginning of `led expr`, but got `%s`", get_token_kind_value(token->kind));
    return ast_node_error_create(NULL, token->loc);
}

ASTNode* ast_parser_parse_expr_binary(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec) {
    assert(ast_parser != NULL && lhs != NULL);

    SourceLoc loc = lhs->loc;

    const Token* token = ts_peek_next(ast_parser->ts);
    loc.end = token->loc.end;

    if (!is_token_kind_a_binop(token->kind)) {
        ast_node_destroy(lhs);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_BINARY);
        return ast_node_error_create(NULL, loc);
    }

    TokenKind op = token->kind;

    ts_move_forward(ast_parser->ts);

    ASTNode* rhs = (get_op_associativity(prec) == OP_ASSOC_LEFT)
        ? ast_parser_parse_expr_with_prec(ast_parser, prec)
        : ast_parser_parse_expr_with_prec(ast_parser, prec - 1);

    loc.end = rhs->loc.end;

    if (rhs->kind == AST_NODE_ERROR) {
        ast_node_destroy(lhs);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_BINARY);
        return ast_node_error_create(rhs, loc);
    }

    return ast_node_expr_binary_create(op, lhs, rhs, loc);
}

ASTNode* ast_parser_parse_expr_postfix_unary(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    SourceLoc loc = lhs->loc;

    const Token* token = ts_peek_next(ast_parser->ts);
    loc.end = token->loc.end;

    if (!is_token_kind_an_postfix_unop(token->kind)) {
        ast_node_destroy(lhs);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_POSTFIX_UNARY);
        return ast_node_error_create(NULL, loc);
    }

    TokenKind op = token->kind;
    ts_move_forward(ast_parser->ts);

    return ast_node_expr_postfix_unary_create(op, lhs, loc);
}

ASTNode* ast_parser_parse_expr_call(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    SourceLoc loc = lhs->loc;

    const Token* token = ADVANCE(TOKEN_L_BRACE);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_L_BRACE) {
        ast_node_destroy(lhs);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_CAST);
        return ast_node_error_create(NULL, loc);
    }

    Vector items = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    token = ts_peek_next(ast_parser->ts);
    while (!ts_is_end(ast_parser->ts)) {
        if (items.size == 0 && token->kind == TOKEN_R_BRACE) {
            break;
        }

        ASTNode* item = ast_parser_parse_expr(ast_parser);
        loc.end = item->loc.end;

        if (item->kind == AST_NODE_ERROR) {
            ast_node_destroy(lhs);
            vector_destroy(&items);
            TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_CAST);
            return ast_node_error_create(item, loc);
        }

        vector_push_back(&items, &item);

        token = ts_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            ts_move_forward(ast_parser->ts);
            continue;
        }
        break;
    }

    token = ADVANCE(TOKEN_R_BRACE);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_BRACE) {
        ast_node_destroy(lhs);
        vector_destroy(&items);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_CALL);
        return ast_node_error_create(NULL, loc);
    }

    return ast_node_expr_call_create(lhs, items, loc);
}

ASTNode* ast_parser_parse_expr_index(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    SourceLoc loc = lhs->loc;

    const Token* token = ADVANCE(TOKEN_L_BRACKET);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_L_BRACKET) {
        ast_node_destroy(lhs);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_INDEX);
        return ast_node_error_create(NULL, loc);
    }

    Vector items = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    token = ts_peek_next(ast_parser->ts);
    while (!ts_is_end(ast_parser->ts)) {
        if (items.size == 0 && token->kind == TOKEN_R_BRACKET) {
            break;
        }

        ASTNode* item = ast_parser_parse_expr(ast_parser);
        loc.end = item->loc.end;

        if (item->kind == AST_NODE_ERROR) {
            ast_node_destroy(lhs);
            vector_destroy(&items);
            TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_INDEX);
            return ast_node_error_create(item, loc);
        }

        token = ts_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            ts_move_forward(ast_parser->ts);
            continue;
        }
        break;
    }

    token = ADVANCE(TOKEN_R_BRACKET);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_BRACKET) {
        ast_node_destroy(lhs);
        vector_destroy(&items);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_INDEX);
        return ast_node_error_create(NULL, loc);
    }

    return ast_node_expr_call_create(lhs, items, loc);
}

ASTNode* ast_parser_parse_expr_member(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    SourceLoc loc = lhs->loc;

    const Token* token = ADVANCE(TOKEN_DOT);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_DOT) {
        ast_node_destroy(lhs);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_MEMBER);
        return ast_node_error_create(NULL, loc);
    }

    ASTNode* member = ast_parser_parse_expr(ast_parser);
    loc.end = member->loc.end;

    if (member->kind == AST_NODE_ERROR) {
        ast_node_destroy(lhs);
        TRACE_PARSE_FAILED_AST(loc, AST_NODE_EXPR_MEMBER);
        return ast_node_error_create(member, loc);
    }

    return ast_node_expr_member_create(lhs, member, loc);
}
