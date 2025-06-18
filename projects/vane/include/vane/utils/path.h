#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

#if defined(PLATFORM_WINDOWS)
#define PATH_SEP         '\\'
#define PATH_SEP_STR     "\\"
#define ALT_PATH_SEP     '/'
#define ALT_PATH_SEP_STR "/"
#else
#define ALT_PATH_SEP     '\\'
#define ALT_PATH_SEP_STR "\\"
#define PATH_SEP         '/'
#define PATH_SEP_STR     "/"
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

static inline bool is_path_sep(char c) {
    return (c == PATH_SEP) || (c == ALT_PATH_SEP);
}

String path_to_absolute(const String* path);

String path_normalize(const String* path);

String get_current_working_dir();

String path_get_dir(const String* path);

String path_get_name(const String* path);

String path_get_ext(const String* path);

String path_join_cstr(u32 count, const char* paths[]);

String path_join_str(u32 count, const String* paths[]);

bool is_path_absolute(const String* path);

bool is_path_exist(const String* path);

bool is_path_dir(const String* path);

bool is_path_file(const String* path);