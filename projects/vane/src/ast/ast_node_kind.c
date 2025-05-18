#include "vane/ast/ast_node_kind.h"

const char* get_ast_node_kind_name(ASTNodeKind kind) {
    switch (kind) {
#define AST_NODE(KIND, NAME, ...) case AST_NODE_##KIND: return NAME;
#include "vane/ast/ast_node_kind.def"
    default:
        unreachable();
        return NULL;
    }
}

bool is_ast_node_kind_a_misc(ASTNodeKind kind) {
    switch (kind) {
#define AST_NODE_MISC(KIND, NAME, ...) case AST_NODE_##KIND: return true;
#include "vane/ast/ast_node_kind.def"
    default:
        unreachable();
        return false;
    }
}


bool is_ast_node_kind_a_type(ASTNodeKind kind) {
    switch (kind) {
#define AST_NODE_TYPE(KIND, NAME, ...) case AST_NODE_TYPE_##KIND: return true;
#include "vane/ast/ast_node_kind.def"
    default:
        unreachable();
        return false;
    }
}

bool is_ast_node_kind_a_stmt(ASTNodeKind kind) {
    switch (kind) {
#define AST_NODE_STMT(KIND, NAME, ...) case AST_NODE_STMT_##KIND: return true;
#include "vane/ast/ast_node_kind.def"
    default:
        unreachable();
        return false;
    }
}

bool is_ast_node_kind_a_expr(ASTNodeKind kind) {
    switch (kind) {
#define AST_NODE_EXPR(KIND, NAME, ...) case AST_NODE_EXPR_##KIND: return true;
#include "vane/ast/ast_node_kind.def"
    default:
        unreachable();
        return false;
    }
}

bool is_ast_node_kind_a_literal(ASTNodeKind kind) {
    switch (kind) {
#define AST_NODE_LITERAL(KIND, NAME, ...) case AST_NODE_LITERAL_##KIND: return true;
#include "vane/ast/ast_node_kind.def"
    default:
        unreachable();
        return false;
    }
}