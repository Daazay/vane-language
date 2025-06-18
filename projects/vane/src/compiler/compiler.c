#include "vane/compiler/compiler.h"

#include "vane/utils/hash.h"
#include "vane/utils/file_utils.h"
#include "vane/utils/path.h"

#pragma region UTILITIES

#define COMPILER_DEFAULT_PACKAGES_COUNT 8
#define COMPILER_DEFAULT_COLLECTION_PATHS_COUNT 2

#pragma endregion

Compiler compiler_create(BuildOptions* build_options) {
    assert(build_options != NULL);

    Compiler compiler = { 0 };

    compiler.build_options = build_options;

    compiler.packages = hashmap_create(
        COMPILER_DEFAULT_PACKAGES_COUNT,
        HASHMAP_KEY_SPECS(String, &get_string_hash, &string_eq_str, &string_destroy),
        HASHMAP_VALUE_SPECS(Package*, &package_destroy)
    );

    compiler.rc = report_collector_create();

    return compiler;
}

void compiler_destroy(Compiler* compiler) {
    if (compiler == NULL) {
        return;
    }

    hashmap_destroy(&compiler->packages);

    report_collector_destroy(&compiler->rc);
}

struct PackageWalkCtx {
    Compiler* compiler;
    Package* package;
};

static DirWalkAction package_walk_file_callback(String* path_, void* data) {
    assert(path_ != NULL && data != NULL);

    // Process only files with `.vn` ext
    if (!string_has_suffix_cstr(path_, ".vn")) {
        return DIR_WALK_CONTINUE;
    }

    struct PackageWalkCtx* ctx = data;

    hashmap_put(&ctx->package->source_files, path_, NULL);
    const String* path = hashmap_key_at(&ctx->package->source_files, path_);

    // walk_dir should not destroy this string
    *path_ = STRING_EMPTY;


    String content = STRING_EMPTY;

    IOStatus status = file_content_load(path, &content);
    switch (status) {
    case IO_OK: break;
    case IO_ERR_INVALID_PATH:
        RC_REPORT_IO_ERROR(&ctx->compiler->rc, path, "Invalid path.");
        return DIR_WALK_CONTINUE;
    case IO_ERR_NOT_FOUND:
        RC_REPORT_IO_ERROR(&ctx->compiler->rc, path, "File not found.");
        return DIR_WALK_CONTINUE;
    case IO_ERR_EMPTY_FILE:
        RC_REPORT_IO_DEBUG(&ctx->compiler->rc, path, "File is empty");
        return DIR_WALK_CONTINUE;
    case IO_ERR_READ_FAILED:
        RC_REPORT_IO_ERROR(&ctx->compiler->rc, path, "Failed to read file content.");
        return DIR_WALK_CONTINUE;
    default:
        unreachable();
        return DIR_WALK_CONTINUE;
    }

    SourceFile* source_file = source_file_create(path, content, &ctx->compiler->rc);
    hashmap_put(&ctx->package->source_files, path, &source_file);

    return DIR_WALK_CONTINUE;
}

static DirWalkAction package_walk_dir_callback(String* path, void* data) {
    assert(path != NULL && data != NULL);

    if (string_has_prefix_cstr(path, ".")) {
        return DIR_WALK_CONTINUE;
    }

    struct PackageWalkCtx* ctx = data;

    Package* subpackage = compiler_load_package(ctx->compiler, path);
    if (subpackage != NULL) {
        package_add_subpackage(ctx->package, subpackage);
    }

    return DIR_WALK_CONTINUE;
}

const String* compiler_get_collection_path(Compiler* compiler, const String* collection_name) {
    assert(compiler != NULL && collection_name != NULL);

    return hashmap_at(&compiler->build_options->collections, collection_name);
}

Package* compiler_load_package(Compiler* compiler, const String* dirpath) {
    assert(compiler != NULL && dirpath != NULL);

    // Resolve the absolute path and check if the package has already been loaded.
    // If it has, return the existing package associated with that path.
    // If not, insert the absolute path into the hashmap to track loading.
    // This ensures that we already tries to load package by this path.
    const String* path = NULL;
    {
        String abs_path = path_to_absolute(dirpath);

        Package* package = hashmap_at(&compiler->packages, &abs_path);
        if (package != NULL) {
            string_destroy(&abs_path);
            return package;
        }

        hashmap_put(&compiler->packages, &abs_path, NULL);
        path = hashmap_key_at(&compiler->packages, &abs_path);
    }

    Package* package = package_create(path, &compiler->rc);
    hashmap_put(&compiler->packages, path, &package);

    struct PackageWalkCtx data = {
        .compiler = compiler,
        .package = package,
    };

    DirWalkCtx ctx = {
        .data = &data,
        .file_callack_fn = &package_walk_file_callback,
        .dir_callack_fn = &package_walk_dir_callback,
    };

    IOStatus status = dir_walk(path, &ctx);
    switch (status) {
    case IO_OK: break;
    case IO_ERR_INVALID_PATH:
        RC_REPORT_IO_ERROR(&compiler->rc, path, "Invalid path");
        return NULL;
    case IO_ERR_NOT_FOUND:
        RC_REPORT_IO_ERROR(&compiler->rc, path, "Directory not found");
        return NULL;
    case IO_ERR_READ_FAILED:
        RC_REPORT_IO_ERROR(&compiler->rc, path, "Failed to read directory entries");
        return NULL;
    default:
        unreachable();
        return NULL;
    }

    return package;
}

Package* compiler_load_imported_package(Compiler* compiler, const String* import_path) {
    assert(compiler != NULL && import_path != NULL);

    i64 colon_pos = string_find_c(import_path, ':');

    String path_to_import = STRING_EMPTY;

    if (colon_pos != NPOS && colon_pos > 0) {
        String collection_name = string_substr(import_path, 0, colon_pos);

        const String* collection_path = compiler_get_collection_path(compiler, &collection_name);
        if (collection_path == NULL) {
            RC_REPORT_INTERNAL_ERROR(&compiler->rc, "unknown collection `%.*s`", (i32)collection_name.len, collection_name.text);
            string_destroy(&collection_name);
            return NULL;
        }

        String package_path = string_substr(import_path, colon_pos + 1, import_path->len - colon_pos);
        path_to_import = path_join_str(2, (const String *[]) { collection_path, &package_path });
        string_destroy(&package_path);
    }
    else {
        String package_path = string_substr(import_path, colon_pos + 1, import_path->len - colon_pos);
        path_to_import = path_join_str(2, (const String *[]) { &compiler->build_options->root_path, &package_path });
        string_destroy(&package_path);
    }

    Package* package = compiler_load_package(compiler, &path_to_import);
    string_destroy(&path_to_import);

    return package;
}

bool compiler_parse_source_files(Compiler* compiler) {
    assert(compiler != NULL);

    bool success = true;

    HashmapIterator package_it = hashmap_get_it(&compiler->packages);
    while (hashmap_it_next(&package_it)) {
        Package* package = package_it.value;
        if (package == NULL) {
            continue;
        }

        bool ok = package_parse_source_files(package);
        if (!ok) {
            success = false;
        }
    }

    return success;
}

bool compiler_resolve_imports(Compiler* compiler) {
    assert(compiler != NULL);

    bool success = true;

    HashmapIterator package_it = hashmap_get_it(&compiler->packages);
    while (hashmap_it_next(&package_it)) {
        Package* package = package_it.value;
        if (package == NULL) {
            continue;
        }

        bool ok = package_resolve_imports(package, compiler);
        if (!ok) {
            success = false;
        }
    }

    return success;
}