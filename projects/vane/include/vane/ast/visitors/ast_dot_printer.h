#pragma once

#include "vane/ast/ast_node_visitor.h"

typedef struct ASTDotPrinter ASTDotPrinter;

struct ASTDotPrinter {
    const ASTNode* parent;
};

void ast_dot_printer_pre_fn(const ASTNode* parent, const ASTNode* node, void* uctx);

void ast_dot_printer_print_node(const ASTNode* node);