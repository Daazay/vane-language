#include "vane/sema/scope.h"

#include <stdlib.h>

#include "vane/utils/hash.h"
#include "vane/compiler/import_entry.h"

#define SCOPE_DEFAULT_SCOPES_COUNT 4
#define SCOPE_DEFAULT_SYMBOLS_COUNT 4

Scope* scope_create(String name, Scope* parent) {
    Scope* scope = malloc(sizeof(Scope));
    assert(scope != NULL);

    scope->name = name;
    scope->parent = parent;

    scope->scopes = vector_create(
        SCOPE_DEFAULT_SCOPES_COUNT,
        VECTOR_ITEM_SPECS(Scope*, &scope_destroy)
    );
    scope->symbols = hashmap_create(
        SCOPE_DEFAULT_SYMBOLS_COUNT,
        HASHMAP_KEY_SPECS(String, &get_string_hash, &string_eq_str, NULL),
        HASHMAP_VALUE_SPECS(Symbol*, &symbol_destroy)
    );

    if (parent != NULL) {
        vector_push_back(&parent->scopes, &scope);
    }

    return scope;
}

void scope_destroy(Scope* scope) {
    if (scope == NULL) {
        return;
    }

    vector_destroy(&scope->scopes);
    hashmap_destroy(&scope->symbols);
    string_destroy(&scope->name);

    free(scope);
}

void scope_add_symbol(Scope* scope, Symbol* symbol) {
    assert(scope != NULL && symbol != NULL);

    symbol->scope = scope;
    hashmap_put(&scope->symbols, &symbol->name, &symbol);
}

Symbol* scope_lookup_current(const Scope* scope, const String* name) {
    assert(scope != NULL && name != NULL);

    return hashmap_at(&scope->symbols, name);
}

Symbol* scope_lookup(const Scope* scope, const String* name) {
    assert(scope != NULL && name != NULL);

    Symbol* symbol = NULL;

    const Scope* it = scope;
    while (it != NULL) {
        symbol = scope_lookup_current(it, name);

        if (symbol != NULL) {
            break;
        }

        it = it->parent;
    }

    return symbol;
}

bool scope_resolve_import_identifiers(Scope* scope, const Vector* import_entries, ReportCollector* rc) {
    assert(scope != NULL && import_entries != NULL && rc != NULL);

    bool status = true;

    for (u32 i = 0; i < import_entries->size; ++i) {
        ImportEntry* entry = vector_at(import_entries, i);

        Symbol* symbol = scope_lookup_current(scope, &entry->name);
        if (symbol == NULL) {
            symbol = symbol_create(SYMBOL_IMPORT, entry->name,
                (entry->node->as.import_decl.alias != NULL)
                ? entry->node->as.import_decl.alias->loc
                : entry->node->as.import_decl.path->loc
            );
            entry->name = STRING_EMPTY;
            symbol->as.import.target = entry->target;
            scope_add_symbol(scope, symbol);
            continue;
        }

        // Check if imports the same:
        // import "io" as io
        // import "io" as io        // In this case just skip. Maybe warn
        // import "../std/io" as io // If resolved package the same, do as in prevous
        // import "io"              // Same
        // import "io2" as io       // Different package, so error with dupplication error
        if ((u64)entry->target == (u64)symbol->as.import.target) {
            RC_NOTE(rc, "Already imported at line %d", symbol->loc.range.begin.line);
            RC_REPORT_SEMANTIC_WARN(rc, entry->node->loc, "Redundant import of package `%.*s`", (i32)symbol->name.len, symbol->name.text);
            continue;
        }

        RC_NOTE(rc, "Previosly import at %d line %d column", symbol->loc.range.begin.line, symbol->loc.range.begin.column);
        RC_REPORT_SEMANTIC_ERROR(rc, entry->node->loc, "import `%.*s` is already imported", (i32)symbol->name.len, symbol->name.text);
        status = false;
    }

    return status;
}

bool scope_resolve_source_file_identifiers(Scope* scope, const Vector* nodes, ReportCollector* rc) {
    assert(scope != NULL && nodes != NULL && rc != NULL);

    bool status = true;

    for (u32 i = 0; i < nodes->size; ++i) {
        const ASTNode* entity = vector_at(nodes, i);

        // We process this
        if (entity->kind == AST_NODE_IMPORT_DECL) {
            continue;
        }

        Symbol* symbol = NULL;

        switch (entity->kind) {
        case AST_NODE_IMPORT_DECL:
            // this must processed in scope_resolve_import_identifiers
            break;
        case AST_NODE_STMT_TYPEALIAS_DECL:
            symbol = scope_lookup(scope, &entity->as.stmt_typealias_decl.id->as.id.value);
            if (symbol != NULL) {
                RC_NOTE(rc, "`%.*s` was previously defined at %d line and %d column", symbol->name.len, symbol->name.text, symbol->loc.range.begin.line, symbol->loc.range.begin.column);
                RC_REPORT_SEMANTIC_ERROR(rc, entity->as.stmt_typealias_decl.id->loc, "Identifier `%.*s` is already in use", (i32)symbol->name.len, symbol->name.text);
                status = false;
                break;
            }

            symbol = symbol_create(SYMBOL_TYPEALIAS, string_clone(&entity->as.stmt_typealias_decl.id->as.id.value), entity->as.stmt_typealias_decl.id->loc);
            symbol->as.typealias.type = NULL;
            scope_add_symbol(scope->parent, symbol);
            break;
        case AST_NODE_FUN_DECL:
            symbol = scope_lookup(scope, &entity->as.fun_decl.sign->as.fun_sign.id->as.id.value);
            if (symbol != NULL) {
                RC_NOTE(rc, "`%.*s` was previously defined at %d line and %d column", symbol->name.len, symbol->name.text, symbol->loc.range.begin.line, symbol->loc.range.begin.column);
                RC_REPORT_SEMANTIC_ERROR(rc, entity->as.fun_decl.sign->as.fun_sign.id->loc, "Identifier `%.*s` is already in use", (i32)symbol->name.len, symbol->name.text);
                status = false;
                break;
            }

            symbol = symbol_create(SYMBOL_FUNCTION, string_clone(&entity->as.fun_decl.sign->as.fun_sign.id->as.id.value), entity->as.fun_decl.sign->as.fun_sign.id->loc);
            symbol->as.function.type = NULL;
            scope_add_symbol(scope->parent, symbol);

            if (!scope_resolve_function_identifiers(scope, entity, rc)) {
                status = false;
            }
            break;
        default:
            unreachable();
            break;
        }
    }

    return status;
}

bool scope_resolve_function_identifiers(Scope* scope, const ASTNode* fun_decl, ReportCollector* rc) {
    assert(scope != NULL && fun_decl != NULL && rc != NULL);
    assert(fun_decl->kind == AST_NODE_FUN_DECL);

    bool status = true;

    Scope* func_scope = scope_create(string_clone(&fun_decl->as.fun_decl.sign->as.fun_sign.id->as.id.value), scope);

    // Add fun parameters

    for (u32 i = 0; i < fun_decl->as.fun_decl.sign->as.fun_sign.params.size; ++i) {
        const ASTNode* param = vector_at(&fun_decl->as.fun_decl.sign->as.fun_sign.params, i);

        Symbol* symbol = scope_lookup_current(func_scope, &param->as.fun_param.id->as.id.value);
        if (symbol != NULL) {
            RC_NOTE(rc, "`%.*s` was previously defined at %d line and %d column", symbol->name.len, symbol->name.text, symbol->loc.range.begin.line, symbol->loc.range.begin.column);
            RC_REPORT_SEMANTIC_ERROR(rc, param->as.fun_param.id->loc, "Identifier `%.*s` is already in use", (i32)symbol->name.len, symbol->name.text);
            status = false;
            continue;
        }

        symbol = symbol_create(SYMBOL_PARAMETER, string_clone(&param->as.fun_param.id->as.id.value), param->as.fun_param.id->loc);
        symbol->as.parameter.type = NULL;
        scope_add_symbol(func_scope, symbol);
    }

    if (!scope_resolve_stmt_block_identifiers(func_scope, &fun_decl->as.fun_decl.block, rc)) {
        status = false;
    }

    return status;
}

bool scope_resolve_var_decl_identifiers(Scope* scope, const ASTNode* var_decl, ReportCollector* rc) {
    assert(scope != NULL && var_decl != NULL && rc != NULL);
    assert(var_decl->kind == AST_NODE_STMT_VAR_DECL);

    bool status = true;

    for (u32 j = 0; j < var_decl->as.stmt_var_decl.items.size; ++j) {
        const ASTNode* var_item = vector_at(&var_decl->as.stmt_var_decl.items, j);

        Symbol* symbol = scope_lookup_current(scope, &var_item->as.stmt_var_item.id->as.id.value);
        if (symbol != NULL) {
            RC_NOTE(rc, "`%.*s` was previously defined at %d line and %d column", symbol->name.len, symbol->name.text, symbol->loc.range.begin.line, symbol->loc.range.begin.column);
            RC_REPORT_SEMANTIC_ERROR(rc, var_item->as.stmt_var_item.id->loc, "Identifier `%.*s` is already in use", (i32)symbol->name.len, symbol->name.text);
            status = false;
            continue;
        }

        symbol = symbol_create(SYMBOL_VARIABLE, string_clone(&var_item->as.stmt_var_item.id->as.id.value), var_item->as.stmt_var_item.id->loc);
        symbol->as.variable.type = NULL;
        scope_add_symbol(scope, symbol);
    }

    return status;
}

bool scope_resolve_stmt_block_identifiers(Scope* scope, const Vector* nodes, ReportCollector* rc) {
    assert(scope != NULL && nodes != NULL && rc != NULL);

    bool status = true;

    for (u32 i = 0; i < nodes->size; ++i) {
        const ASTNode* stmt = vector_at(nodes, i);

        Symbol* symbol = NULL;

        switch (stmt->kind) {
        case AST_NODE_STMT_TYPEALIAS_DECL:
            symbol = scope_lookup_current(scope, &stmt->as.stmt_typealias_decl.id->as.id.value);
            if (symbol != NULL) {
                RC_NOTE(rc, "`%.*s` was previously defined at %d line and %d column", symbol->name.len, symbol->name.text, symbol->loc.range.begin.line, symbol->loc.range.begin.column);
                RC_REPORT_SEMANTIC_ERROR(rc, stmt->as.stmt_typealias_decl.id->loc, "Identifier `%.*s` is already in use", (i32)symbol->name.len, symbol->name.text);
                status = false;
                break;
            }

            symbol = symbol_create(SYMBOL_TYPEALIAS, string_clone(&stmt->as.stmt_typealias_decl.id->as.id.value), stmt->as.stmt_typealias_decl.id->loc);
            symbol->as.typealias.type = NULL;
            scope_add_symbol(scope, symbol);
            break;
        case AST_NODE_STMT_BLOCK:
            Scope* block_scope = scope_create(string_from_format("scope[%d]", i), scope);
            if (!scope_resolve_stmt_block_identifiers(block_scope, &stmt->as.stmt_block.block, rc)) {
                status = false;
            }
            break;
        case AST_NODE_STMT_VAR_DECL:
            if (!scope_resolve_var_decl_identifiers(scope, stmt, rc)) {
                status = false;
            }
            break;
        case AST_NODE_STMT_CONDITION:
            for (u32 j = 0; j < stmt->as.stmt_condition.branches.size; ++j) {
                const  ASTNode* br = vector_at(&stmt->as.stmt_condition.branches, j);

                Scope* br_scope = scope_create(string_from_format("cond[%d]branch[%d]", i, j), scope);
                if (!scope_resolve_stmt_block_identifiers(br_scope, &br->as.stmt_branch.block, rc)) {
                    status = false;
                }
            }
            break;
        case AST_NODE_STMT_WHILE:
            Scope* while_scope = scope_create(string_from_format("while[%d]", i), scope);
            if (!scope_resolve_stmt_block_identifiers(while_scope, &stmt->as.stmt_while.block, rc)) {
                status = false;
            }
            break;
        case AST_NODE_STMT_DO:
            Scope* do_scope = scope_create(string_from_format("while[%d]", i), scope);
            if (!scope_resolve_stmt_block_identifiers(do_scope, &stmt->as.stmt_while.block, rc)) {
                status = false;
            }
            break;
        default:
            break;
        }
    }

    return status;
}