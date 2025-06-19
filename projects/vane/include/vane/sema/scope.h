#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"
#include "vane/utils/hashmap.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/ast/ast_node.h"

#include "vane/sema/symbol.h"

typedef struct Scope Scope;

struct Scope {
    String name;
    Scope* parent;

    Hashmap symbols;
    Vector scopes;
};

Scope* scope_create(String name, Scope* parent);

void scope_destroy(Scope* scope);

void scope_add_symbol(Scope* scope, Symbol* symbol);

Symbol* scope_lookup_current(const Scope* scope, const String* name);

Symbol* scope_lookup(const Scope* scope, const String* name);

//

bool scope_resolve_import_identifiers(Scope* scope, const Vector* import_entries, ReportCollector* rc);

bool scope_resolve_source_file_identifiers(Scope* scope, const Vector* nodes, ReportCollector* rc);

bool scope_resolve_function_identifiers(Scope* scope, const ASTNode* fun_decl, ReportCollector* rc);

bool scope_resolve_var_decl_identifiers(Scope* scope, const ASTNode* var_decl, ReportCollector* rc);

bool scope_resolve_stmt_block_identifiers(Scope* scope, const Vector* nodes, ReportCollector* rc);