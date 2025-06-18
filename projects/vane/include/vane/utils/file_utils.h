#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/vector.h"

typedef enum IOStatus IOStatus;

typedef enum DirWalkAction DirWalkAction;
typedef struct DirEntry DirEntry;
typedef DirWalkAction(*dir_walk_callback_fn)(const DirEntry* entry, void* ctx);

enum IOStatus {
    IO_OK = 0,
    IO_ERR_INVALID_PATH,
    IO_ERR_NOT_FOUND,
    IO_ERR_IS_DIR,
    IO_ERR_IS_FILE,
    IO_ERR_ACCESS_DENIED,
    IO_ERR_ALREADY_EXISTS,
    IO_ERR_READ_FAILED,
    IO_ERR_WRITE_FAILED,
    IO_ERR_CREATE_FAILED,
    IO_ERR_NOT_DIR,
    IO_ERR_NOT_FILE,
    IO_ERR_UNKNOWN,
};

enum DirWalkAction {
    DIR_WALK_STOP     = 0,
    DIR_WALK_CONTINUE = 1,
};

struct DirEntry {
    String path;
    bool is_dir;
};

IOStatus file_content_load(const String* path, String* content);

IOStatus dir_walk(const String* path, dir_walk_callback_fn callback_fn, void* ctx);