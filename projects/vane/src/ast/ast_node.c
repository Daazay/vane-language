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
#define AST_NODE(KIND, NAME, STRUCT, DESTROY_FUNCS) \
case AST_NODE_##KIND: \
    DESTROY_FUNCS; \
    break;
#include "vane/ast/ast_node_kind.def"
    default:
        unreachable();
        break;
    }

    free(node);
}