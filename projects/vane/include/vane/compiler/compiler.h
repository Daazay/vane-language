#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/hashmap.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/compiler/build_options.h"
#include "vane/compiler/source_file.h"
#include "vane/compiler/package.h"

typedef struct Compiler Compiler;

struct Compiler {
    BuildOptions* build_options;

    // key: [dirpath, String, &string_destroy]
    // value: [package, Package*, &package_destroy]
    Hashmap packages;

    ReportCollector rc;
};

Compiler compiler_create(BuildOptions* build_options);

void compiler_destroy(Compiler* compiler);

Package* compiler_load_package(Compiler* compiler, const String* dirpath);