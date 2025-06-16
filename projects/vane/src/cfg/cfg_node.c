#include "vane/cfg/cfg_node.h"

#include <stdlib.h>

const char* get_cfg_node_kind_name(CFGNodeKind kind) {
    switch (kind) {
    case CFG_NODE_UNKNOWN:     return "unknown";
    case CFG_NODE_FUN_ENTRY:   return "fun_entry";
    case CFG_NODE_FUN_EXIT:    return "fun_exit";
    case CFG_NODE_BASIC_BLOCK: return "basic_block";
    case CFG_NODE_CONDITION:   return "condition";
    case CFG_NODE_UNREACHABLE: return "unreachable";
    default:
        unreachable();
        return NULL;
    }
}

CFGNode* cfg_node_create(CFGNodeKind kind) {
    CFGNode* node = malloc(sizeof(CFGNode));
    assert(node != NULL);

    node->kind = kind;
    node->next = NULL;
    node->alt = NULL;
    node->visit_flag = false;
    node->is_reachable = false;

    return node;
}

void cfg_node_destroy(CFGNode* node) {
    if (node == NULL) {
        return;
    }

    free(node);
}