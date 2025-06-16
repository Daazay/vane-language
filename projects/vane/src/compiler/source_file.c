#include "vane/compiler/source_file.h"

#include <stdlib.h>

#include "vane/ast/ast_parser.h"

SourceFile* source_file_load(const String* path, ReportCollector* rc) {
    assert(path != NULL && rc != NULL);

    FileContent fc = { 0 };

    IOStatus status = file_content_load(&fc, path);
    switch (status) {
    case IO_STATUS_OK: break;
    case IO_STATUS_ERR_INVALID_PATH:
        REPORT_COLLECTOR_REPORT_IO_ERROR(rc, path, "Invalid path.");
        return NULL;
    case IO_STATUS_ERR_FILE_NOT_FOUND:
        REPORT_COLLECTOR_REPORT_IO_ERROR(rc, path, "File not found.");
        return NULL;
    case IO_STATUS_ERR_FILE_READ_FAILED:
        REPORT_COLLECTOR_REPORT_IO_ERROR(rc, path, "Failed to read file content.");
        return NULL;
    case IO_STATUS_ERR_FILE_EMPTY:
        REPORT_COLLECTOR_REPORT_IO_WARN(rc, path, "File is empty.");
        return NULL;
    default:
        unreachable();
        break;
    }

    SourceFile* source_file = malloc(sizeof(SourceFile));
    assert(source_file != NULL);

    source_file->fc = fc;
    source_file->ast_nodes = vector_create(8, VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy));
    source_file->ts = token_stream_create(64, &source_file->fc, rc);
    source_file->rc = rc;

    return source_file;
}

void source_file_destroy(SourceFile* source_file) {
    if (source_file == NULL) {
        return;
    }

    file_content_destroy(&source_file->fc);
    vector_destroy(&source_file->ast_nodes);
    token_stream_destroy(&source_file->ts);

    free(source_file);
}

#include <stdio.h>

void source_file_parse(SourceFile* source_file) {
    assert(source_file != NULL);

    ASTParser ast_parser = ast_parser_create(&source_file->ts, source_file->rc);

    while (!token_stream_is_end(&source_file->ts)) {
        const ASTNode* ast = ast_parser_parse_package_entity(&ast_parser);
        if (ast->kind == AST_NODE_ERROR) {
            token_stream_move_forward(ast_parser.ts);
        }

        vector_push_back(&source_file->ast_nodes, &ast);
    }
}