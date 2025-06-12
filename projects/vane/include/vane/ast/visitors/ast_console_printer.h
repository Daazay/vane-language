#pragma once

#include "vane/ast/ast_node_visitor.h"

typedef struct ASTConsolePrinter ASTConsolePrinter;

struct ASTConsolePrinter {
    u32 indent;
};

void ast_console_printer_pre_fn(const ASTNode* parent, const ASTNode* node, void* uctx);

void ast_console_printer_post_fn(const ASTNode* parent, const ASTNode* node, void* uctx);

void ast_console_printer_print_node(const ASTNode* node);