#include "vane/compiler/source_file.h"

#include <stdlib.h>

#include "vane/utils/file_utils.h"
#include "vane/utils/path.h"

#include "vane/scanner/token_stream.h"

#include "vane/ast/ast_parser.h"

#include "vane/compiler/compiler.h"
#include "vane/compiler/import_entry.h"

#define SOURCE_FILE_DEFAULT_AST_NODES_SIZE 32
#define SOURCE_FILE_DEFAULT_IMPORTS_SIZE   4

SourceFile* source_file_load(const String* path, ReportCollector* rc) {
    assert(path != NULL && rc != NULL);

    String content = STRING_EMPTY;

    IOStatus status = file_content_load(path, &content);
    switch (status) {
    case IO_OK: break;
    case IO_ERR_INVALID_PATH:
        RC_REPORT_INTERNAL_ERROR(rc, "Invalid filepath  - `%.*s`", (i32)path->len, path->text);
        return NULL;
    case IO_ERR_NOT_FOUND:
        RC_REPORT_INTERNAL_ERROR(rc, "File not found - `%.*s`", (i32)path->len, path->text);
        return NULL;
    case IO_ERR_EMPTY_FILE:
        RC_REPORT_INTERNAL_WARN(rc, "File is empty  - `%.*s`", (i32)path->len, path->text);
        return NULL;
    case IO_ERR_READ_FAILED:
        RC_REPORT_INTERNAL_ERROR(rc, "Failed to read file content  - `%.*s`", (i32)path->len, path->text);
        return NULL;
    default:
        unreachable();
        return NULL;
    }

    SourceFile* source_file = malloc(sizeof(SourceFile));
    assert(source_file != NULL);

    source_file->path = path;
    source_file->content = content;

    source_file->ast = NULL;

    source_file->imports = vector_create(
        SOURCE_FILE_DEFAULT_IMPORTS_SIZE,
        VECTOR_ITEM_SPECS(ImportEntry, &import_entry_destroy)
    );

    source_file->scope = NULL;
    source_file->package = NULL;

    source_file->rc = rc;

    return source_file;
}

void source_file_destroy(SourceFile* source_file) {
    if (source_file == NULL) {
        return;
    }

    string_destroy(&source_file->content);
    ast_node_destroy(source_file->ast);
    vector_destroy(&source_file->imports);

    // This scope will be destroyed when the package containing this source file is destroyed.
    //scope_destroy(source_file->scope);

    free(source_file);
}

bool source_file_parse(SourceFile* source_file) {
    assert(source_file != NULL);

    bool success = true;

    TokenStream ts = token_stream_create(0, source_file->path, &source_file->content, source_file->rc);
    ASTParser ast_parser = ast_parser_create(&ts);

    //
    source_file->ast = ast_node_create(AST_NODE_SOURCE_FILE, token_stream_peek_next(&ts)->loc);
    source_file->ast->as.source_file.entities = vector_create(8, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));
    //

    while (!is_token_stream_end(&ts)) {
        ASTNode* ast = ast_parser_parse_package_entity(&ast_parser);

        switch (ast->kind) {
        case AST_NODE_ERROR:
            success = false;
            token_stream_move_forward(&ts); // recover
            break;
        case AST_NODE_IMPORT_DECL:
            ImportEntry import = {
                .node = ast,
                .target = NULL,
            };
            vector_push_back(&source_file->imports, &import);
            break;
        default:
            break;
        }

        vector_push_back(&source_file->ast->as.source_file.entities, &ast);
    }

    ast_parser_destroy(&ast_parser);
    token_stream_destroy(&ts);

    return success;
}

static void split_import_path(const String* path, String* collection_name, String* package_path) {
    assert(path != NULL);

    i64 colon_pos = string_find_c(path, ':');

    if (colon_pos == NPOS) {
        if (package_path != NULL) {
            *package_path = string_clone(path);
        }
    }
    else if (colon_pos == 0) {
        if (package_path != NULL) {
            *package_path = string_substr(path, colon_pos + 1, path->len - colon_pos);
        }
    }
    else {
        if (collection_name != NULL) {
            *collection_name = string_substr(path, 0, colon_pos);
        }
        if (package_path != NULL) {
            *package_path = string_substr(path, colon_pos + 1, path->len - colon_pos);
        }
    }
}

bool source_file_resolve_imports(SourceFile* source_file, struct Compiler* compiler) {
    assert(source_file != NULL);

    bool success = true;

    for (u32 i = 0; i < source_file->imports.size; ++i) {
        ImportEntry* entry = vector_at(&source_file->imports, i);

        const String* ast_path = &entry->node->as.import_decl.path->as.expr_literal.value;
        String collection_name = STRING_EMPTY;
        String package_path = STRING_EMPTY;

        split_import_path(ast_path, &collection_name, &package_path);

        entry->target = compiler_try_load_imported_package(compiler, &collection_name, &package_path);

        if (entry->node->as.import_decl.alias != NULL) {
            entry->name = string_clone(&entry->node->as.import_decl.alias->as.id.value);
        }
        else if (entry->target != NULL) {
            entry->name = path_get_name(entry->target->path);
        }
        else {
            entry->name = path_get_name(&package_path);
        }

        string_destroy(&collection_name);
        string_destroy(&package_path);

        if (entry->target == NULL) {
            RC_REPORT_INTERNAL_ERROR(source_file->rc, "cannot resolve import path `%.*s`", (i32)ast_path->len, ast_path->text);
            success = false;
            continue;
        }
    }

    return success;
}

bool source_file_resolve_symbols(SourceFile* source_file) {
    assert(source_file != NULL);

    if (source_file->scope != NULL) {
        return true;
    }

    source_file->scope = scope_create(SCOPE_SOURCE_FILE, source_file->package->scope);

    bool success = true;

    if (!scope_resolve_import_symbols(source_file->scope, &source_file->imports, source_file->rc)) {
        success = false;
    }
    if (!scope_resolve_source_file_symbols(source_file->scope, &source_file->ast->as.source_file.entities, source_file->rc)) {
        success = false;
    }

    return success;
}