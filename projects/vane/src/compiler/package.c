#include "vane/compiler/package.h"

#include <stdlib.h>

#include "vane/utils/path.h"
#include "vane/utils/hash.h"

#include "vane/compiler/compiler.h"

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

    package->scope = NULL;
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

    scope_destroy(package->scope);

    free(package);
}

void package_add_source_file(Package* package, SourceFile* source_file) {
    assert(package != NULL && source_file != NULL);

    source_file->package = package;
    hashmap_put(&package->source_files, source_file->path, &source_file);
}

void package_add_subpackage(Package* package, Package* subpackage) {
    assert(package != NULL && subpackage != NULL);

    subpackage->parent_package = package;
    vector_push_back(&package->subpackages, &subpackage);
}

bool package_parse_source_files(Package* package) {
    assert(package != NULL);

    bool success = true;

    HashmapIterator source_file_it = hashmap_get_it(&package->source_files);
    while (hashmap_it_next(&source_file_it)) {
        SourceFile* source_file = source_file_it.value;
        if (source_file == NULL) {
            continue;
        }

        bool ok = source_file_parse(source_file);
        if (!ok) {
            success = false;
        }
    }

    return success;
}

bool package_resolve_imports(Package* package, struct Compiler* compiler) {
    assert(package != NULL);

    bool success = true;

    HashmapIterator source_file_it = hashmap_get_it(&package->source_files);
    while (hashmap_it_next(&source_file_it)) {
        SourceFile* source_file = source_file_it.value;
        if (source_file == NULL) {
            continue;
        }

        bool ok = source_file_resolve_imports(source_file, compiler);
        if (!ok) {
            success = false;
        }
    }

    return success;
}

bool package_resolve_identifiers(Package* package) {
    assert(package != NULL);

    if (package->scope != NULL) {
        return true;
    }

    package->scope = scope_create(string_clone(package->path), NULL);

    bool success = true;

    HashmapIterator source_file_it = hashmap_get_it(&package->source_files);
    while (hashmap_it_next(&source_file_it)) {
        SourceFile* source_file = source_file_it.value;
        if (source_file == NULL) {
            continue;
        }

        bool ok = source_file_resolve_identifiers(source_file);
        if (!ok) {
            success = false;
        }
    }

    return success;
}