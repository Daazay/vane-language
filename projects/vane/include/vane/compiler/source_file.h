#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/hashmap.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/ast/ast_node.h"

#include "vane/sema/scope.h"

typedef struct SourceFile SourceFile;
struct Compiler;

struct SourceFile {
    const String* path;
    String content;

    ASTNode* ast;

    // ImportEntry
    Vector imports;

    Scope* scope;
    struct Package* package;

    ReportCollector* rc;
};

SourceFile* source_file_load(const String* path, ReportCollector* rc);

void source_file_destroy(SourceFile* source_file);

bool source_file_parse(SourceFile* source_file);

bool source_file_resolve_imports(SourceFile* source_file, struct Compiler* compiler);

bool source_file_resolve_identifiers(SourceFile* source_file);