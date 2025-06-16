#include "vane/compiler/package.h"

#include <stdlib.h>

#include "vane/utils/hash.h"
#include "vane/utils/path.h"
#include "vane/utils/file_utils.h"

Package* package_create(const String* dirpath, ReportCollector* rc) {
    assert(dirpath != NULL);

    Package* package = malloc(sizeof(Package));
    assert(package != NULL);

    package->dirpath = dirpath;
    package->name = get_path_name(dirpath);
    package->rc = rc;
    package->source_files = hashmap_create(8,
        HASHMAP_KEY_SPECS(String, &get_string_hash, &string_eq_str, &string_destroy),
        HASHMAP_VALUE_SPECS(SourceFile*, &source_file_destroy)
    );
    package->subpackages = vector_create(4, VECTOR_ITEM_SPECS(Package*, NULL));
    package->parent_package = NULL;

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