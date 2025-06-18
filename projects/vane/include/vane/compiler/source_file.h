#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/hashmap.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/ast/ast_node.h"

typedef struct SourceFile SourceFile;

struct SourceFile {
    const String* path;
    String content;

    Vector ast_nodes;
    Hashmap imports;

    struct Package* package;

    ReportCollector* rc;
};

SourceFile* source_file_create(const String* path, String content, ReportCollector* rc);

void source_file_destroy(SourceFile* source_file);

bool source_file_parse(SourceFile* source_file);