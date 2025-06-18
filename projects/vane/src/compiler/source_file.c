#include "vane/compiler/source_file.h"

#include <stdlib.h>

#include "vane/utils/file_utils.h"

SourceFile* source_file_create(const String* path, String content, ReportCollector* rc) {
    assert(path != NULL && rc != NULL);

    SourceFile* source_file = malloc(sizeof(SourceFile));
    assert(source_file != NULL);

    source_file->path = path;
    source_file->content = content;

    source_file->package = NULL;

    source_file->rc = rc;

    return source_file;
}

void source_file_destroy(SourceFile* source_file) {
    if (source_file == NULL) {
        return;
    }

    string_destroy(&source_file->content);

    free(source_file);
}