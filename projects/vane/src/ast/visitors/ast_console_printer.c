#include "vane/ast/visitors/ast_console_printer.h"

#include <stdio.h>

void ast_console_printer_pre_fn(const ASTNode* parent, const ASTNode* node, void* uctx) {
    (void)parent;

    ASTConsolePrinter* ctx = uctx;

    printf("[%*u:%*u,%*u:%*u] ",
        3, node->loc.begin.line,
        3, node->loc.begin.column,
        3, node->loc.end.line,
        3, node->loc.end.column
    );

    for (u32 i = 0; i < ctx->indent; ++i) {
        printf("  ");
    }

    printf("%s\n", get_ast_node_kind_name(node->kind));
    ctx->indent++;
}

void ast_console_printer_post_fn(const ASTNode* parent, const ASTNode* node, void* uctx) {
    (void)parent;
    (void)node;

    ASTConsolePrinter* ctx = uctx;
    ctx->indent--;
}

void ast_console_printer_print_node(const ASTNode* node) {
    ASTConsolePrinter ctx = { .indent = 0 };
    ASTVisitor visitor = {
        .ctx = &ctx,
        .pre_fn = &ast_console_printer_pre_fn,
        .post_fn = &ast_console_printer_post_fn,
    };

    ast_node_visit_with(NULL, node, &visitor);
}