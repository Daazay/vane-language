#include "vane/compiler/import_entry.h"

#include "vane/ast/ast_node.h"

ImportEntry import_entry_create(const ASTNode* node) {
    return (ImportEntry) {
        .name = STRING_EMPTY,
        .node = node,
        .target = NULL,
    };
}

void import_entry_destroy(ImportEntry* entry) {
    if (entry == NULL) {
        return;
    }

    string_destroy(&entry->name);
}