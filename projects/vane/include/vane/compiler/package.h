#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/vector.h"
#include "vane/utils/hashmap.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/compiler/source_file.h"

#include "vane/sema/scope.h"

typedef struct Package Package;
struct Compiler;

struct Package {
    const String* path;
    String name;

    // k: [path, String, &string_destroy]
    // v: [file, SourceFile*, &source_file_destroy]
    Hashmap source_files;

    Vector subpackages;

    Scope* scope;
    const Package* parent_package;

    Symbol* entry_point;

    ReportCollector* rc;
};

Package* package_create(const String* path, ReportCollector* rc);

void package_destroy(Package* package);

void package_add_source_file(Package* package, SourceFile* source_file);

void package_add_subpackage(Package* package, Package* subpackage);

bool package_parse_source_files(Package* package);

bool package_resolve_imports(Package* package, struct Compiler* compiler);

bool package_resolve_symbols(Package* package);

bool package_resolve_entry_point(Package* package);