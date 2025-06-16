#pragma once

#include "vane/utils/defines.h"

typedef enum CFGNodeKind CFGNodeKind;
typedef struct CFGEdge CFGEdge;
typedef struct CFGNode CFGNode;

enum CFGNodeKind {
    CFG_NODE_UNKNOWN = 0,
    CFG_NODE_FUN_ENTRY,
    CFG_NODE_FUN_EXIT,
    CFG_NODE_BASIC_BLOCK,
    CFG_NODE_CONDITION,
    CFG_NODE_UNREACHABLE,
};

struct CFGNode {
    CFGNodeKind kind;

    CFGNode* next;
    CFGNode* alt;

    bool visit_flag;
    bool is_reachable;
};

const char* get_cfg_node_kind_name(CFGNodeKind kind);

CFGNode* cfg_node_create(CFGNodeKind kind);

void cfg_node_destroy(CFGNode* node);