#include "vane/compiler/source_file.h"

#include <stdlib.h>

#include "vane/utils/file_utils.h"

#include "vane/scanner/token_stream.h"

#include "vane/ast/ast_parser.h"

#define SOURCE_FILE_DEFAULT_AST_NODES_SIZE 32

SourceFile* source_file_create(const String* path, String content, ReportCollector* rc) {
    assert(path != NULL && rc != NULL);

    SourceFile* source_file = malloc(sizeof(SourceFile));
    assert(source_file != NULL);

    source_file->path = path;
    source_file->content = content;

    source_file->ast_nodes = vector_create(
        SOURCE_FILE_DEFAULT_AST_NODES_SIZE,
        VECTOR_ITEM_SPECS(ASTNode*, &ast_node_destroy)
    );
    source_file->package = NULL;

    source_file->rc = rc;

    return source_file;
}

void source_file_destroy(SourceFile* source_file) {
    if (source_file == NULL) {
        return;
    }

    string_destroy(&source_file->content);
    vector_destroy(&source_file->ast_nodes);

    free(source_file);
}

bool source_file_parse(SourceFile* source_file) {
    assert(source_file != NULL);

    bool success = true;

    TokenStream ts = token_stream_create(0, source_file->path, &source_file->content, source_file->rc);
    ASTParser ast_parser = ast_parser_create(&ts);

    while (!is_token_stream_end(&ts)) {
        ASTNode* ast = ast_parser_parse_package_entity(&ast_parser);

        if (ast->kind == AST_NODE_ERROR) {
            success = false;
            token_stream_move_forward(&ts); // recover
        }

        vector_push_back(&source_file->ast_nodes, &ast);
    }

    ast_parser_destroy(&ast_parser);
    token_stream_destroy(&ts);

    return success;
}