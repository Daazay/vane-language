#include "vane/cfg/cfg_builder.h"

#include <stdio.h>

void cfg_result_destroy(CFGResult* result) {
    if (result == NULL) {
        return;
    }

    vector_destroy(&result->nodes);
    result->entry = NULL;
}

CFGBuilder cfg_builder_create() {
    return (CFGBuilder) {
        .nodes = vector_create(4, VECTOR_ITEM_SPECS(CFGNode*, &cfg_node_destroy)),
        .scopes = vector_create(4, VECTOR_ITEM_SPECS(CFGScope, NULL)),
        .curr_scope = NULL,
        .curr_node = NULL,
        .exit_node = NULL,
        .is_reachable = true,
    };
}

void cfg_builder_destroy(CFGBuilder* builder) {
    if (builder == NULL) {
        return;
    }

    vector_destroy(&builder->scopes);
}

void cfg_builder_push_scope(CFGBuilder* builder, CFGNode* break_node, CFGNode* continue_node) {
    assert(builder != NULL);

    CFGScope scope = cfg_scope_create(break_node, continue_node);
    vector_push_back(&builder->scopes, &scope);

    builder->curr_scope = vector_at_back(&builder->scopes);
}

void cfg_builder_pop_scope(CFGBuilder* builder) {
    assert(builder != NULL);

    vector_remove(&builder->scopes, builder->scopes.size - 1);
    builder->curr_scope = vector_at_back(&builder->scopes);
    builder->is_reachable = true;
}

CFGNode* cfg_builder_create_node(CFGBuilder* builder, CFGNodeKind kind) {
    assert(builder != NULL);

    CFGNode* node = cfg_node_create(kind);
    vector_push_back(&builder->nodes, &node);
    node->is_reachable = builder->is_reachable;

    return node;
}

//

CFGResult build_cfg_for_fun_decl(const ASTNode* ast) {
    assert(ast != NULL);
    assert(ast->kind == AST_NODE_FUN_DECL);

    CFGBuilder builder = cfg_builder_create();

    CFGNode* entry_node = cfg_builder_create_node(&builder, CFG_NODE_FUN_ENTRY);
    CFGNode* exit_node = cfg_builder_create_node(&builder, CFG_NODE_FUN_EXIT);

    cfg_builder_push_scope(&builder, NULL, NULL);

    builder.curr_node = entry_node;
    builder.exit_node = exit_node;

    cfg_builder_prepare_stmt_block(&builder, &ast->as.fun_decl.block);

    builder.curr_node->next = exit_node;

    CFGResult result = {
        .nodes = builder.nodes,
        .entry = entry_node,
    };

    cfg_builder_destroy(&builder);

    return result;
}

void cfg_builder_prepare_stmt_block(CFGBuilder* builder, const Vector* stmt_block) {
    assert(builder != NULL && stmt_block != NULL);

    for (u32 i = 0; i < stmt_block->size; ++i) {
        const ASTNode* stmt = vector_at(stmt_block, i);
        cfg_builder_prepare_stmt(builder, stmt);
    }
}

void cfg_builder_prepare_stmt(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL);

    if (!builder->is_reachable) {
        printf("unrechable code at %d:%d\n", (i32)ast->loc.begin.line, (i32)ast->loc.begin.column);
        return;
    }

    switch (ast->kind) {
    case AST_NODE_STMT_EMPTY: break;
    case AST_NODE_STMT_VAR_DECL:
        cfg_builder_prepare_var_decl(builder, ast);
        break;
    case AST_NODE_STMT_EXPR:
        cfg_builder_prepare_expr(builder, ast);
        break;
    case AST_NODE_STMT_CONDITION:
        cfg_builder_prepare_condition(builder, ast);
        break;
    case AST_NODE_STMT_WHILE:
        cfg_builder_prepare_while_loop(builder, ast);
        break;
    case AST_NODE_STMT_DO:
        cfg_builder_prepare_do_loop(builder, ast);
        break;
    case AST_NODE_STMT_BREAK:
        cfg_builder_handle_break(builder, ast);
        break;
    case AST_NODE_STMT_CONTINUE:
        cfg_builder_handle_continue(builder, ast);
        break;
    case AST_NODE_STMT_RETURN:
        cfg_builder_handle_return(builder, ast);
        break;
    default:
        unreachable();
        break;
    }
}

void cfg_builder_prepare_var_decl(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_VAR_DECL);

    if (builder->curr_node->kind != CFG_NODE_BASIC_BLOCK) {
        CFGNode* node = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);
        builder->curr_node->next = node;
        builder->curr_node = node;
    }
}

void cfg_builder_prepare_expr(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_EXPR);

    if (builder->curr_node->kind != CFG_NODE_BASIC_BLOCK) {
        CFGNode* node = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);
        builder->curr_node->next = node;
        builder->curr_node = node;
    }
}

void cfg_builder_prepare_block(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_BLOCK);

    CFGNode* block_node = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);
    builder->curr_node->next = block_node;
    builder->curr_node = block_node;

    cfg_builder_push_scope(builder, builder->curr_scope->break_node, builder->curr_scope->continue_node);

    cfg_builder_prepare_stmt_block(builder, &ast->as.stmt_block.block);

    cfg_builder_pop_scope(builder);
}

void cfg_builder_prepare_condition(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_CONDITION);
    assert(ast->as.stmt_condition.branches.size > 0);

    CFGNode* merge_node = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);
    CFGNode* last_cond = NULL;

    bool has_else = false;

    for (u32 i = 0; i < ast->as.stmt_condition.branches.size; ++i) {
        const ASTNode* br = vector_at(&ast->as.stmt_condition.branches, i);
        const bool is_conditional = (br->as.stmt_branch.expr != NULL);

        cfg_builder_push_scope(builder, builder->curr_scope->break_node, builder->curr_scope->continue_node);

        // IF /IF-ELSE BRANCHES
        if (is_conditional) {
            CFGNode* cond_node = cfg_builder_create_node(builder, CFG_NODE_CONDITION);
            CFGNode* block_node = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);

            if (last_cond == NULL) {
                builder->curr_node->next = cond_node;
            }
            else {
                last_cond->alt = cond_node;
            }

            last_cond = cond_node;
            cond_node->next = block_node;

            builder->curr_node = block_node;
            cfg_builder_prepare_stmt_block(builder, &br->as.stmt_branch.block);

            if (builder->is_reachable) {
                builder->curr_node->next = merge_node;
            }
        }
        // ELSE BRANCH
        else {
            has_else = true;
            CFGNode* else_node = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);

            last_cond->alt = else_node;

            builder->curr_node = else_node;
            cfg_builder_prepare_stmt_block(builder, &br->as.stmt_branch.block);

            if (builder->is_reachable) {
                builder->curr_node->next = merge_node;
            }
        }

        cfg_builder_pop_scope(builder);
    }

    if (!has_else) {
        last_cond->alt = merge_node;
    }

    builder->curr_node = merge_node;
}

void cfg_builder_prepare_while_loop(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_WHILE);

    CFGNode* cond_node = cfg_builder_create_node(builder, CFG_NODE_CONDITION);
    CFGNode* loop_body_start = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);
    CFGNode* merge_node = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);

    builder->curr_node->next = cond_node;

    cond_node->next = loop_body_start;
    cond_node->alt = merge_node;

    cfg_builder_push_scope(builder, merge_node, cond_node);

    builder->curr_node = loop_body_start;
    cfg_builder_prepare_stmt_block(builder, &ast->as.stmt_while.block);

    if (builder->is_reachable) {
        builder->curr_node->next = cond_node;
    }

    cfg_builder_pop_scope(builder);

    builder->curr_node = merge_node;
}

void cfg_builder_prepare_do_loop(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_DO);

    CFGNode* cond_node = cfg_builder_create_node(builder, CFG_NODE_CONDITION);
    CFGNode* loop_body_start = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);
    CFGNode* merge_node = cfg_builder_create_node(builder, CFG_NODE_BASIC_BLOCK);

    builder->curr_node->next = loop_body_start;

    cond_node->next = loop_body_start;
    cond_node->alt = merge_node;

    cfg_builder_push_scope(builder, merge_node, cond_node);

    builder->curr_node = loop_body_start;
    cfg_builder_prepare_stmt_block(builder, &ast->as.stmt_while.block);

    if (builder->is_reachable) {
        builder->curr_node->next = cond_node;
    }

    cfg_builder_pop_scope(builder);

    builder->curr_node = merge_node;
}

void cfg_builder_handle_break(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_BREAK);

    builder->curr_node->next = builder->curr_scope->break_node;
    builder->is_reachable = false;
}

void cfg_builder_handle_continue(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_CONTINUE);

    builder->curr_node->next = builder->curr_scope->continue_node;
    builder->is_reachable = false;
}

void cfg_builder_handle_return(CFGBuilder* builder, const ASTNode* ast) {
    assert(builder != NULL && ast != NULL);
    assert(ast->kind == AST_NODE_STMT_RETURN);

    const ASTNode* expr = ast->as.stmt_return.expr;
    if (expr != NULL) {
        // append stmt to current node
    }

    builder->curr_node->next = builder->exit_node;
    builder->is_reachable = false;
}