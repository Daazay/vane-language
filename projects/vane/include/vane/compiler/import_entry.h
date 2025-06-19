#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

typedef struct ImportEntry ImportEntry;

struct ImportEntry {
    String name;
    const struct ASTNode* node;
    struct Package* target;
};

ImportEntry import_entry_create(const struct ASTNode* node);

void import_entry_destroy(ImportEntry* entry);