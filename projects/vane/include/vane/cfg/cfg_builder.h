#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"

#include "vane/ast/ast_node.h"

#include "vane/cfg/cfg_node.h"
#include "vane/cfg/cfg_scope.h"

typedef struct CFGResult CFGResult;
typedef struct CFGBuilder CFGBuilder;

struct CFGResult {
    Vector nodes;
    CFGNode* entry;
};

struct CFGBuilder {
    Vector nodes;
    Vector scopes;

    CFGScope* curr_scope;

    CFGNode* curr_node;
    CFGNode* exit_node;

    bool is_reachable;
};

void cfg_result_destroy(CFGResult* result);

CFGBuilder cfg_builder_create();

void cfg_builder_destroy(CFGBuilder* builder);

void cfg_builder_push_scope(CFGBuilder* builder, CFGNode* break_node, CFGNode* continue_node);

void cfg_builder_pop_scope(CFGBuilder* builder);

CFGNode* cfg_builder_create_node(CFGBuilder* builder, CFGNodeKind kind);

//

CFGResult build_cfg_for_fun_decl(const ASTNode* ast);

void cfg_builder_prepare_stmt_block(CFGBuilder* builder, const Vector* stmt_block);

void cfg_builder_prepare_stmt(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_prepare_var_decl(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_prepare_expr(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_prepare_block(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_prepare_condition(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_prepare_while_loop(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_prepare_do_loop(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_handle_break(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_handle_continue(CFGBuilder* builder, const ASTNode* ast);

void cfg_builder_handle_return(CFGBuilder* builder, const ASTNode* ast);