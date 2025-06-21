#include "vane/compiler/compiler.h"

#include <stdio.h>

#include "vane/utils/hash.h"
#include "vane/utils/file_utils.h"
#include "vane/utils/path.h"

#include "vane/ast/visitors/ast_dot_visitor.h"

#include "vane/compiler/import_entry.h"

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

    compiler.type_system = (TypeSystem){ 0 };

    compiler.entry_point = NULL;

    compiler.rc = report_collector_create();

    return compiler;
}

void compiler_destroy(Compiler* compiler) {
    if (compiler == NULL) {
        return;
    }

    hashmap_destroy(&compiler->packages);

    type_system_destroy(&compiler->type_system);

    report_collector_destroy(&compiler->rc);

    build_options_destroy(compiler->build_options);
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

    // A bit dirty, but for now oK
    hashmap_put(&ctx->package->source_files, path_, NULL);
    const String* path = hashmap_key_at(&ctx->package->source_files, path_);

    // walk_dir should not destroy this string
    *path_ = STRING_EMPTY;

    SourceFile* source_file = source_file_load(path, &ctx->compiler->rc);
    package_add_source_file(ctx->package, source_file);

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
        RC_REPORT_INTERNAL_ERROR(&compiler->rc, "Invalid directoty path - `%.*s`", (i32)path->len, path->text);
        return NULL;
    case IO_ERR_NOT_FOUND:
        RC_REPORT_INTERNAL_ERROR(&compiler->rc, "Directory not found - `%.*s`", (i32)path->len, path->text);
        return NULL;
    case IO_ERR_READ_FAILED:
        RC_REPORT_INTERNAL_ERROR(&compiler->rc, "Failed to read directory entries - `%.*s`", (i32)path->len, path->text);
        return NULL;
    default:
        unreachable();
        return NULL;
    }

    return package;
}

Package* compiler_try_load_imported_package(Compiler* compiler, const String* collection_name, const String* package_path) {
    assert(compiler != NULL && collection_name != NULL && package_path != NULL);

    String path = STRING_EMPTY;
    if (!is_string_empty(collection_name)) {
        const String* collection_path = compiler_get_collection_path(compiler, collection_name);
        if (collection_path == NULL) {
            RC_REPORT_INTERNAL_ERROR(&compiler->rc, "unknown collection `%.*s`", (i32)collection_name->len, collection_name->text);
            return NULL;
        }

        path = path_join_str(2, (const String *[]) { collection_path, package_path });
    }
    else {
        path = path_join_str(2, (const String *[]) { &compiler->build_options->root_path, package_path });
    }

    Package* package = compiler_load_package(compiler, &path);
    string_destroy(&path);

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

bool compiler_print_ast(Compiler* compiler) {
    assert(compiler != NULL);

    bool save_to_file = compiler->build_options->command_options.parse_ast.save_to_file;

    if (save_to_file) {
        // TODO:
        unreachable();
        return false;
    }

    HashmapIterator package_it = hashmap_get_it(&compiler->packages);
    while (hashmap_it_next(&package_it)) {
        const Package* package = package_it.value;
        if (package == NULL) {
            continue;
        }

        HashmapIterator source_file_it = hashmap_get_it(&package->source_files);
        while (hashmap_it_next(&source_file_it)) {
            const SourceFile* source_file = source_file_it.value;
            if (source_file == NULL) {
                continue;
            }

            printf("[file: %.*s]\n", (i32)source_file->path->len, source_file->path->text);

            ast_print_dot(source_file->ast, stdout);
        }
    }

    return true;
}

bool compiler_show_imports(Compiler* compiler) {
    assert(compiler != NULL);

    HashmapIterator package_it = hashmap_get_it(&compiler->packages);
    while (hashmap_it_next(&package_it)) {
        const Package* package = package_it.value;
        if (package == NULL) {
            continue;
        }

        //printf("[package:%.*s] - `%lld`\n", (i32)package->name.len, package->name.text, (u64)package);

        HashmapIterator source_file_it = hashmap_get_it(&package->source_files);
        while (hashmap_it_next(&source_file_it)) {
            const SourceFile* source_file = source_file_it.value;
            if (source_file == NULL) {
                continue;
            }

            printf("file:`%.*s`\n", (i32)source_file->path->len, source_file->path->text);

            if (source_file->imports.size == 0) {
                continue;
            }

            for (u32 i = 0; i < source_file->imports.size; ++i) {
                const ImportEntry* import = vector_at(&source_file->imports, i);
                const String* ast_path = &import->node->as.import_decl.path->as.expr_literal.value;

                printf("    - [package:%.*s] - ", (i32)ast_path->len, ast_path->text);

                if (import->target == NULL) {
                    printf("unresolved\n");
                }
                else {
                    printf("`%.*s`\n", (i32)import->target->path->len, import->target->path->text);
                }
            }
        }
    }

    return true;
}

bool compiler_resolve_symbols(Compiler* compiler) {
    assert(compiler != NULL);

    bool success = true;

    HashmapIterator package_it = hashmap_get_it(&compiler->packages);
    while (hashmap_it_next(&package_it)) {
        Package* package = package_it.value;
        if (package == NULL) {
            continue;
        }

        bool ok = package_resolve_symbols(package);
        if (!ok) {
            success = false;
        }
    }

    return success;
}

bool compiler_resolve_entry_point(Compiler* compiler) {
    assert(compiler != NULL);

    if (compiler->entry_point != NULL) {
        return true;
    }

    bool success = false;

    HashmapIterator package_it = hashmap_get_it(&compiler->packages);
    while (hashmap_it_next(&package_it)) {
        Package* package = package_it.value;
        if (package == NULL) {
            continue;
        }

        if (package_resolve_entry_point(package)) {
            if (compiler->entry_point == NULL) {
                compiler->entry_point = package;
                success = true;
                continue;
            }
            success = false;

            RC_TRACE(&compiler->rc, (SourceLoc){ 0 }, "Entry point in package `%.*s`",
                (i32)package->name.len, package->name.text
            );
        }
    }

    if (!success) {
        if (compiler->entry_point != NULL) {
            RC_TRACE(&compiler->rc, (SourceLoc) { 0 }, "Entry point in package `%.*s`",
                (i32)compiler->entry_point->name.len, compiler->entry_point->name.text
            );
            RC_REPORT_SEMANTIC_ERROR(&compiler->rc, (SourceLoc) { 0 }, "Multiple entry points found: function `main` is defined in multiple packages");
        }
        else {
            RC_REPORT_SEMANTIC_ERROR(&compiler->rc, (SourceLoc) { 0 }, "Entry point not found.");
        }
    }

    return success;
}

bool compiler_resolve_types(Compiler* compiler) {
    assert(compiler != NULL);

    bool success = true;

    HashmapIterator package_it = hashmap_get_it(&compiler->packages);
    while (hashmap_it_next(&package_it)) {
        Package* package = package_it.value;
        if (package == NULL) {
            continue;
        }

        if (package->scope == NULL) {
            continue;
        }

        bool ok = scope_resolve_types(package->scope, &compiler->type_system, &compiler->rc);
        if (!ok) {
            success = false;
        }
    }

    return success;
}