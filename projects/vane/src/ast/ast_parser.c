#include "vane/ast/ast_parser.h"

#pragma region DIAGNOSTIC

#define REPORT(SEVERITY_LOC, FORMAT, ...) do { \
    String _msg = string_from_fmt(FORMAT, ##__VA_ARGS__); \
    report_collector_append_report(ast_parser->rc, SEVERITY, _msg, LOC); \
} while (false)

#define REPORT_INFO(LOC, FORMAT, ...)  REPORT(DIAG_SEVERITY_INFO, LOC, FORMAT, ##__VA_ARGS__)
#define REPORT_WARN(LOC, FORMAT, ...)  REPORT(DIAG_SEVERITY_WARN, LOC, FORMAT, ##__VA_ARGS__)
#define REPORT_ERROR(LOC, FORMAT, ...) REPORT(DIAG_SEVERITY_ERROR, LOC, FORMAT, ##__VA_ARGS__)

#define TRACE(LOC, FORMAT, ...) do { \
    String _msg = string_from_fmt(FORMAT, ##__VA_ARGS__); \
    report_collector_append_report_trace(ast_parser->rc, _msg, LOC); \
} while (false)

#define TRACE_FAILED_TO_PARSE_STR(LOC, STR) TRACE(LOC, "Failed to parse `%s`", STR)
#define TRACE_FAILED_TO_PARSE_AST(LOC, AST) TRACE_FAILED_TO_PARSE_STR(LOC, get_ast_node_kind_name(AST))

#pragma endregion

#pragma region MyRegion

ASTNode* ast_node_error_create(ASTNodeKind failed, ASTNode * prev, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_ERROR, loc);
    node->as.error.failed = failed;
    node->as.error.prev = prev;
    return node;
}

ASTNode* ast_node_expr_binary_create(TokenKind op, ASTNode * lhs, ASTNode * rhs, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_BINARY, loc);
    node->as.expr_binary.op = op;
    node->as.expr_binary.lhs = lhs;
    node->as.expr_binary.rhs = rhs;
    return node;
}

ASTNode* ast_node_expr_prefix_unary_create(TokenKind op, ASTNode * rhs, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_PREFIX_UNARY, loc);
    node->as.expr_prefix_unary.op = op;
    node->as.expr_prefix_unary.rhs = rhs;
    return node;
}

ASTNode* ast_node_expr_postfix_unary_create(TokenKind op, ASTNode * lhs, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_POSTFIX_UNARY, loc);
    node->as.expr_postfix_unary.op = op;
    node->as.expr_postfix_unary.lhs = lhs;
    return node;
}

ASTNode* ast_node_expr_braces_create(ASTNode * expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_BRACES, loc);
    node->as.expr_braces.expr = expr;
    return node;
}

ASTNode* ast_node_expr_place_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_PLACE, loc);
    node->as.expr_place.value = value;
    return node;
}

ASTNode* ast_node_expr_call_create(ASTNode * callee, Vector args, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_CALL, loc);
    node->as.expr_call.callee = callee;
    node->as.expr_call.args = args;
    return node;
}

ASTNode* ast_node_expr_index_create(ASTNode * callee, Vector args, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_INDEX, loc);
    node->as.expr_index.callee = callee;
    node->as.expr_index.args = args;
    return node;
}

ASTNode* ast_node_expr_member_create(ASTNode * object, ASTNode * member, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_MEMBER, loc);
    node->as.expr_member.object = object;
    node->as.expr_member.member = member;
    return node;
}

ASTNode* ast_node_expr_literal_create(TokenKind token_kind, String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_LITERAL, loc);
    node->as.expr_literal.kind = token_kind;
    node->as.expr_literal.value = value;
    return node;
}

#pragma endregion

ASTParser ast_parser_create(TokenStream* ts, ReportCollector* rc) {
    assert(ts != NULL && rc != NULL);

    return (ASTParser) {
        .ts = ts,
        .rc = rc,
    };
}

void ast_parser_destroy(ASTParser* ast_parser) {
    if (ast_parser == NULL) {
        return;
    }
    /* DO NOTHING */
}

ASTNode* ast_parser_parse_expr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* node = ast_parser_parse_expr_with_prec(ast_parser, OP_PREC_NONE);
    if (node->kind == AST_NODE_ERROR) {
        TRACE_FAILED_TO_PARSE_AST(node->loc, AST_NODE_GROUP_EXPR);
    }
    return node;
}

ASTNode* ast_parser_parse_expr_with_prec(ASTParser* ast_parser, OpPrecedence prec) {
    assert(ast_parser != NULL);

    ASTNode* node = ast_parser_parse_expr_nud(ast_parser);
    if (node->kind == AST_NODE_ERROR) {
        return node;
    }

    while (!token_stream_is_end(ast_parser->ts)) {
        OpPrecedence new_prec = get_token_kind_precedence(token_stream_peek_next(ast_parser->ts)->kind);
        if (new_prec <= prec) {
            break;
        }

        node = ast_parser_parse_expr_led(ast_parser, node, new_prec);
        if (node->kind == AST_NODE_ERROR) {
            return node;
        }
    }

    return node;
}

ASTNode* ast_parser_parse_expr_nud(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_peek_next(ast_parser->ts);
    switch (token->kind) {
    case TOKEN_IDENTIFIER: return ast_parser_parse_expr_place(ast_parser);
    case TOKEN_L_BRACE:    return ast_parser_parse_expr_braces(ast_parser);
    default:
        if (is_token_kind_a_literal(token->kind)) {
            return ast_parser_parse_expr_literal(ast_parser);
        } else if (is_token_kind_a_prefix_unop(token->kind)) {
            return ast_parser_parse_expr_prefix_unary(ast_parser);
        }
        break;
    }

    TRACE(token->loc, "Expected beginning of `%s`, but got `%s`",
        get_ast_node_kind_name(AST_NODE_GROUP_EXPR_NUD),
        get_token_kind_value(token->kind)
    );
    TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_GROUP_EXPR_NUD);

    return ast_node_error_create(AST_NODE_GROUP_EXPR_NUD, NULL, token->loc);
}

ASTNode* ast_parser_parse_expr_led(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_peek_next(ast_parser->ts);
    switch (token->kind) {
    case TOKEN_L_BRACE:   return ast_parser_parse_expr_call(ast_parser, lhs);
    case TOKEN_L_BRACKET: return ast_parser_parse_expr_index(ast_parser, lhs);
    case TOKEN_DOT:       return ast_parser_parse_expr_member(ast_parser, lhs);
    default:
        if (is_token_kind_a_postfix_unop(token->kind)) {
            return ast_parser_parse_expr_postfix_unary(ast_parser, lhs);
        }
        else if (is_token_kind_a_binop(token->kind)) {
            return ast_parser_parse_expr_binary(ast_parser, lhs, prec);
        }
        break;
    }

    TRACE(token->loc, "Expected beginning of `%s`, but got `%s`",
        get_ast_node_kind_name(AST_NODE_GROUP_EXPR_LED),
        get_token_kind_value(token->kind)
    );
    TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_GROUP_EXPR_LED);

    return ast_node_error_create(AST_NODE_GROUP_EXPR_LED, NULL, token->loc);
}

ASTNode* ast_parser_parse_expr_binary(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec) {
    assert(ast_parser != NULL && lhs != NULL);

    SourceLoc loc = lhs->loc;

    const Token* token = token_stream_peek_next(ast_parser->ts);
    loc.end = token->loc.end;

    if (!is_token_kind_a_binop(token->kind)) {
        ast_node_destroy(lhs);
        TRACE(token->loc, "Expected binary operator, but got `%s`",
            get_token_kind_value(token->kind)
        );
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_BINARY);
        return ast_node_error_create(AST_NODE_EXPR_BINARY, NULL, loc);
    }

    TokenKind op = token->kind;

    token_stream_move_forward(ast_parser->ts);

    ASTNode* rhs = (get_op_associativity(prec) == OP_ASSOC_LEFT)
        ? ast_parser_parse_expr_with_prec(ast_parser, prec)
        : ast_parser_parse_expr_with_prec(ast_parser, prec - 1);

    loc.end = rhs->loc.end;

    if (rhs->kind == AST_NODE_ERROR) {
        ast_node_destroy(lhs);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_BINARY);
        return ast_node_error_create(AST_NODE_EXPR_BINARY, rhs, loc);
    }

    return ast_node_expr_binary_create(op, lhs, rhs, loc);
}

ASTNode* ast_parser_parse_expr_prefix_unary(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_peek_next(ast_parser->ts);
    if (!is_token_kind_a_prefix_unop(token->kind)) {
        TRACE(token->loc, "Expected prefix unary operator, but got `%s`",
            get_token_kind_value(token->kind)
        );
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_EXPR_PREFIX_UNARY);
        return ast_node_error_create(AST_NODE_EXPR_PREFIX_UNARY, NULL, token->loc);
    }

    SourceLoc loc = token->loc;
    TokenKind op = token->kind;

    token_stream_move_forward(ast_parser->ts);

    ASTNode* rhs = ast_parser_parse_expr_with_prec(ast_parser, OP_PREC_NONE);
    loc.end = rhs->loc.end;

    if (rhs->kind == AST_NODE_ERROR) {
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_EXPR_PREFIX_UNARY);
        return ast_node_error_create(AST_NODE_EXPR_PREFIX_UNARY, rhs, token->loc);
    }

    return ast_node_expr_prefix_unary_create(op, rhs, loc);
}

ASTNode* ast_parser_parse_expr_postfix_unary(ASTParser* ast_parser, ASTNode* lhs) {
    assert(ast_parser != NULL && lhs != NULL);

    SourceLoc loc = lhs->loc;

    const Token* token = token_stream_peek_next(ast_parser->ts);
    loc.end = token->loc.end;

    if (!is_token_kind_a_postfix_unop(token->kind)) {
        ast_node_destroy(lhs);
        TRACE(loc, "Expected postfix unary operator, but got `%s`",
            get_token_kind_value(token->kind)
        );
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_POSTFIX_UNARY);
        return ast_node_error_create(AST_NODE_EXPR_POSTFIX_UNARY, NULL, loc);
    }

    TokenKind op = token->kind;
    token_stream_move_forward(ast_parser->ts);

    return ast_node_expr_postfix_unary_create(op, lhs, loc);
}

ASTNode* ast_parser_parse_expr_braces(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_L_BRACE);
    if (token->kind != TOKEN_L_BRACE) {
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_EXPR_BRACES);
        return ast_node_error_create(AST_NODE_EXPR_BRACES, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* expr = ast_parser_parse_expr_with_prec(ast_parser, OP_PREC_NONE);
    loc.end = expr->loc.end;

    if (expr->kind == AST_NODE_ERROR) {
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_BRACES);
        return ast_node_error_create(AST_NODE_EXPR_BRACES, expr, loc);
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_R_BRACE);
    loc.end = expr->loc.end;

    if (token->kind != TOKEN_R_BRACE) {
        ast_node_destroy(expr);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_BRACES);
        return ast_node_error_create(AST_NODE_EXPR_BRACES, NULL, loc);
    }

    return ast_node_expr_braces_create(expr, loc);
}

ASTNode* ast_parser_parse_expr_place(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_IDENTIFIER);
    if (token->kind != TOKEN_IDENTIFIER) {
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_EXPR_PLACE);
        return ast_node_error_create(AST_NODE_EXPR_PLACE, NULL, token->loc);
    }

    String msg = string_clone(&token->value);

    return ast_node_expr_place_create(msg, token->loc);
}

ASTNode* ast_parser_parse_expr_call(ASTParser* ast_parser, ASTNode* callee) {
    assert(ast_parser != NULL && callee != NULL);

    SourceLoc loc = callee->loc;

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_L_BRACE);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_L_BRACE) {
        ast_node_destroy(callee);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_CALL);
        return ast_node_error_create(AST_NODE_EXPR_CALL, NULL, loc);
    }

    Vector args = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    token = token_stream_peek_next(ast_parser->ts);
    while (!token_stream_is_end(ast_parser->ts)) {
        if (args.size == 0 && token->kind == TOKEN_R_BRACE) {
            break;
        }

        ASTNode* arg = ast_parser_parse_expr_with_prec(ast_parser, OP_PREC_NONE);
        loc.end = arg->loc.end;

        if (arg->kind == AST_NODE_ERROR) {
            vector_destroy(&args);
            ast_node_destroy(callee);
            TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_CALL);
            return ast_node_error_create(AST_NODE_EXPR_CALL, arg, loc);
        }

        vector_push_back(&args, &arg);

        token = token_stream_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            token_stream_move_forward(ast_parser->ts);
            continue;
        }
        break;
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_R_BRACE);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_BRACE) {
        vector_destroy(&args);
        ast_node_destroy(callee);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_CALL);
        return ast_node_error_create(AST_NODE_EXPR_CALL, NULL, loc);
    }

    return ast_node_expr_call_create(callee, args, loc);
}

ASTNode* ast_parser_parse_expr_index(ASTParser* ast_parser, ASTNode* callee) {
    assert(ast_parser != NULL && callee != NULL);

    SourceLoc loc = callee->loc;

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_L_BRACKET);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_L_BRACKET) {
        ast_node_destroy(callee);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_INDEX);
        return ast_node_error_create(AST_NODE_EXPR_INDEX, NULL, loc);
    }

    Vector args = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    do {
        ASTNode* arg = ast_parser_parse_expr_with_prec(ast_parser, OP_PREC_NONE);
        loc.end = arg->loc.end;

        if (arg->kind == AST_NODE_ERROR) {
            vector_destroy(&args);
            ast_node_destroy(callee);
            TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_INDEX);
            return ast_node_error_create(AST_NODE_EXPR_INDEX, arg, loc);
        }

        vector_push_back(&args, &arg);

        token = token_stream_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            token_stream_move_forward(ast_parser->ts);
            continue;
        }
        break;
    } while (!token_stream_is_end(ast_parser->ts));

    token = token_stream_advance_if(ast_parser->ts, TOKEN_R_BRACKET);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_BRACKET) {
        vector_destroy(&args);
        ast_node_destroy(callee);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_INDEX);
        return ast_node_error_create(AST_NODE_EXPR_INDEX, NULL, loc);
    }

    return ast_node_expr_index_create(callee, args, loc);
}

ASTNode* ast_parser_parse_expr_member(ASTParser* ast_parser, ASTNode* object) {
    assert(ast_parser != NULL && object != NULL);

    SourceLoc loc = object->loc;

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_DOT);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_DOT) {
        ast_node_destroy(object);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_MEMBER);
        return ast_node_error_create(AST_NODE_EXPR_MEMBER, NULL, loc);
    }

    ASTNode* member = ast_parser_parse_expr_with_prec(ast_parser, OP_PREC_NONE);
    loc.end = member->loc.end;

    if (member->kind == AST_NODE_ERROR) {
        ast_node_destroy(object);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_EXPR_MEMBER);
        return ast_node_error_create(AST_NODE_EXPR_MEMBER, member, loc);
    }

    return ast_node_expr_member_create(object, member, loc);
}

ASTNode* ast_parser_parse_expr_literal(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_peek_next(ast_parser->ts);
    if (!is_token_kind_a_literal(token->kind)) {
        TRACE(token->loc, "Expected literal, but got `%s`",
            get_token_kind_value(token->kind)
        );
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_EXPR_LITERAL);
        return ast_node_error_create(AST_NODE_EXPR_LITERAL, NULL, token->loc);
    }

    SourceLoc loc = token->loc;
    TokenKind kind = token->kind;
    String value = string_clone(&token->value);

    token_stream_move_forward(ast_parser->ts);

    return ast_node_expr_literal_create(kind, value, loc);
}