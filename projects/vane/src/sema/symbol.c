#include "vane/sema/symbol.h"

#include <stdlib.h>

#include "vane/sema/scope.h"
#include "vane/ast/ast_node.h"

const char* get_symbol_kind_name(SymbolKind kind) {
    switch (kind) {
    case SYMBOL_FUNCTION:  return "function";
    case SYMBOL_VARIABLE:  return "variable";
    case SYMBOL_PARAMETER: return "parameter";
    case SYMBOL_TYPEALIAS: return "type alias";
    case SYMBOL_IMPORT:    return "import alias";
    default:
        unreachable();
        return NULL;
    }
}

Symbol* symbol_create(SymbolKind kind, String name, const ASTNode* ast) {
    Symbol* symbol = malloc(sizeof(Symbol));
    assert(symbol != NULL);

    symbol->kind = kind;
    symbol->name = name;
    symbol->ast = ast;

    return symbol;
}

void symbol_destroy(Symbol* symbol) {
    if (symbol == NULL) {
        return;
    }

    string_destroy(&symbol->name);

    free(symbol);
}

static Type* get_type_pointer_or_create(TypeSystem* ts, Type* to) {
    assert(ts != NULL && to != NULL);

    Type tmp = {
        .kind = TYPE_POINTER,
        .as.pointer.to = to,
    };
    Type* target_type = &tmp;

    Type* existing = (Type*)hashset_at(&ts->pointer_types, &target_type);
    if (existing != NULL) {
        return existing;
    }

    Type* type = type_pointer_create(to);
    hashset_put(&ts->pointer_types, &type);
    return type;
}

static Type* get_type_alias_or_create(TypeSystem* ts, Type* to) {
    assert(ts != NULL && to != NULL);

    Type tmp = {
        .kind = TYPE_ALIAS,
        .as.pointer.to = to,
    };
    Type* target_type = &tmp;

    Type* existing = (Type*)hashset_at(&ts->alias_types, &target_type);
    if (existing != NULL) {
        return existing;
    }

    Type* type = type_alias_create(to);
    hashset_put(&ts->alias_types, &type);
    return type;
}

static Type* get_type_array_or_create(TypeSystem* ts, const ASTNode* size_expr, Type* element_type) {
    assert(ts != NULL && size_expr != NULL && element_type != NULL);

    // TODO: evaluate expr from ast
    u32 length = 0;

    Type tmp = {
        .kind = TYPE_ARRAY,
        .as.array.element_type = element_type,
        .as.array.length = length,
    };

    Type* target_type = &tmp;

    Type* existing = (Type*)hashset_at(&ts->array_types, &target_type);
    if (existing != NULL) {
        return existing;
    }

    Type* type = type_array_create(element_type, length);
    hashset_put(&ts->array_types, &type);
    return type;
}

static Type* get_type_slice_or_create(TypeSystem* ts, Type* element_type) {
    assert(ts != NULL && element_type != NULL);

    Type tmp = {
        .kind = TYPE_SLICE,
        .as.slice.element_type = element_type,
    };

    Type* target_type = &tmp;

    Type* existing = (Type*)hashset_at(&ts->slice_types, &target_type);
    if (existing != NULL) {
        return existing;
    }

    Type* type = type_slice_create(ts, element_type);
    hashset_put(&ts->slice_types, &type);
    return type;
}

static Type* get_type_function_or_create(TypeSystem* ts, Type** param_types, u32 params_count, Type* return_type) {
    assert(ts != NULL && (param_types != NULL || params_count == 0) && return_type != NULL);

    Type tmp = {
        .kind = TYPE_FUNCTION,
        .as.function.param_types = param_types,
        .as.function.params_count = params_count,
        .as.function.return_type = return_type,
    };

    Type* target_type = &tmp;

    Type* existing = (Type*)hashset_at(&ts->function_types, &target_type);
    if (existing != NULL) {
        free(param_types);
        return existing;
    }

    Type* type = type_function_create(param_types, params_count, return_type);
    hashset_put(&ts->function_types, &type);
    return type;
}

Type* try_get_type_from_ast(Scope* scope, const ASTNode* ast, TypeSystem* ts, ReportCollector* rc) {
    assert(scope != NULL && ts != NULL && rc != NULL);

    if (ast == NULL) {
        return &ts->builtin_types[TYPE_BUILTIN_ANY];
    }

    switch (ast->kind) {
    case AST_NODE_TYPEREF_BUILTIN: {
        TypeBuiltinKind kind = get_type_builtin_kind_from_token_kind(ast->as.typeref_builtin.kind);
        return &ts->builtin_types[kind];
    } break;
    case AST_NODE_TYPEREF_CUSTOM: {
        const String* name = &ast->as.typeref_custom.value;
        Symbol* symbol = scope_lookup_kind(scope, name, SYMBOL_TYPEALIAS);

        if (symbol == NULL) {
            RC_TRACE(rc, ast->loc, "Unknown typeref `%.*s`", (i32)name->len, name->text);
            return &ts->unresolved_type;
        }

        if (symbol->type == NULL) {
            symbol_resolve_type(symbol, ts, rc);
        }

        return get_type_alias_or_create(ts, symbol->type);
    } break;
    case AST_NODE_TYPEREF_PTR: {
        Type* to = try_get_type_from_ast(scope, ast->as.typeref_ptr.typeref, ts, rc);

        if (to->kind == TYPE_UNRESOLVED) {
            return to;
        }

        return get_type_pointer_or_create(ts, to);
    } break;
    case AST_NODE_TYPEREF_ARR: {
        Type* element_type = try_get_type_from_ast(scope, ast->as.typeref_arr.typeref, ts, rc);

        if (element_type->kind == TYPE_UNRESOLVED) {
            return element_type;
        }

        if (ast->as.typeref_arr.size_expr != NULL) {
            RC_REPORT_SEMANTIC_WARN(rc, ast->as.typeref_arr.size_expr->loc, "TODO: implement ast expr evaluator.");
            return get_type_array_or_create(ts, ast->as.typeref_arr.size_expr, element_type);
        }

        return get_type_slice_or_create(ts, element_type);
    } break;
    case AST_NODE_TYPEREF_FUN: {
        const Vector* params = &ast->as.fun_sign.params;

        Type** param_types = NULL;
        u32 params_count = 0;

        if (params->size > 0) {
            params_count = params->size;

            param_types = malloc(params_count * sizeof(Type*));
            assert(param_types != NULL);

            for (u32 i = 0; i < params->size; ++i) {
                const ASTNode* param = vector_at(params, i);

                param_types[i] = try_get_type_from_ast(scope, param->as.fun_param.typeref, ts, rc);
            }
        }

        Type* return_type = try_get_type_from_ast(scope, ast->as.fun_sign.typeref, ts, rc);

        return get_type_function_or_create(ts, param_types, params_count, return_type);
    } break;
    default:
        unreachable();
        return NULL;
    }
}

bool symbol_resolve_type(Symbol* symbol, TypeSystem* ts, ReportCollector* rc) {
    assert(symbol != NULL && ts != NULL && rc != NULL);
    assert(symbol->kind != SYMBOL_IMPORT);

    if (symbol->type != NULL) {
        return true;
    }

    bool status = true;

    const ASTNode* ast = symbol->ast;
    Scope* scope = symbol->scope;

    switch (symbol->kind) {
    case SYMBOL_FUNCTION: {
        const Vector* params = &ast->as.fun_sign.params;

        Type** param_types = NULL;
        u32 params_count = 0;

        if (params->size > 0) {
            params_count = params->size;

            param_types = malloc(params_count * sizeof(Type*));
            assert(param_types != NULL);

            for (u32 i = 0; i < params->size; ++i) {
                const ASTNode* param = vector_at(params, i);

                param_types[i] = try_get_type_from_ast(scope, param->as.fun_param.typeref, ts, rc);

                if (param_types[i]->kind == TYPE_UNRESOLVED) {
                    RC_REPORT_SEMANTIC_ERROR(rc, ast->as.fun_sign.id->loc,
                        "Failed to resolve type for %s `%.*s`",
                        get_symbol_kind_name(symbol->kind),
                        (i32)symbol->name.len, symbol->name.text
                    );
                    status = false;
                }
            }
        }

        Type* return_type = try_get_type_from_ast(scope, ast->as.fun_sign.typeref, ts, rc);

        if (return_type->kind == TYPE_UNRESOLVED) {
            RC_REPORT_SEMANTIC_ERROR(rc, ast->as.fun_sign.id->loc,
                "Failed to resolve type for %s `%.*s`",
                get_symbol_kind_name(symbol->kind),
                (i32)symbol->name.len, symbol->name.text
            );
            status = false;
        }

        symbol->type = get_type_function_or_create(ts, param_types, params_count, return_type);
    } break;
    case SYMBOL_PARAMETER: {
        Type* type = try_get_type_from_ast(scope, ast->as.fun_param.typeref, ts, rc);

        if (type->kind == TYPE_UNRESOLVED) {
            RC_REPORT_SEMANTIC_ERROR(rc, ast->as.fun_param.id->loc,
                "Failed to resolve type for %s `%.*s`",
                get_symbol_kind_name(symbol->kind),
                (i32)symbol->name.len, symbol->name.text
            );
            status = false;
        }

        symbol->type = type;
    } break;
    case SYMBOL_VARIABLE: {
        Type* type = try_get_type_from_ast(scope, ast->as.stmt_var_item.typeref, ts, rc);

        if (type->kind == TYPE_UNRESOLVED) {
            RC_REPORT_SEMANTIC_ERROR(rc, ast->as.stmt_var_item.id->loc,
                "Failed to resolve type for %s `%.*s`",
                get_symbol_kind_name(symbol->kind),
                (i32)symbol->name.len, symbol->name.text
            );
            status = false;
        }

        symbol->type = type;
    } break;
    case SYMBOL_TYPEALIAS: {
        Type* type = try_get_type_from_ast(scope, ast->as.stmt_typealias_decl.typeref, ts, rc);

        if (type->kind == TYPE_UNRESOLVED) {
            RC_REPORT_SEMANTIC_ERROR(rc, ast->as.stmt_typealias_decl.id->loc,
                "Failed to resolve type for %s `%.*s`",
                get_symbol_kind_name(symbol->kind),
                (i32)symbol->name.len, symbol->name.text
            );
            status = false;
        }

        symbol->type = type;
    } break;
    default:
        unreachable();
        return false;
    }

    return status;
}