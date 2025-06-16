#include "vane/cfg/visitors/cfg_visitor.h"

void cfg_visit_with(CFGNode* node, const CFGVisitor* visitor) {
    if (node == NULL) {
        return;
    }
    if (node->visit_flag == visitor->visit_flag) {
        return;
    }
    node->visit_flag = visitor->visit_flag;

    if (visitor->pre_fn != NULL) {
        visitor->pre_fn(node, node->next, node->alt, visitor->ctx);
    }

    cfg_visit_with(node->next, visitor);
    cfg_visit_with(node->alt, visitor);

    if (visitor->post_fn != NULL) {
        visitor->post_fn(node, node->next, node->alt, visitor->ctx);
    }
}