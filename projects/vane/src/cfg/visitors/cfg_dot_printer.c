#include "vane/cfg/visitors/cfg_dot_printer.h"

#include <stdio.h>

#include "vane/cfg/visitors/cfg_visitor.h"

static void declare_node(const CFGNode* node, const char* label) {
    const char* color = !node->is_reachable ? "red" : "";

    printf("  n%lld [label=\"%s\", color=\"%s\"];\n",
        (u64)node, label, color
    );
}

static void declare_edge(const CFGNode* from, const CFGNode* to, const char* label) {
    const char* _style = !to->is_reachable ? "dashed" : "";

    printf("  n%lld -> n%lld [label=\"%s\", style=\"%s\"];\n",
        (u64)from, (u64)to, label, _style
    );
}

void cfg_dot_printer_pre_fn(CFGNode* node, CFGNode* next, CFGNode* alt, void* ctx) {
    switch (node->kind) {
    case CFG_NODE_FUN_ENTRY:
        declare_node(node, get_cfg_node_kind_name(node->kind));
        declare_edge(node, node->next, "");
        break;
    case CFG_NODE_FUN_EXIT:
        declare_node(node, get_cfg_node_kind_name(node->kind));
        break;
    case CFG_NODE_BASIC_BLOCK:
        declare_node(node, get_cfg_node_kind_name(node->kind));
        declare_edge(node, node->next, "");
        // edge to unreachable
        if (node->alt != NULL) {
            declare_edge(node, node->alt, "");
        }
        break;
    case CFG_NODE_CONDITION:
        declare_node(node, get_cfg_node_kind_name(node->kind));
        declare_edge(node, node->next, "true");
        declare_edge(node, node->alt, "false");
        break;
    case CFG_NODE_UNREACHABLE:
        declare_node(node, get_cfg_node_kind_name(node->kind));
        declare_edge(node, node->next, "");
        break;
    default:
        unreachable();
        break;
    }

    /*printf("  n%lld [label=\"%s\", color=\"%s\"];\n",
        (u64)node,
        get_cfg_node_kind_name(node->kind),
        (node->is_reachable ? "green" : "red")
    );

    if (next != NULL) {
        if (alt != NULL) {
            printf("  n%lld -> n%lld [label=\"true\", style=\"%s\"];\n", (u64)node, (u64)next,
                ((!node->is_reachable || !next->is_reachable) ? "dashed" : "")
            );
            printf("  n%lld -> n%lld [label=\"false\", style=\"%s\"];\n", (u64)node, (u64)alt,
                ((!node->is_reachable || !alt->is_reachable) ? "dashed" : "")
            );
        }
        else {
            printf("  n%lld -> n%lld[style=\"%s\"];\n", (u64)node, (u64)next,
                ((!node->is_reachable || !next->is_reachable) ? "dashed" : "")
            );
        }
    }*/
}

void cfg_dot_printer_print(const CFGNode* node) {
    CFGVisitor v = {
        .ctx = NULL,
        .visit_flag = !node->visit_flag,
        .pre_fn = &cfg_dot_printer_pre_fn,
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

    cfg_visit_with(node, &v);

    printf("}\n");
}