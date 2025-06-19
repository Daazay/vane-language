#include "vane/sema/symbol.h"

#include <stdlib.h>

Symbol* symbol_create(SymbolKind kind, String name, SourceLoc loc) {
    Symbol* symbol = malloc(sizeof(Symbol));
    assert(symbol != NULL);

    symbol->kind = kind;
    symbol->name = name;
    symbol->loc = loc;

    return symbol;
}

void symbol_destroy(Symbol* symbol) {
    if (symbol == NULL) {
        return;
    }

    string_destroy(&symbol->name);

    free(symbol);
}