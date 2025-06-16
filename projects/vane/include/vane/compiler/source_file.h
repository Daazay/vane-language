#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/file_utils.h"
#include "vane/utils/vector.h"

#include "vane/scanner/token_stream.h"

#include "vane/ast/ast_node.h"

#include "vane/diagnostic/report_collector.h"

typedef struct SourceFile SourceFile;

struct SourceFile {
    FileContent fc;

    // lexical
    TokenStream ts;

    // syntax
    Vector ast_nodes;

    // semantic
    // ...

    ReportCollector* rc;
    struct Package* parent_package;
};

SourceFile* source_file_load(const String* path, ReportCollector* rc);

void source_file_destroy(SourceFile* source_file);

void source_file_parse(SourceFile* source_file);