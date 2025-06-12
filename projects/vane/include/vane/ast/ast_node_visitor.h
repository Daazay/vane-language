#pragma once

#include "vane/utils/defines.h"

#include "vane/ast/ast_node.h"

typedef struct ASTVisitor ASTVisitor;

typedef void(*ASTVisitorPreFn)(ASTNode* parent, ASTNode* node, void* ctx);
typedef void(*ASTVisitorPostFn)(ASTNode* parent, ASTNode* node, void* ctx);

struct ASTVisitor {
    ASTVisitorPreFn pre_fn;
    ASTVisitorPreFn post_fn;
    void* ctx;
};

void ast_node_visit_with(ASTNode* parent, ASTNode* node, const ASTVisitor* visitor);