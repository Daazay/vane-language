#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"
#include "vane/utils/hashmap.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/ast/ast_node.h"

#include "vane/sema/symbol.h"
#include "vane/sema/type_system.h"

typedef enum ScopeKind ScopeKind;
typedef struct Scope Scope;

enum ScopeKind {
    SCOPE_UNKNOWN = 0,
    SCOPE_PACKAGE,
    SCOPE_SOURCE_FILE,
    SCOPE_FUNCTION,
    SCOPE_BASIC,
};

struct Scope {
    ScopeKind kind;
    Scope* parent;

    Hashmap symbols;
    Vector scopes;
};

const char* get_scope_kind_name(ScopeKind kind);

Scope* scope_create(ScopeKind kind, Scope* parent);

void scope_destroy(Scope* scope);

void scope_add_symbol(Scope* scope, Symbol* symbol);

Symbol* scope_lookup_current(const Scope* scope, const String* name);

Symbol* scope_lookup_current_kind(const Scope* scope, const String* name, SymbolKind kind);

Symbol* scope_lookup(const Scope* scope, const String* name);

Symbol* scope_lookup_kind(const Scope* scope, const String* name, SymbolKind kind);

//

bool scope_resolve_import_symbols(Scope* scope, const Vector* import_entries, ReportCollector* rc);

bool scope_resolve_source_file_symbols(Scope* scope, const Vector* nodes, ReportCollector* rc);

bool scope_resolve_function_symbols(Scope* scope, const ASTNode* fun_decl, ReportCollector* rc);

bool scope_resolve_var_decl_symbols(Scope* scope, const ASTNode* var_decl, ReportCollector* rc);

bool scope_resolve_stmt_block_symbols(Scope* scope, const Vector* nodes, ReportCollector* rc);

bool scope_resolve_types(const Scope* scope, TypeSystem* ts, ReportCollector* rc);