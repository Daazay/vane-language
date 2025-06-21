#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

#include "vane/diagnostic/source_loc.h"
#include "vane/diagnostic/report_collector.h"

#include "vane/sema/type.h"
#include "vane/sema/type_system.h"

typedef enum SymbolKind SymbolKind;
typedef struct Symbol Symbol;

struct Scope;
struct ASTNode;

enum SymbolKind {
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER,
    SYMBOL_VARIABLE,
    SYMBOL_TYPEALIAS,
    SYMBOL_IMPORT,
};

struct Symbol {
    SymbolKind kind;

    String name;

    struct Scope* scope;
    const struct ASTNode* ast;

    union {
        // IMPORT
        struct Package* target;
        // OTHER
        Type* type;
    };
};

const char* get_symbol_kind_name(SymbolKind kind);

Symbol* symbol_create(SymbolKind kind, String name, const struct ASTNode* ast);

void symbol_destroy(Symbol* symbol);

Type* try_get_type_from_ast(struct Scope* scope, const struct ASTNode* ast, TypeSystem* ts, ReportCollector* rc);

bool symbol_resolve_type(Symbol* symbol, TypeSystem* ts, ReportCollector* rc);