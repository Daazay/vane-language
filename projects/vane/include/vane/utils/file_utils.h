#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/vector.h"

typedef enum IOStatus IOStatus;

typedef enum DirWalkAction DirWalkAction;
typedef struct DirWalkCtx DirWalkCtx;

typedef DirWalkAction(*dir_walk_file_callback_fn)(String* path, void* data);
typedef DirWalkAction(*dir_walk_dir_callback_fn)(String* path, void* data);

enum IOStatus {
    IO_OK = 0,
    IO_ERR_INVALID_PATH,
    IO_ERR_NOT_FOUND,
    IO_ERR_NOT_FILE,
    IO_ERR_NOT_DIR,
    IO_ERR_IS_FILE,
    IO_ERR_IS_DIR,
    IO_ERR_ALREADY_EXIST,
    IO_ERR_ACCESS_DENIED,
    IO_ERR_EMPTY_FILE,
    IO_ERR_READ_FAILED,
    IO_ERR_UNKNOWN,
};

enum DirWalkAction {
    DIR_WALK_STOP     = 0,
    DIR_WALK_CONTINUE = 1,
};

struct DirWalkCtx {
    dir_walk_file_callback_fn file_callack_fn;
    dir_walk_file_callback_fn dir_callack_fn;
    void* data;
};

const char* get_io_status_text(IOStatus status);

IOStatus file_content_load(const String* path, String* content);

IOStatus dir_walk(const String* path, const DirWalkCtx* ctx);

IOStatus file_create(const String* path, bool overwrite);

IOStatus dir_create(const String* path);