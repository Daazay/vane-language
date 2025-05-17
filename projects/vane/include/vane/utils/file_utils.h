#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

typedef enum IOStatus IOStatus;

typedef struct DirEntry DirEntry;
typedef bool(*iterate_directory_fn)(const String* name, bool is_dir, u64 size, void* data);

enum IOStatus {
    IO_STATUS_OK,
    IO_STATUS_ERR_INVALID_PATH,
    IO_STATUS_ERR_FILE_NOT_FOUND,
    IO_STATUS_ERR_FILE_READ_FAILED,
    IO_STATUS_ERR_FILE_EMPTY,
    IO_STATUS_ERR_DIR_NOT_FOUND,
    IO_STATUS_ERR_DIR_READ_FAILED,
};

IOStatus file_load(const String* path, byte** data, u64* size);

IOStatus iterate_directory(const String* dirpath, iterate_directory_fn iterate_fn, void* data);