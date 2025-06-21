#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/hashmap.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/compiler/package.h"
#include "vane/compiler/build_options.h"

#include "vane/sema/type_system.h"

typedef struct Compiler Compiler;

struct Compiler {
    BuildOptions* build_options;

    // k: [dirpath, String, &string_destroy]
    // v: [package, Package*, &package_destroy]
    Hashmap packages;

    TypeSystem type_system;

    ReportCollector rc;
};

Compiler compiler_create(BuildOptions* build_options);

void compiler_destroy(Compiler* compiler);

const String* compiler_get_collection_path(Compiler* compiler, const String* collection_name);

Package* compiler_load_package(Compiler* compiler, const String* dirpath);

Package* compiler_try_load_imported_package(Compiler* compiler, const String* collection_name, const String* package_path);

bool compiler_parse_source_files(Compiler* compiler);

bool compiler_resolve_imports(Compiler* compiler);

bool compiler_print_ast(Compiler* compiler);

bool compiler_show_imports(Compiler* compiler);

bool compiler_resolve_symbols(Compiler* compiler);

bool compiler_resolve_types(Compiler* compiler);