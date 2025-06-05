#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/vector.h"
#include "vane/utils/source_loc.h"

#include "vane/scanner/token_kind.h"

#include "vane/ast/ast_node_kind.h"

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeKind kind;

    union {
#define AST_NODE(KIND, NAME, ...) __VA_ARGS__
#include "vane/ast/ast_node_kind.def"
    } as;

    SourceLoc loc;
};

ASTNode* ast_node_create(ASTNodeKind kind, SourceLoc loc);

void ast_node_destroy(ASTNode* node);

//
ASTNode* ast_node_error_create(ASTNode* next, SourceLoc loc);

ASTNode* ast_node_identifier_create(String value, SourceLoc loc);

ASTNode* ast_node_package_decl_create(ASTNode* id, SourceLoc loc);

ASTNode* ast_node_import_decl_create(ASTNode* path, ASTNode* alias, SourceLoc loc);

ASTNode* ast_node_type_builtin_create(TokenKind kind, SourceLoc loc);

ASTNode* ast_node_type_custom_create(String value, SourceLoc loc);

ASTNode* ast_node_type_ptr_create(ASTNode* type, SourceLoc loc);

ASTNode* ast_node_type_arr_create(ASTNode* type, ASTNode* expr, SourceLoc loc);

ASTNode* ast_node_func_param_create(ASTNode* id, ASTNode* type, SourceLoc loc);

ASTNode* ast_node_func_sign_create(ASTNode* id, Vector params, ASTNode* type, SourceLoc loc);

ASTNode* ast_node_func_decl_create(ASTNode* sign, Vector body, SourceLoc loc);

ASTNode* ast_node_stmt_block_create(Vector stmts, SourceLoc loc);

ASTNode* ast_node_const_item_create(ASTNode* id, ASTNode* type, ASTNode* expr, SourceLoc loc);

ASTNode* ast_node_stmt_const_decl_create(Vector items, SourceLoc loc);

ASTNode* ast_node_var_item_create(ASTNode* id, ASTNode* type, ASTNode* expr, SourceLoc loc);

ASTNode* ast_node_stmt_var_decl_create(Vector items, SourceLoc loc);

ASTNode* ast_node_branch_create(ASTNode* expr, Vector block, SourceLoc loc);

ASTNode* ast_node_stmt_condition_create(Vector branches, SourceLoc loc);

ASTNode* ast_node_stmt_while_create(ASTNode* expr, Vector block, SourceLoc loc);

ASTNode* ast_node_stmt_do_create(ASTNode* expr, Vector block, SourceLoc loc);

ASTNode* ast_node_stmt_break_create(SourceLoc loc);

ASTNode* ast_node_stmt_continue_create(SourceLoc loc);

ASTNode* ast_node_stmt_defer_create(ASTNode* defer, SourceLoc loc);

ASTNode* ast_node_stmt_expr_create(ASTNode* expr, SourceLoc loc);

ASTNode* ast_node_stmt_return_create(ASTNode* expr, SourceLoc loc);

ASTNode* ast_node_expr_binary_create(TokenKind op, ASTNode* lhs, ASTNode* rhs, SourceLoc loc);

ASTNode* ast_node_expr_prefix_unary_create(TokenKind op, ASTNode* rhs, SourceLoc loc);

ASTNode* ast_node_expr_postfix_unary_create(TokenKind op, ASTNode* lhs, SourceLoc loc);

ASTNode* ast_node_expr_braces_create(ASTNode* expr, SourceLoc loc);

ASTNode* ast_node_expr_call_create(ASTNode* callee, Vector args, SourceLoc loc);

ASTNode* ast_node_expr_index_create(ASTNode* callee, Vector args, SourceLoc loc);

ASTNode* ast_node_expr_cast_create(ASTNode* type, ASTNode* expr, SourceLoc loc);

ASTNode* ast_node_expr_place_create(String value, SourceLoc loc);

ASTNode* ast_node_expr_member_create(ASTNode* object, ASTNode* member, SourceLoc loc);

ASTNode* ast_node_expr_init_list_create(Vector items, SourceLoc loc);

ASTNode* ast_node_expr_nil_create(SourceLoc loc);

ASTNode* ast_node_expr_literal_create(TokenKind kind, String value, SourceLoc loc);

ASTNode* ast_node_expr_string_literal_create(String value, SourceLoc loc);

ASTNode* ast_node_expr_char_literal_create(String value, SourceLoc loc);

ASTNode* ast_node_expr_dec_literal_create(String value, SourceLoc loc);

ASTNode* ast_node_expr_hex_literal_create(String value, SourceLoc loc);

ASTNode* ast_node_expr_bin_literal_create(String value, SourceLoc loc);

ASTNode* ast_node_expr_bool_literal_create(String value, SourceLoc loc);