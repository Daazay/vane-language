#pragma once

#include "vane/utils/defines.h"

#include "vane/cfg/cfg_node.h"

typedef struct CFGVisitor CFGVisitor;

typedef void(*CFGVisitorPreFn)(CFGNode* node, CFGNode* next, CFGNode* alt, void* ctx);
typedef void(*CFGVisitorPostFn)(CFGNode* node, CFGNode* next, CFGNode* alt, void* ctx);

struct CFGVisitor {
    CFGVisitorPreFn pre_fn;
    CFGVisitorPostFn post_fn;
    bool visit_flag;
    void* ctx;
};

void cfg_visit_with(CFGNode* node, const CFGVisitor* visitor);