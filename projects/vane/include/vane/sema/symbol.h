#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

#include "vane/diagnostic/source_loc.h"

typedef enum SymbolKind SymbolKind;
typedef struct Symbol Symbol;

enum SymbolKind {
    SYMBOL_IMPORT,
    SYMBOL_TYPEALIAS,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER,
    SYMBOL_VARIABLE,
};

struct Symbol {
    SymbolKind kind;

    String name;
    SourceLoc loc;

    struct Scope* scope;

    union {
        struct {
            struct Package* target;
        } import;
        struct {
            struct Type* type;
        } typealias;
        struct {
            struct Type* type;
        } function;
        struct {
            struct Type* type;
        } parameter;
        struct {
            struct Type* type;
        } variable;
    } as;
};

Symbol* symbol_create(SymbolKind kind, String name, SourceLoc loc);

void symbol_destroy(Symbol* symbol);