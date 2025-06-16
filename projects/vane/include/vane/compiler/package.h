#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/vector.h"
#include "vane/utils/hashmap.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/compiler/source_file.h"

typedef struct Package Package;

struct Package {
    const String* dirpath;
    String name;


    // key: [path, String, &string_destroy]
    // value: [file, SourceFile*, &package_destroy]
    Hashmap source_files;

    // const Package* pkg
    Vector subpackages;

    Package* parent_package;

    ReportCollector* rc;
};

Package* package_create(const String* dirpath, ReportCollector* rc);

void package_destroy(Package* package);

void package_add_subpackage(Package* package, Package* subpackage);