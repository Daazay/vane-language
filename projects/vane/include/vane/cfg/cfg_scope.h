#pragma once

#include "vane/utils/defines.h"

#include "vane/cfg/cfg_node.h"

typedef struct CFGScope CFGScope;

struct CFGScope {
    CFGNode* break_node;
    CFGNode* continue_node;
};

CFGScope cfg_scope_create(CFGNode* break_node, CFGNode* continue_node);