#include "vane/ast/visitors/ast_dot_printer.h"

#include <stdio.h>

void ast_dot_printer_pre_fn(const ASTNode* parent, const ASTNode* node, void* uctx) {
    ASTDotPrinter* ctx = uctx;

    printf("  n%lld [label=\"%s\"];\n", (u64)node, get_ast_node_kind_name(node->kind));

    if (parent != NULL) {
        printf("  n%lld -> n%lld;\n", (u64)parent, (u64)node);
    }
}

void ast_dot_printer_print_node(const ASTNode* node) {
    ASTDotPrinter ctx = { .parent = NULL };
    ASTVisitor v = {
        .ctx = &ctx,
        .pre_fn = &ast_dot_printer_pre_fn,
        .post_fn = NULL,
    };

    printf(
        "digraph {\n"
        "  ranksep = 0.35;\n"
        "  node [\n"
        "    shape = \"record\",\n"
        "    style = \"solid, filled\",\n"
        "    fontcolor = \"dark\",\n"
        "    fontsize = 12,\n"
        "    width = 0.5,\n"
        "    height = 0.25\n"
        "  ];\n\n"
        "  edge [\n"
        "    arrowsize = 0.6,\n"
        "    color = \"black\",\n"
        "    style = \"light\"\n"
        "  ];\n\n"
    );

    ast_node_visit_with(NULL, node, &v);

    printf("}\n");
}