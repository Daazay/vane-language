#include "vane/cfg/cfg_scope.h"

#include <stdlib.h>

CFGScope cfg_scope_create(CFGNode* break_node, CFGNode* continue_node) {
    return (CFGScope) {
        .break_node = break_node,
        .continue_node = continue_node,
    };
}