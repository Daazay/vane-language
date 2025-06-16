#include "vane/ast/ast_parser.h"

#pragma region DIAGNOSTIC

#define REPORT_FAILED_TO_PARSE_STR(LOC, STR) REPORT_COLLECTOR_REPORT_SYNTAX_ERROR(ast_parser->rc, LOC, "Failed to parse `%s`", STR)
#define REPORT_FAILED_TO_PARSE_AST(LOC, AST) REPORT_FAILED_TO_PARSE_STR(LOC, get_ast_node_kind_name(AST))

#define TRACE_FAILED_TO_PARSE_STR(LOC, STR) REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, LOC, "Failed to parse `%s`", STR)
#define TRACE_FAILED_TO_PARSE_AST(LOC, AST) TRACE_FAILED_TO_PARSE_STR(LOC, get_ast_node_kind_name(AST))

#pragma endregion

#pragma region NODE_CREATE

ASTNode* ast_node_error_create(ASTNodeKind failed, ASTNode * prev, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_ERROR, loc);
    node->as.error.failed = failed;
    node->as.error.prev = prev;
    return node;
}

ASTNode* ast_node_identifier_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_IDENTIFIER, loc);
    node->as.id.value = value;
    return node;
}

ASTNode* ast_node_type_builtin_create(TokenKind token_kind, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_TYPE_BUILTIN, loc);
    node->as.type_builtin.kind = token_kind;
    return node;
}

ASTNode* ast_node_type_custom_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_TYPE_CUSTOM, loc);
    node->as.type_custom.value = value;
    return node;
}

ASTNode* ast_node_type_ptr_create(ASTNode* type, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_TYPE_PTR, loc);
    node->as.type_ptr.type = type;
    return node;
}

ASTNode* ast_node_type_arr_create(ASTNode* size_expr, ASTNode* type, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_TYPE_ARR, loc);
    node->as.type_arr.size_expr = size_expr;
    node->as.type_arr.type = type;
    return node;
}

ASTNode* ast_node_package_decl_create(ASTNode* id, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_PACKAGE_DECL, loc);
    node->as.package_decl.id = id;
    return node;
}

ASTNode* ast_node_import_decl_create(ASTNode* path, ASTNode* alias, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_IMPORT_DECL, loc);
    node->as.import_decl.path = path;
    node->as.import_decl.alias = alias;
    return node;
}

ASTNode* ast_node_typealias_decl_create(ASTNode* id, ASTNode* type, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_TYPEALIAS_DECL, loc);
    node->as.typealias_decl.id = id;
    node->as.typealias_decl.type = type;
    return node;
}

ASTNode* ast_node_fun_param_create(ASTNode* id, ASTNode* type, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_FUN_PARAM, loc);
    node->as.fun_param.id = id;
    node->as.fun_param.type = type;
    return node;
}

ASTNode* ast_node_fun_sign_create(ASTNode* id, Vector params, ASTNode* type, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_FUN_SIGN, loc);
    node->as.fun_sign.id = id;
    node->as.fun_sign.params = params;
    node->as.fun_sign.type = type;
    return node;
}

ASTNode* ast_node_fun_decl_create(ASTNode* sign, Vector block, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_FUN_DECL, loc);
    node->as.fun_decl.sign = sign;
    node->as.fun_decl.block = block;
    return node;
}

ASTNode* ast_node_stmt_empty_create(SourceLoc loc) {
    return ast_node_create(AST_NODE_STMT_EMPTY, loc);
}

ASTNode* ast_node_stmt_block_create(Vector block, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_BLOCK, loc);
    node->as.stmt_block.block = block;
    return node;
}

ASTNode* ast_node_stmt_var_item_create(ASTNode* id, ASTNode* type, ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_VAR_ITEM, loc);
    node->as.stmt_var_item.id = id;
    node->as.stmt_var_item.type = type;
    node->as.stmt_var_item.expr = expr;
    return node;
}

ASTNode* ast_node_stmt_var_decl_create(Vector items, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_VAR_DECL, loc);
    node->as.stmt_var_decl.items = items;
    return node;
}

ASTNode* ast_node_stmt_branch_create(ASTNode* expr, Vector block, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_BRANCH, loc);
    node->as.stmt_branch.expr = expr;
    node->as.stmt_branch.block = block;
    return node;
}

ASTNode* ast_node_stmt_condition_create(Vector branches, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_CONDITION, loc);
    node->as.stmt_condition.branches = branches;
    return node;
}

ASTNode* ast_node_stmt_while_create(ASTNode* expr, Vector block, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_WHILE, loc);
    node->as.stmt_while.expr = expr;
    node->as.stmt_while.block = block;
    return node;
}

ASTNode* ast_node_stmt_do_create(ASTNode* expr, Vector block, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_DO, loc);
    node->as.stmt_do.expr = expr;
    node->as.stmt_do.block = block;
    return node;
}

ASTNode* ast_node_stmt_break_create(SourceLoc loc) {
    return ast_node_create(AST_NODE_STMT_BREAK, loc);
}

ASTNode* ast_node_stmt_continue_create(SourceLoc loc) {
    return ast_node_create(AST_NODE_STMT_CONTINUE, loc);
}

ASTNode* ast_node_stmt_return_create(ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_RETURN, loc);
    node->as.stmt_return.expr = expr;
    return node;
}

ASTNode* ast_node_stmt_expr_create(ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_EXPR, loc);
    node->as.stmt_expr.expr = expr;
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

ASTNode* ast_parser_parse_identifier(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_IDENTIFIER);
    if (token->kind != TOKEN_IDENTIFIER) {
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_IDENTIFIER);
        return ast_node_error_create(AST_NODE_IDENTIFIER, NULL, token->loc);
    }

    return ast_node_identifier_create(string_clone(&token->value), token->loc);
}

static ASTNode* ast_parser_parse_type_impl(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_peek_next(ast_parser->ts);
    switch (token->kind) {
    case TOKEN_IDENTIFIER: return ast_parser_parse_type_custom(ast_parser);
    case TOKEN_CARET:      return ast_parser_parse_type_ptr(ast_parser);
    case TOKEN_L_BRACKET:  return ast_parser_parse_type_arr(ast_parser);
    default:
        if (is_token_kind_a_builtin_type(token->kind)) {
            return ast_parser_parse_type_builtin(ast_parser);
        }
        break;
    }

    REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Expected beginning of `%s`, but got `%s`",
        get_ast_node_kind_name(AST_NODE_GROUP_TYPE),
        get_token_kind_value(token->kind)
    );

    return ast_node_error_create(AST_NODE_GROUP_TYPE, NULL, token->loc);
}

ASTNode* ast_parser_parse_type(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* node = ast_parser_parse_type_impl(ast_parser);
    if (node->kind == AST_NODE_ERROR) {
        TRACE_FAILED_TO_PARSE_AST(node->loc, AST_NODE_GROUP_TYPE);
    }
    return node;
}

ASTNode* ast_parser_parse_type_builtin(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_peek_next(ast_parser->ts);
    if (!is_token_kind_a_builtin_type(token->kind)) {
        REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Expected builtin type, but got `%s`",
            get_token_kind_value(token->kind)
        );
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_TYPE_BUILTIN);
        return ast_node_error_create(AST_NODE_TYPE_BUILTIN, NULL, token->loc);
    }

    SourceLoc loc = token->loc;
    TokenKind kind = token->kind;

    token_stream_move_forward(ast_parser->ts);

    return ast_node_type_builtin_create(kind, loc);
}

ASTNode* ast_parser_parse_type_custom(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_IDENTIFIER);
    if (token->kind != TOKEN_IDENTIFIER) {
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_TYPE_CUSTOM);
        return ast_node_error_create(AST_NODE_TYPE_CUSTOM, NULL, token->loc);
    }

    return ast_node_type_custom_create(string_clone(&token->value), token->loc);
}

ASTNode* ast_parser_parse_type_ptr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_CARET);
    if (token->kind != TOKEN_CARET) {
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_TYPE_PTR);
        return ast_node_error_create(AST_NODE_TYPE_PTR, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* type = ast_parser_parse_type_impl(ast_parser);
    loc.end = type->loc.end;

    if (type->kind == AST_NODE_ERROR) {
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_TYPE_PTR);
        return ast_node_error_create(AST_NODE_TYPE_PTR, type, loc);
    }

    return ast_node_type_ptr_create(type, loc);
}

ASTNode* ast_parser_parse_type_arr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_L_BRACKET);
    if (token->kind != TOKEN_L_BRACKET) {
        TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_TYPE_ARR);
        return ast_node_error_create(AST_NODE_TYPE_ARR, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* size_expr = NULL;

    token = token_stream_peek_next(ast_parser->ts);
    if (token->kind != TOKEN_R_BRACKET) {
        size_expr = ast_parser_parse_expr(ast_parser);
        loc.end = size_expr->loc.end;

        if (size_expr->kind == AST_NODE_ERROR) {
            TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_TYPE_ARR);
            return ast_node_error_create(AST_NODE_TYPE_ARR, size_expr, loc);
        }
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_R_BRACKET);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_R_BRACKET) {
        ast_node_destroy(size_expr);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_TYPE_ARR);
        return ast_node_error_create(AST_NODE_TYPE_ARR, NULL, loc);
    }

    ASTNode* type = ast_parser_parse_type_impl(ast_parser);
    loc.end = type->loc.end;

    if (type->kind == AST_NODE_ERROR) {
        ast_node_destroy(size_expr);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_TYPE_ARR);
        return ast_node_error_create(AST_NODE_TYPE_ARR, type, loc);
    }

    return ast_node_type_arr_create(size_expr, type, loc);
}

ASTNode* ast_parser_parse_package_entity(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_peek_next(ast_parser->ts);
    switch (token->kind) {
    case TOKEN_KEYWORD_PACKAGE:   return ast_parser_parse_package_decl(ast_parser);
    case TOKEN_KEYWORD_IMPORT:    return ast_parser_parse_import_decl(ast_parser);
    case TOKEN_KEYWORD_TYPEALIAS: return ast_parser_parse_typealias_decl(ast_parser);
    case TOKEN_KEYWORD_FUN:       return ast_parser_parse_fun_decl(ast_parser);
    default:
        break;
    }

    REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Unexpected token `%s` at package scope",
        get_token_kind_value(token->kind)
    );
    REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_GROUP_PACKAGE_ENTITY);

    return ast_node_error_create(AST_NODE_GROUP_PACKAGE_ENTITY, NULL, token->loc);
}

ASTNode* ast_parser_parse_package_decl(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_PACKAGE);
    if (token->kind != TOKEN_KEYWORD_PACKAGE) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_PACKAGE_DECL);
        return ast_node_error_create(AST_NODE_PACKAGE_DECL, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* id = ast_parser_parse_identifier(ast_parser);
    loc.end = id->loc.end;

    if (id->kind == AST_NODE_ERROR) {
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_PACKAGE_DECL);
        return ast_node_error_create(AST_NODE_PACKAGE_DECL, id, loc);
    }

    return ast_node_package_decl_create(id, loc);
}

ASTNode* ast_parser_parse_import_decl(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_IMPORT);
    if (token->kind != TOKEN_KEYWORD_IMPORT) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_IMPORT_DECL);
        return ast_node_error_create(AST_NODE_IMPORT_DECL, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    token = token_stream_advance_if(ast_parser->ts, TOKEN_LITERAL_STRING);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_LITERAL_STRING) {
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_IMPORT_DECL);
        return ast_node_error_create(AST_NODE_IMPORT_DECL, NULL, loc);
    }

    ASTNode* path = ast_node_expr_literal_create(TOKEN_LITERAL_STRING, string_clone(&token->value), token->loc);

    ASTNode* alias = NULL;

    token = token_stream_peek_next(ast_parser->ts);
    if (token->kind == TOKEN_KEYWORD_AS) {
        alias = ast_parser_parse_identifier(ast_parser);
        loc.end = alias->loc.end;

        if (alias->kind == AST_NODE_ERROR) {
            ast_node_destroy(path);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_IMPORT_DECL);
            return ast_node_error_create(AST_NODE_IMPORT_DECL, alias, loc);
        }
    }

    return ast_node_import_decl_create(path, alias, loc);
}

ASTNode* ast_parser_parse_typealias_decl(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_TYPEALIAS);
    if (token->kind != TOKEN_KEYWORD_TYPEALIAS) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_TYPEALIAS_DECL);
        return ast_node_error_create(AST_NODE_TYPEALIAS_DECL, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* id = ast_parser_parse_identifier(ast_parser);
    loc.end = id->loc.end;

    if (id->kind == AST_NODE_ERROR) {
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_TYPEALIAS_DECL);
        return ast_node_error_create(AST_NODE_TYPEALIAS_DECL, id, loc);
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_EQUAL);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_EQUAL) {
        ast_node_destroy(id);
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_TYPEALIAS_DECL);
        return ast_node_error_create(AST_NODE_TYPEALIAS_DECL, NULL, loc);
    }

    ASTNode* type = ast_parser_parse_type(ast_parser);
    loc.end = type->loc.end;

    if (id->kind == AST_NODE_ERROR) {
        ast_node_destroy(id);
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_TYPEALIAS_DECL);
        return ast_node_error_create(AST_NODE_TYPEALIAS_DECL, type, loc);
    }

    return ast_node_typealias_decl_create(id, type, loc);
}

ASTNode* ast_parser_parse_fun_param(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* id = ast_parser_parse_identifier(ast_parser);
    if (id->kind == AST_NODE_ERROR) {
        TRACE_FAILED_TO_PARSE_AST(id->loc, AST_NODE_FUN_PARAM);
        return ast_node_error_create(AST_NODE_FUN_PARAM, id, id->loc);
    }

    SourceLoc loc = id->loc;

    ASTNode* type = NULL;

    const Token* token = token_stream_peek_next(ast_parser->ts);
    if (token->kind == TOKEN_COLON) {
        token_stream_move_forward(ast_parser->ts);

        type = ast_parser_parse_type(ast_parser);
        loc.end = type->loc.end;

        if (type->kind == AST_NODE_ERROR) {
            ast_node_destroy(id);
            TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_PARAM);
            return ast_node_error_create(AST_NODE_FUN_PARAM, type, loc);
        }
    }

    return ast_node_fun_param_create(id, type, loc);
}

ASTNode* ast_parser_parse_fun_sign(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* id = ast_parser_parse_identifier(ast_parser);
    if (id->kind == AST_NODE_ERROR) {
        TRACE_FAILED_TO_PARSE_AST(id->loc, AST_NODE_FUN_SIGN);
        return ast_node_error_create(AST_NODE_FUN_SIGN, id, id->loc);
    }

    SourceLoc loc = id->loc;

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_L_BRACE);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_L_BRACE) {
        ast_node_destroy(id);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_SIGN);
        return ast_node_error_create(AST_NODE_FUN_SIGN, NULL, loc);
    }

    Vector params = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    token = token_stream_peek_next(ast_parser->ts);
    while (!token_stream_is_end(ast_parser->ts)) {
        if (params.size == 0 && token->kind == TOKEN_R_BRACE) {
            break;
        }

        ASTNode* param = ast_parser_parse_fun_param(ast_parser);
        loc.end = param->loc.end;

        if (param->kind == AST_NODE_ERROR) {
            ast_node_destroy(id);
            vector_destroy(&params);
            TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_SIGN);
            return ast_node_error_create(AST_NODE_FUN_SIGN, param, loc);
        }

        vector_push_back(&params, &param);

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
        ast_node_destroy(id);
        vector_destroy(&params);
        TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_SIGN);
        return ast_node_error_create(AST_NODE_FUN_SIGN, NULL, loc);
    }

    ASTNode* type = NULL;

    token = token_stream_peek_next(ast_parser->ts);
    if (token->kind == TOKEN_COLON) {
        token_stream_move_forward(ast_parser->ts);

        type = ast_parser_parse_type(ast_parser);
        loc.end = type->loc.end;

        if (type->kind == AST_NODE_ERROR) {
            ast_node_destroy(id);
            vector_destroy(&params);
            TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_SIGN);
            return ast_node_error_create(AST_NODE_FUN_SIGN, type, loc);
        }
    }

    return ast_node_fun_sign_create(id, params, type, loc);
}

ASTNode* ast_parser_parse_fun_decl(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_FUN);
    if (token->kind != TOKEN_KEYWORD_FUN) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_FUN_DECL);
        return ast_node_error_create(AST_NODE_FUN_DECL, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* sign = ast_parser_parse_fun_sign(ast_parser);
    loc.end = sign->loc.end;

    if (sign->kind == AST_NODE_ERROR) {
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_DECL);
        return ast_node_error_create(AST_NODE_FUN_DECL, sign, loc);
    }

    Vector block = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    token = token_stream_peek_next(ast_parser->ts);
    if (token->kind == TOKEN_EQUAL) {
        token_stream_move_forward(ast_parser->ts);

        ASTNode* expr = ast_parser_parse_expr(ast_parser);
        loc.end = expr->loc.end;

        if (expr->kind == AST_NODE_ERROR) {
            ast_node_destroy(sign);
            vector_destroy(&block);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_DECL);
            return ast_node_error_create(AST_NODE_FUN_DECL, expr, loc);
        }

        ASTNode* return_stmt = ast_node_stmt_return_create(expr, expr->loc);
        vector_push_back(&block, &return_stmt);
    }
    else {
        while (!token_stream_is_end(ast_parser->ts) && token->kind != TOKEN_KEYWORD_END) {
            ASTNode* stmt = ast_parser_parse_stmt(ast_parser);
            loc.end = stmt->loc.end;

            if (stmt->kind == AST_NODE_ERROR) {
                ast_node_destroy(sign);
                vector_destroy(&block);
                REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_DECL);
                return ast_node_error_create(AST_NODE_FUN_DECL, stmt, loc);
            }

            vector_push_back(&block, &stmt);

            token = token_stream_peek_next(ast_parser->ts);
        }

        token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_END);
        loc.end = token->loc.end;

        if (token->kind != TOKEN_KEYWORD_END) {
            ast_node_destroy(sign);
            vector_destroy(&block);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_FUN_DECL);
            return ast_node_error_create(AST_NODE_FUN_DECL, NULL, loc);
        }
    }

    return ast_node_fun_decl_create(sign, block, loc);
}

ASTNode* ast_parser_parse_stmt(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_peek_next(ast_parser->ts);

    if (is_token_kind_a_beginning_of_stmt(token->kind)) {
        switch (token->kind) {
        case TOKEN_KEYWORD_BEGIN:    return ast_parser_parse_stmt_block(ast_parser);
        case TOKEN_KEYWORD_VAR:      return ast_parser_parse_stmt_var_decl(ast_parser);
        case TOKEN_KEYWORD_IF:       return ast_parser_parse_stmt_condition(ast_parser);
        case TOKEN_KEYWORD_WHILE:    return ast_parser_parse_stmt_while(ast_parser);
        case TOKEN_KEYWORD_DO:       return ast_parser_parse_stmt_do(ast_parser);
        case TOKEN_KEYWORD_BREAK:    return ast_parser_parse_stmt_break(ast_parser);
        case TOKEN_KEYWORD_CONTINUE: return ast_parser_parse_stmt_continue(ast_parser);
        case TOKEN_KEYWORD_RETURN:   return ast_parser_parse_stmt_return(ast_parser);
        case TOKEN_SEMICOLON:
            SourceLoc loc = token->loc;
            token_stream_move_forward(ast_parser->ts);
            return ast_node_stmt_empty_create(loc);
        default:
            return ast_parser_parse_stmt_expr(ast_parser);
        }
    }

    REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Expected beginning of `%s`, but got `%s`",
        get_ast_node_kind_name(AST_NODE_GROUP_STMT),
        get_token_kind_value(token->kind)
    );
    TRACE_FAILED_TO_PARSE_AST(token->loc, AST_NODE_GROUP_STMT);

    return ast_node_error_create(AST_NODE_GROUP_STMT, NULL, token->loc);
}

ASTNode* ast_parser_parse_stmt_block(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_BEGIN);
    if (token->kind != TOKEN_KEYWORD_BEGIN) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_BLOCK);
        return ast_node_error_create(AST_NODE_STMT_BLOCK, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    Vector block = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    while (!token_stream_is_end(ast_parser->ts) && token_stream_peek_next(ast_parser->ts)->kind != TOKEN_KEYWORD_END) {
        ASTNode* stmt = ast_parser_parse_stmt(ast_parser);
        loc.end = stmt->loc.end;

        if (stmt->kind == AST_NODE_ERROR) {
            vector_destroy(&block);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_BLOCK);
            return ast_node_error_create(AST_NODE_STMT_BLOCK, stmt, loc);
        }

        vector_push_back(&block, &stmt);
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_END);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_KEYWORD_END) {
        vector_destroy(&block);
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_BLOCK);
        return ast_node_error_create(AST_NODE_STMT_BLOCK, NULL, loc);
    }

    return ast_node_stmt_block_create(block, loc);
}

ASTNode* ast_parser_parse_stmt_var_item(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* id = ast_parser_parse_identifier(ast_parser);
    if (id->kind == AST_NODE_ERROR) {
        TRACE_FAILED_TO_PARSE_AST(id->loc, AST_NODE_STMT_VAR_ITEM);
        return ast_node_error_create(AST_NODE_STMT_VAR_ITEM, id, id->loc);
    }

    SourceLoc loc = id->loc;

    ASTNode* type = NULL;

    const Token* token = token_stream_peek_next(ast_parser->ts);

    if (token->kind == TOKEN_COLON) {
        token_stream_move_forward(ast_parser->ts);

        type = ast_parser_parse_type(ast_parser);
        loc.end = type->loc.end;

        if (type->kind == AST_NODE_ERROR) {
            ast_node_destroy(id);
            TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_VAR_ITEM);
            return ast_node_error_create(AST_NODE_STMT_VAR_ITEM, type, loc);
        }
    }

    ASTNode* expr = NULL;

    token = token_stream_peek_next(ast_parser->ts);

    if (token->kind == TOKEN_EQUAL) {
        token_stream_move_forward(ast_parser->ts);

        expr = ast_parser_parse_expr(ast_parser);
        loc.end = expr->loc.end;

        if (expr->kind == AST_NODE_ERROR) {
            ast_node_destroy(id);
            ast_node_destroy(type);
            TRACE_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_VAR_ITEM);
            return ast_node_error_create(AST_NODE_STMT_VAR_ITEM, expr, loc);
        }
    }

    return ast_node_stmt_var_item_create(id, type, expr, loc);
}

ASTNode* ast_parser_parse_stmt_var_decl(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_VAR);
    if (token->kind != TOKEN_KEYWORD_VAR) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_VAR_DECL);
        return ast_node_error_create(AST_NODE_STMT_VAR_DECL, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    Vector items = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    do {
        ASTNode* item = ast_parser_parse_stmt_var_item(ast_parser);
        loc.end = item->loc.end;

        if (item->kind == AST_NODE_ERROR) {
            vector_destroy(&items);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_VAR_DECL);
            return ast_node_error_create(AST_NODE_STMT_VAR_DECL, item, loc);
        }

        vector_push_back(&items, &item);

        token = token_stream_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_COMMA) {
            token_stream_move_forward(ast_parser->ts);
            continue;
        }
        break;
    } while (!token_stream_is_end(ast_parser->ts));

    return ast_node_stmt_var_decl_create(items, loc);
}

ASTNode* ast_parser_parse_stmt_branch(ASTParser* ast_parser, bool start_with_else) {
    assert(ast_parser != NULL);

    const Token* token = NULL;
    if (start_with_else) {
        token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_ELSE);
        if (token->kind != TOKEN_KEYWORD_ELSE) {
            REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_BRANCH);
            return ast_node_error_create(AST_NODE_STMT_BRANCH, NULL, token->loc);
        }
    }
    else {
        token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_IF);
        if (token->kind != TOKEN_KEYWORD_IF) {
            REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_BRANCH);
            return ast_node_error_create(AST_NODE_STMT_BRANCH, NULL, token->loc);
        }
    }

    SourceLoc loc = token->loc;
    bool is_else_br = false;

    if (start_with_else) {
        if (token_stream_peek_next(ast_parser->ts)->kind == TOKEN_KEYWORD_IF) {
            token_stream_move_forward(ast_parser->ts);
        }
        else {
            is_else_br = true;
        }
    }

    ASTNode* expr = NULL;
    if (!is_else_br) {
        expr = ast_parser_parse_expr(ast_parser);
        loc.end = expr->loc.end;

        if (expr->kind == AST_NODE_ERROR) {
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_BRANCH);
            return ast_node_error_create(AST_NODE_STMT_BRANCH, expr, loc);
        }

        token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_THEN);
        loc.end = token->loc.end;

        if (token->kind != TOKEN_KEYWORD_THEN) {
            ast_node_destroy(expr);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_BRANCH);
            return ast_node_error_create(AST_NODE_STMT_BRANCH, NULL, loc);
        }
    }

    Vector block = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    while (!token_stream_is_end(ast_parser->ts)) {
        token = token_stream_peek_next(ast_parser->ts);
        if (is_else_br) {
            if (token->kind == TOKEN_KEYWORD_END) {
                break;
            }
        }
        else if ((token->kind == TOKEN_KEYWORD_ELSE) || (token->kind == TOKEN_KEYWORD_END)) {
            break;
        }

        ASTNode* stmt = ast_parser_parse_stmt(ast_parser);
        loc.end = stmt->loc.end;

        if (stmt->kind == AST_NODE_ERROR) {
            ast_node_destroy(expr);
            vector_destroy(&block);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_BRANCH);
            return ast_node_error_create(AST_NODE_STMT_BRANCH, stmt, loc);
        }

        vector_push_back(&block, &stmt);
    }

    if (is_else_br) {
        token = token_stream_expect(ast_parser->ts, TOKEN_KEYWORD_END);
        loc.end = token->loc.end;

        if (token->kind != TOKEN_KEYWORD_END) {
            ast_node_destroy(expr);
            vector_destroy(&block);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_BRANCH);
            return ast_node_error_create(AST_NODE_STMT_BRANCH, NULL, loc);
        }
    }
    else {
        token = token_stream_expect_any(ast_parser->ts, TOKEN_KEYWORD_ELSE, TOKEN_KEYWORD_END);
        loc.end = token->loc.end;

        if (token->kind != TOKEN_KEYWORD_END && token->kind != TOKEN_KEYWORD_ELSE) {
            ast_node_destroy(expr);
            vector_destroy(&block);
            REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_BRANCH);
            return ast_node_error_create(AST_NODE_STMT_BRANCH, NULL, token->loc);
        }
    }

    return ast_node_stmt_branch_create(expr, block, loc);
}

ASTNode* ast_parser_parse_stmt_condition(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_expect(ast_parser->ts, TOKEN_KEYWORD_IF);
    if (token->kind != TOKEN_KEYWORD_IF) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_CONDITION);
        return ast_node_error_create(AST_NODE_STMT_CONDITION, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    Vector branches = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    do {
        ASTNode* branch = ast_parser_parse_stmt_branch(ast_parser, branches.size > 0);
        loc.end = branch->loc.end;

        if (branch->kind == AST_NODE_ERROR) {
            vector_destroy(&branches);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_CONDITION);
            return ast_node_error_create(AST_NODE_STMT_CONDITION, branch, loc);
        }

        vector_push_back(&branches, &branch);

        // in case of else branch
        if (branch->as.stmt_branch.expr == NULL) {
            break;
        }
        token = token_stream_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_KEYWORD_END) {
            break;
        }
    } while (!token_stream_is_end(ast_parser->ts));

    token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_END);
    if (token->kind != TOKEN_KEYWORD_END) {
        vector_destroy(&branches);
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_CONDITION);
        return ast_node_error_create(AST_NODE_STMT_CONDITION, NULL, token->loc);
    }

    return ast_node_stmt_condition_create(branches, loc);
}

ASTNode* ast_parser_parse_stmt_while(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_WHILE);
    if (token->kind != TOKEN_KEYWORD_WHILE) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_WHILE);
        return ast_node_error_create(AST_NODE_STMT_WHILE, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* expr = ast_parser_parse_expr(ast_parser);
    loc.end = expr->loc.end;

    if (expr->kind == AST_NODE_ERROR) {
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_WHILE);
        return ast_node_error_create(AST_NODE_STMT_WHILE, expr, loc);
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_DO);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_KEYWORD_DO) {
        ast_node_destroy(expr);
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_WHILE);
        return ast_node_error_create(AST_NODE_STMT_WHILE, NULL, loc);
    }

    Vector block = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    while (!token_stream_is_end(ast_parser->ts)) {
        token = token_stream_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_KEYWORD_END) {
            break;
        }

        ASTNode* stmt = ast_parser_parse_stmt(ast_parser);
        loc.end = stmt->loc.end;

        if (stmt->kind == AST_NODE_ERROR) {
            ast_node_destroy(expr);
            vector_destroy(&block);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_WHILE);
            return ast_node_error_create(AST_NODE_STMT_WHILE, stmt, loc);
        }

        vector_push_back(&block, &stmt);
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_END);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_KEYWORD_END) {
        ast_node_destroy(expr);
        vector_destroy(&block);
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_WHILE);
        return ast_node_error_create(AST_NODE_STMT_WHILE, NULL, loc);
    }

    return ast_node_stmt_while_create(expr, block, loc);
}

ASTNode* ast_parser_parse_stmt_do(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_DO);
    if (token->kind != TOKEN_KEYWORD_DO) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_DO);
        return ast_node_error_create(AST_NODE_STMT_DO, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    Vector block = vector_create(4, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));

    while (!token_stream_is_end(ast_parser->ts)) {
        token = token_stream_peek_next(ast_parser->ts);
        if (token->kind == TOKEN_KEYWORD_LOOP) {
            break;
        }

        ASTNode* stmt = ast_parser_parse_stmt(ast_parser);
        loc.end = stmt->loc.end;

        if (stmt->kind == AST_NODE_ERROR) {
            vector_destroy(&block);
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_DO);
            return ast_node_error_create(AST_NODE_STMT_DO, stmt, loc);
        }

        vector_push_back(&block, &stmt);
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_LOOP);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_KEYWORD_LOOP) {
        vector_destroy(&block);
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_DO);
        return ast_node_error_create(AST_NODE_STMT_DO, NULL, loc);
    }

    token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_WHILE);
    loc.end = token->loc.end;

    if (token->kind != TOKEN_KEYWORD_WHILE) {
        vector_destroy(&block);
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_DO);
        return ast_node_error_create(AST_NODE_STMT_DO, NULL, loc);
    }

    ASTNode* expr = ast_parser_parse_expr(ast_parser);
    loc.end = expr->loc.end;

    if (expr->kind == AST_NODE_ERROR) {
        vector_destroy(&block);
        REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_DO);
        return ast_node_error_create(AST_NODE_STMT_DO, expr, loc);
    }

    return ast_node_stmt_do_create(expr, block, loc);
}

ASTNode* ast_parser_parse_stmt_break(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_BREAK);
    if (token->kind != TOKEN_KEYWORD_BREAK) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_BREAK);
        return ast_node_error_create(AST_NODE_STMT_BREAK, NULL, token->loc);
    }

    return ast_node_stmt_break_create(token->loc);
}

ASTNode* ast_parser_parse_stmt_continue(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_CONTINUE);
    if (token->kind != TOKEN_KEYWORD_CONTINUE) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_CONTINUE);
        return ast_node_error_create(AST_NODE_STMT_CONTINUE, NULL, token->loc);
    }

    return ast_node_stmt_continue_create(token->loc);
}

ASTNode* ast_parser_parse_stmt_return(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    const Token* token = token_stream_advance_if(ast_parser->ts, TOKEN_KEYWORD_RETURN);
    if (token->kind != TOKEN_KEYWORD_RETURN) {
        REPORT_FAILED_TO_PARSE_AST(token->loc, AST_NODE_STMT_RETURN);
        return ast_node_error_create(AST_NODE_STMT_RETURN, NULL, token->loc);
    }

    SourceLoc loc = token->loc;

    ASTNode* expr = NULL;

    token = token_stream_peek_next(ast_parser->ts);
    if (token->kind != TOKEN_SEMICOLON && token->first_in_line == false) {
        expr = ast_parser_parse_expr(ast_parser);
        loc.end = expr->loc.end;

        if (expr->kind == AST_NODE_ERROR) {
            REPORT_FAILED_TO_PARSE_AST(loc, AST_NODE_STMT_RETURN);
            return ast_node_error_create(AST_NODE_STMT_RETURN, expr, loc);
        }
    }

    return ast_node_stmt_return_create(expr, loc);
}

ASTNode* ast_parser_parse_stmt_expr(ASTParser* ast_parser) {
    assert(ast_parser != NULL);

    ASTNode* expr = ast_parser_parse_expr(ast_parser);
    if (expr->kind == AST_NODE_ERROR) {
        REPORT_FAILED_TO_PARSE_AST(expr->loc, AST_NODE_STMT_EXPR);
        return ast_node_error_create(AST_NODE_STMT_EXPR, expr, expr->loc);
    }

    return ast_node_stmt_expr_create(expr, expr->loc);
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

    const Token* token = NULL;
    while (!token_stream_is_end(ast_parser->ts)) {
        token = token_stream_peek_next(ast_parser->ts);

        if (token->first_in_line) {
            break;
        }

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

    REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Expected beginning of `%s`, but got `%s`",
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

    REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Expected beginning of `%s`, but got `%s`",
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
        REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Expected binary operator, but got `%s`",
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
        REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Expected prefix unary operator, but got `%s`",
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
        REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, loc, "Expected postfix unary operator, but got `%s`",
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

    ASTNode* member = ast_parser_parse_expr_place(ast_parser);
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
        REPORT_COLLECTOR_TRACE_SYNTAX(ast_parser->rc, token->loc, "Expected literal, but got `%s`",
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