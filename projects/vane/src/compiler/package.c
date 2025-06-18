#include "vane/compiler/package.h"

#include <stdlib.h>

#include "vane/utils/path.h"
#include "vane/utils/hash.h"

#define PACKAGE_DEFAULT_SOURCE_FILE_COUNT 8
#define PACKAGE_DEFAULT_SUBPACKAGE_COUNT 8

Package* package_create(const String* path, ReportCollector* rc) {
    assert(path != NULL && rc != NULL);

    Package* package = malloc(sizeof(Package));
    assert(package != NULL);

    package->path = path;
    package->name = path_get_name(path);

    package->source_files = hashmap_create(
        PACKAGE_DEFAULT_SOURCE_FILE_COUNT,
        HASHMAP_KEY_SPECS(String, &get_string_hash, &string_eq_str, &string_destroy),
        HASHMAP_VALUE_SPECS(SourceFile*, &source_file_destroy)
    );
    package->subpackages = vector_create(
        PACKAGE_DEFAULT_SUBPACKAGE_COUNT,
        VECTOR_ITEM_SPECS(Package*, NULL)
    );

    package->parent_package = NULL;

    package->rc = rc;

    return package;
}

void package_destroy(Package* package) {
    if (package == NULL) {
        return;
    }

    string_destroy(&package->name);

    hashmap_destroy(&package->source_files);
    vector_destroy(&package->subpackages);

    free(package);
}

void package_add_subpackage(Package* package, Package* subpackage) {
    assert(package != NULL && subpackage != NULL);

    subpackage->parent_package = package;
    vector_push_back(&package->subpackages, &subpackage);
}