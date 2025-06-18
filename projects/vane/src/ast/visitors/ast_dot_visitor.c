#include "vane/ast/visitors/ast_dot_visitor.h"

#include <stdio.h>

typedef struct ASTDotVisitorCtx ASTDotVisitorCtx;

struct ASTDotVisitorCtx {
    void* out;
};

void ast_dot_visitor_print_fn(ASTNode* parent, ASTNode* node, void* data) {
    ASTDotVisitorCtx* ctx = data;

    fprintf(ctx->out, "  n%lld [label=\"%s\"];\n", (u64)node, get_ast_node_kind_name(node->kind));

    if (parent != NULL) {
        fprintf(ctx->out, "  n%lld -> n%lld;\n", (u64)parent, (u64)node);
    }
}

void ast_print_dot(const ASTNode* node, void* stream) {
    assert(node != NULL && stream != NULL);

    ASTDotVisitorCtx ctx = { .out = stream, };
    ASTVisitor v = {
        .data = &ctx,
        .pre_fn = &ast_dot_visitor_print_fn,
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

    ast_visit_with(NULL, node, &v);

    fprintf(stream, "}\n");
}