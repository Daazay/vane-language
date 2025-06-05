#include "vane/ast/ast_node.h"

#include <stdlib.h>

ASTNode* ast_node_create(ASTNodeKind kind, SourceLoc loc) {
    ASTNode* node = malloc(sizeof(ASTNode));
    assert(node != NULL);

    node->kind = kind;
    node->loc = loc;

    return node;
}

void ast_node_destroy(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
    case AST_NODE_ERROR:
        ast_node_destroy(node->as.error.next);
        break;
    case AST_NODE_IDENTIFIER:
        string_destroy(&node->as.identifier.value);
        break;
    case AST_NODE_PACKAGE_DECL:
        ast_node_destroy(node->as.package_decl.id);
        break;
    case AST_NODE_IMPORT_DECL:
        ast_node_destroy(node->as.import_decl.path);
        ast_node_destroy(node->as.import_decl.alias);
        break;
    case AST_NODE_TYPE_U8:
    case AST_NODE_TYPE_I8:
    case AST_NODE_TYPE_U16:
    case AST_NODE_TYPE_I16:
    case AST_NODE_TYPE_U32:
    case AST_NODE_TYPE_I32:
    case AST_NODE_TYPE_U64:
    case AST_NODE_TYPE_I64:
        break;
    case AST_NODE_TYPE_CUSTOM:
        string_destroy(&node->as.type_custom.value);
        break;
    case AST_NODE_TYPE_PTR:
        ast_node_destroy(node->as.type_ptr.type);
        break;
    case AST_NODE_TYPE_ARR:
        ast_node_destroy(node->as.type_arr.type);
        ast_node_destroy(node->as.type_arr.expr );
        break;
    case AST_NODE_FUNC_PARAM:
        ast_node_destroy(node->as.func_param.id);
        ast_node_destroy(node->as.func_param.type);
        break;
    case AST_NODE_FUNC_SIGN:
        ast_node_destroy(node->as.func_sign.id);
        vector_destroy(&node->as.func_sign.params);
        ast_node_destroy(node->as.func_sign.type);
        break;
    case AST_NODE_FUNC_DECL:
        ast_node_destroy(node->as.func_decl.sign);
        vector_destroy(&node->as.func_decl.body);
        break;
    case AST_NODE_STMT_EMPTY:
        break;
    case AST_NODE_STMT_BLOCK:
        vector_destroy(&node->as.stmt_block.stmts);
        break;
    case AST_NODE_CONST_ITEM:
        ast_node_destroy(node->as.const_item.id);
        ast_node_destroy(node->as.const_item.type);
        ast_node_destroy(node->as.const_item.expr);
        break;
    case AST_NODE_STMT_CONST_DECL:
        vector_destroy(&node->as.stmt_const_decl.items);
        break;
    case AST_NODE_VAR_ITEM:
        ast_node_destroy(node->as.var_item.id);
        ast_node_destroy(node->as.var_item.type);
        ast_node_destroy(node->as.var_item.expr);
        break;
    case AST_NODE_STMT_VAR_DECL:
        vector_destroy(&node->as.stmt_var_decl.items);
        break;
    case AST_NODE_BRANCH:
        ast_node_destroy(node->as.branch.expr);
        vector_destroy(&node->as.branch.block);
        break;
    case AST_NODE_STMT_CONDITION:
        vector_destroy(&node->as.stmt_condition.branches);
        break;
    case AST_NODE_STMT_WHILE:
        ast_node_destroy(node->as.stmt_while.expr);
        vector_destroy(&node->as.stmt_while.block);
        break;
    case AST_NODE_STMT_DO:
        ast_node_destroy(node->as.stmt_do.expr);
        vector_destroy(&node->as.stmt_do.block);
        break;
    case AST_NODE_STMT_BREAK:
        break;
    case AST_NODE_STMT_CONTINUE:
        break;
    case AST_NODE_STMT_DEFER:
        ast_node_destroy(node->as.stmt_defer.defer);
        break;
    case AST_NODE_STMT_EXPR:
        ast_node_destroy(node->as.stmt_expr.expr);
        break;
    case AST_NODE_STMT_RETURN:
        ast_node_destroy(node->as.stmt_return.expr);
        break;
    case AST_NODE_EXPR_BINARY:
        ast_node_destroy(node->as.expr_binary.lhs);
        ast_node_destroy(node->as.expr_binary.rhs);
        break;
    case AST_NODE_EXPR_PREFIX_UNARY:
        ast_node_destroy(node->as.expr_prefix_unary.rhs);
        break;
    case AST_NODE_EXPR_POSTFIX_UNARY:
        ast_node_destroy(node->as.expr_postfix_unary.lhs);
        break;
    case AST_NODE_EXPR_BRACES:
        ast_node_destroy(node->as.expr_braces.expr);
        break;
    case AST_NODE_EXPR_CALL:
        ast_node_destroy(node->as.expr_call.callee);
        vector_destroy(&node->as.expr_call.args);
        break;
    case AST_NODE_EXPR_INDEX:
        ast_node_destroy(node->as.expr_index.callee);
        vector_destroy(&node->as.expr_index.args);
        break;
    case AST_NODE_EXPR_CAST:
        ast_node_destroy(node->as.expr_cast.type);
        ast_node_destroy(node->as.expr_cast.expr);
        break;
    case AST_NODE_EXPR_PLACE:
        string_destroy(&node->as.expr_place.value);
        break;
    case AST_NODE_EXPR_MEMBER:
        ast_node_destroy(node->as.expr_member.object);
        ast_node_destroy(node->as.expr_member.member);
        break;
    case AST_NODE_EXPR_INIT_LIST:
        vector_destroy(&node->as.expr_init_list.items);
        break;
    case AST_NODE_EXPR_NIL:
        break;
    case AST_NODE_LITERAL_STRING:
        string_destroy(&node->as.literal.value);
        break;
    case AST_NODE_LITERAL_CHAR:
        string_destroy(&node->as.literal.value);
        break;
    case AST_NODE_LITERAL_DEC:
        string_destroy(&node->as.literal.value);
        break;
    case AST_NODE_LITERAL_HEX:
        string_destroy(&node->as.literal.value);
        break;
    case AST_NODE_LITERAL_BIN:
        string_destroy(&node->as.literal.value);
        break;
    case AST_NODE_LITERAL_BOOL:
        string_destroy(&node->as.literal.value);
        break;
    default:
        unreachable();
        break;
    }

    free(node);
}

ASTNode* ast_node_error_create(ASTNode* next, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_ERROR, loc);
    node->as.error.next = next;
    return node;
}

ASTNode* ast_node_identifier_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_IDENTIFIER, loc);
    node->as.identifier.value = value;
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

ASTNode* ast_node_type_builtin_create(TokenKind kind, SourceLoc loc) {
    switch (kind) {
    case TOKEN_KEYWORD_ANY: return ast_node_create(AST_NODE_TYPE_ANY, loc);
    case TOKEN_KEYWORD_U8:  return ast_node_create(AST_NODE_TYPE_U8, loc);
    case TOKEN_KEYWORD_I8:  return ast_node_create(AST_NODE_TYPE_I8, loc);
    case TOKEN_KEYWORD_U16: return ast_node_create(AST_NODE_TYPE_U16, loc);
    case TOKEN_KEYWORD_I16: return ast_node_create(AST_NODE_TYPE_I16, loc);
    case TOKEN_KEYWORD_U32: return ast_node_create(AST_NODE_TYPE_U32, loc);
    case TOKEN_KEYWORD_I32: return ast_node_create(AST_NODE_TYPE_I32, loc);
    case TOKEN_KEYWORD_U64: return ast_node_create(AST_NODE_TYPE_U64, loc);
    case TOKEN_KEYWORD_I64: return ast_node_create(AST_NODE_TYPE_I64, loc);
    default:
        unreachable();
        return NULL;
    }
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

ASTNode* ast_node_type_arr_create(ASTNode* type, ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_TYPE_ARR, loc);
    node->as.type_arr.type = type;
    node->as.type_arr.expr = expr;
    return node;
}

ASTNode* ast_node_func_param_create(ASTNode* id, ASTNode* type, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_FUNC_PARAM, loc);
    node->as.func_param.id = id;
    node->as.func_param.type = type;
    return node;
}

ASTNode* ast_node_func_sign_create(ASTNode* id, Vector params, ASTNode* type, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_FUNC_SIGN, loc);
    node->as.func_sign.id = id;
    node->as.func_sign.params = params;
    node->as.func_sign.type = type;
    return node;
}

ASTNode* ast_node_func_decl_create(ASTNode* sign, Vector body, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_FUNC_DECL, loc);
    node->as.func_decl.sign = sign;
    node->as.func_decl.body = body;
    return node;
}

ASTNode* ast_node_stmt_block_create(Vector stmts, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_BLOCK, loc);
    node->as.stmt_block.stmts = stmts;
    return node;
}

ASTNode* ast_node_const_item_create(ASTNode* id, ASTNode* type, ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_CONST_ITEM, loc);
    node->as.const_item.id = id;
    node->as.const_item.type = type;
    node->as.const_item.expr = expr;
    return node;
}

ASTNode* ast_node_stmt_const_decl_create(Vector items, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_CONST_DECL, loc);
    node->as.stmt_const_decl.items = items;
    return node;
}

ASTNode* ast_node_var_item_create(ASTNode* id, ASTNode* type, ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_VAR_ITEM, loc);
    node->as.var_item.id = id;
    node->as.var_item.type = type;
    node->as.var_item.expr = expr;
    return node;
}

ASTNode* ast_node_stmt_var_decl_create(Vector items, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_VAR_DECL, loc);
    node->as.stmt_var_decl.items = items;
    return node;
}

ASTNode* ast_node_branch_create(ASTNode* expr, Vector block, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_VAR_DECL, loc);
    node->as.branch.expr = expr;
    node->as.branch.block = block;
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

ASTNode* ast_node_stmt_defer_create(ASTNode* defer, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_DEFER, loc);
    node->as.stmt_defer.defer = defer;
    return node;
}

ASTNode* ast_node_stmt_expr_create(ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_EXPR, loc);
    node->as.stmt_expr.expr = expr;
    return node;
}

ASTNode* ast_node_stmt_return_create(ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_STMT_RETURN, loc);
    node->as.stmt_return.expr = expr;
    return node;
}

ASTNode* ast_node_expr_binary_create(TokenKind op, ASTNode* lhs, ASTNode* rhs, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_BINARY, loc);
    node->as.expr_binary.op = op;
    node->as.expr_binary.lhs = lhs;
    node->as.expr_binary.rhs = rhs;
    return node;
}

ASTNode* ast_node_expr_prefix_unary_create(TokenKind op, ASTNode* rhs, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_PREFIX_UNARY, loc);
    node->as.expr_prefix_unary.op = op;
    node->as.expr_prefix_unary.rhs = rhs;
    return node;
}

ASTNode* ast_node_expr_postfix_unary_create(TokenKind op, ASTNode* lhs, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_POSTFIX_UNARY, loc);
    node->as.expr_postfix_unary.op = op;
    node->as.expr_postfix_unary.lhs = lhs;
    return node;
}

ASTNode* ast_node_expr_braces_create(ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_BRACES, loc);
    node->as.expr_braces.expr = expr;
    return node;
}

ASTNode* ast_node_expr_call_create(ASTNode* callee, Vector args, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_CALL, loc);
    node->as.expr_call.callee = callee;
    node->as.expr_call.args = args;
    return node;
}

ASTNode* ast_node_expr_index_create(ASTNode* callee, Vector args, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_INDEX, loc);
    node->as.expr_index.callee = callee;
    node->as.expr_index.args = args;
    return node;
}

ASTNode* ast_node_expr_cast_create(ASTNode* type, ASTNode* expr, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_CALL, loc);
    node->as.expr_cast.type = type;
    node->as.expr_cast.expr = expr;
    return node;
}

ASTNode* ast_node_expr_place_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_PLACE, loc);
    node->as.expr_place.value = value;
    return node;
}

ASTNode* ast_node_expr_member_create(ASTNode* object, ASTNode* member, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_MEMBER, loc);
    node->as.expr_member.object = object;
    node->as.expr_member.member = member;
    return node;
}

ASTNode* ast_node_expr_init_list_create(Vector items, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_EXPR_INIT_LIST, loc);
    node->as.expr_init_list.items = items;
    return node;
}

ASTNode* ast_node_expr_nil_create(SourceLoc loc) {
    return ast_node_create(AST_NODE_EXPR_NIL, loc);
}

ASTNode* ast_node_expr_literal_create(TokenKind kind, String value, SourceLoc loc) {
    switch (kind) {
    case TOKEN_LITERAL_STRING: return ast_node_expr_string_literal_create(value, loc);
    case TOKEN_LITERAL_CHAR:   return ast_node_expr_char_literal_create(value, loc);
    case TOKEN_LITERAL_DEC:    return ast_node_expr_dec_literal_create(value, loc);
    case TOKEN_LITERAL_HEX:    return ast_node_expr_hex_literal_create(value, loc);
    case TOKEN_LITERAL_BIN:    return ast_node_expr_bin_literal_create(value, loc);
    case TOKEN_LITERAL_BOOL:   return ast_node_expr_bool_literal_create(value, loc);
    default:
        unreachable();
        return NULL;
    }
}

ASTNode* ast_node_expr_string_literal_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_LITERAL_STRING, loc);
    node->as.literal.value = value;
    return node;
}

ASTNode* ast_node_expr_char_literal_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_LITERAL_CHAR, loc);
    node->as.literal.value = value;
    return node;
}

ASTNode* ast_node_expr_dec_literal_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_LITERAL_DEC, loc);
    node->as.literal.value = value;
    return node;
}

ASTNode* ast_node_expr_hex_literal_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_LITERAL_HEX, loc);
    node->as.literal.value = value;
    return node;
}

ASTNode* ast_node_expr_bin_literal_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_LITERAL_BIN, loc);
    node->as.literal.value = value;
    return node;
}

ASTNode* ast_node_expr_bool_literal_create(String value, SourceLoc loc) {
    ASTNode* node = ast_node_create(AST_NODE_LITERAL_BOOL, loc);
    node->as.literal.value = value;
    return node;
}