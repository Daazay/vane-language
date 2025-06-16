#include "vane/compiler/compiler.h"

#include "vane/utils/path.h"
#include "vane/utils/file_utils.h"
#include "vane/utils/hash.h"

Compiler compiler_create(BuildOptions* build_options) {
    return (Compiler) {
        .build_options = build_options,
        .packages = hashmap_create(8,
            HASHMAP_KEY_SPECS(String, &get_string_hash, &string_eq_str, &string_destroy),
            HASHMAP_VALUE_SPECS(Package*, &package_destroy)
        ),
        .rc = report_collector_create(build_options->colored_output),
    };
}

void compiler_destroy(Compiler* compiler) {
    if (compiler == NULL) {
        return;
    }

    hashmap_destroy(&compiler->packages);
    report_collector_destroy(&compiler->rc);
}

struct PackageDirectoryIteratorCtx {
    Compiler* compiler;
    Package* curr_pkg;
    // String
    Vector subpackages;
};

static bool iterate_package_directory_fn(const String* dirpath, const String* name, bool is_dir, u64 size, void* uctx) {
    struct PackageDirectoryIteratorCtx* ctx = uctx;

    // skip hidden/system directories
    if (is_dir && string_has_prefix_cstr(name, ".")) {
        return true;
    }

    String new_path = path_join_str(2, (const String *[]) { dirpath, name });

    if (is_dir) {
        if (ctx->subpackages.raw == NULL) {
            ctx->subpackages = vector_create(4, VECTOR_ITEM_SPECS(String, &string_destroy));
        }

        vector_push_back(&ctx->subpackages, &new_path);
    }
    else if (string_has_suffix_cstr(name, ".vn")) {
        // Why? idk. I just want to save the string in the hashmap as a key.
        // If something goes wrong and we can't load/parse the source_file, in which case it will be null,
        // we will still get an entry with this key, so we won't try to load/parse it anymore.

        // Put new_path in hashmap to store
        HashmapEntry entry = hashmap_put(&ctx->curr_pkg->source_files, &new_path, NULL);

        SourceFile* source_file = source_file_load(entry.key, &ctx->compiler->rc);
        hashmap_put(&ctx->curr_pkg->source_files, entry.key, &source_file);
    }
    else {
        string_destroy(&new_path);
    }
    return true;
}

Package* compiler_load_package(Compiler* compiler, const String* dirpath) {
    assert(compiler != NULL && dirpath != NULL);

    String abs_path = get_absolute_path(dirpath);

    Package* pkg = hashmap_at(&compiler->packages, &abs_path);
    if (pkg != NULL) {
        return pkg;
    }

    // Why? idk. I just want to save the string in the hashmap as a key.
    // If something goes wrong and we can't load/parse the package, in which case it will be null,
    // we will still get an entry with this key, so we won't try to load/parse it anymore.

    // Put abs_path in hashmap to store
    HashmapEntry entry = hashmap_put(&compiler->packages, &abs_path, NULL);
    pkg = package_create(entry.key, &compiler->rc);
    hashmap_put(&compiler->packages, entry.key, &pkg);

    struct PackageDirectoryIteratorCtx iter_ctx = {
        .compiler = compiler,
        .curr_pkg = pkg,
        .subpackages = (Vector) { 0 },
    };

    IOStatus status = iterate_directory(&abs_path, &iterate_package_directory_fn, &iter_ctx);
    switch (status) {
    case IO_STATUS_OK: break;
    case IO_STATUS_ERR_INVALID_PATH:
        REPORT_COLLECTOR_REPORT_IO_ERROR(&compiler->rc, entry.key, "Invalid path.");
        return NULL;
    case IO_STATUS_ERR_DIR_NOT_FOUND:
        REPORT_COLLECTOR_REPORT_IO_ERROR(&compiler->rc, entry.key, "Directory not found.");
        return NULL;
    case IO_STATUS_ERR_DIR_READ_FAILED:
        REPORT_COLLECTOR_REPORT_IO_ERROR(&compiler->rc, entry.key, "Failed to read directory entries.");
        return NULL;
    default:
        unreachable();
        break;
    }

    for (u32 i = 0; i < iter_ctx.subpackages.size; ++i) {
        const String* subpackage_path = vector_at(&iter_ctx.subpackages, i);
        Package* subpkg = compiler_load_package(compiler, subpackage_path);

        if (subpkg != NULL) {
            package_add_subpackage(pkg, subpkg);
        }
    }

    vector_destroy(&iter_ctx.subpackages);

    return pkg;
}