#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

#if defined(PLATFORM_WINDOWS)
#define PATH_SEP     '\\'
#define ALT_PATH_SEP '/'
#else
#define PATH_SEP     '/'
#define ALT_PATH_SEP '\\'
#endif

static inline bool is_path_sep(char c) {
    return (c == PATH_SEP) || (c == ALT_PATH_SEP);
}

String get_absolute_path(const String* path);

String get_normalized_path(const String* path);

String get_current_working_dir();

String get_relative_path(const String* abs_from, const String* abs_to);

String get_path_name(const String* path);

String path_join_str(u32 count, const String* paths[]);

String path_join_cstr(u32 count, const char* paths[]);

bool is_path_absolute(const String* path);

bool is_path_exists(const String* path);

bool is_path_a_dir(const String* path);

bool is_path_a_file(const String* path);