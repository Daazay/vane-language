#include "vane/utils/path.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#endif

#include "vane/utils/vector.h"
#include "vane/utils/string_utils.h"
#include "vane/utils/string_builder.h"

#if defined(PLATFORM_WINDOWS)
#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif
#endif

String path_to_absolute(const String* path) {
    assert(path != NULL);

    if (is_string_empty(path)) {
        return STRING_EMPTY;
    }

    if (is_path_absolute(path)) {
        return string_clone(path);
    }

#if defined(PLATFORM_WINDOWS)
    DWORD len = GetFullPathNameA(path->text, 0, NULL, NULL);
    if (len == 0) {
        return STRING_EMPTY;
    }

    char* text = malloc((u64)len + 1);
    assert(text != NULL);

    DWORD ret = GetFullPathNameA(path->text, len, text, NULL);
    if (ret == 0 || ret >= len) {
        free(text);
        return STRING_EMPTY;
    }

    return string_create(text, (u64)ret);
#else
    char cwd[PATH_MAX] = { 0 };
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return STRING_EMPTY;
    }

    String abs_path = path_join_cstr(2, (const char* []) { cwd, path->text });
    String normalized = path_normalize(&abs_path);
    string_destroy(&abs_path);

    return normalized;
#endif
}

String path_normalize(const String* path) {
    assert(path != NULL);

    if (is_string_empty(path)) {
        return STRING_EMPTY;
    }

    Vector components = vector_create(4, VECTOR_ITEM_SPECS(String, NULL));

    u64 i = 0;
    while (i < path->len) {
        while (i < path->len && is_path_sep(path->text[i])) {
            ++i;
        }

        if (i == path->len) {
            break;
        }

        i64 start = i;

        while (i < path->len && !is_path_sep(path->text[i])) {
            ++i;
        }

        u64 len = i - start;

        if (len == 0) {
            continue;
        }

        String component = (String){ .text = path->text + start, .len = len };

        if (string_eq_cstr(&component, ".")) {
            continue;
        }
        else if (string_eq_cstr(&component, "..")) {
            if (components.size > 0) {
                const String* last = vector_at_back(&components);
                if (!string_eq_cstr(last, "..")) {
                    vector_pop_back(&components);
                    continue;
                }
            }
        }

        vector_push_back(&components, &component);
    }

    StringBuilder sb = string_builder_create(32);

    if (is_path_absolute(path)) {
#if defined(PLATFORM_WINDOWS)
        string_builder_append_c(&sb, path->text[0]);
        string_builder_append_c(&sb, ':');
        string_builder_append_c(&sb, PATH_SEP);
#else
        string_builder_append_c(&sb, PATH_SEP);
#endif
    }

    for (u32 j = 0; j < components.size; ++j) {
        const String* comp = vector_at(&components, j);
        string_builder_append_str(&sb, comp);
        if (j + 1 < components.size) {
            string_builder_append_c(&sb, PATH_SEP);
        }
    }

    if (components.size == 0) {
#if !defined(PLATFORM_WINDOWS)
        if (path->len == 0 || !is_path_sep(path->text[0])) {
            string_builder_append_c(&sb, '.');
        }
#else
        if (!(path->len >= 2 && path->text[1] == ':')) {
            string_builder_append_c(&sb, '.');
        }
#endif
    }

    String normalized = string_builder_get_str(&sb);
    string_builder_destroy(&sb);
    vector_destroy(&components);

    return normalized;
}

String get_current_working_dir() {
#if defined PLATFORM_WINDOWS
    char buf[PATH_MAX] = { 0 };
    DWORD len = GetCurrentDirectoryA(sizeof(buf), buf);
    if (len == 0 || len >= MAX_PATH) {
        return STRING_EMPTY;
    }
    String tmp = { .text = buf, .len = len, };
    return string_clone(&tmp);
#else
    char buf[PATH_MAX] = { 0 };
    if (!getcwd(buf, sizeof(buf))) {
        return STRING_EMPTY;
    }
    return string_from_cstr(buf);
#endif
}

String path_get_dir(const String* path) {
    assert(path != NULL);

    if (is_string_empty(path)) {
        return STRING_EMPTY;
    }

    // skip trailing separators
    i64 end = path->len;
    while (end >= 0 && is_path_sep(path->text[end])) {
        --end;
    }

    if (end == NPOS) {
        return string_from_cstr(PATH_SEP_STR);
    }

    i64 sep_pos = NPOS;
    for (i64 i = end; i >= 0; --i) {
        if (is_path_sep(path->text[i])) {
            sep_pos = i;
            break;
        }
    }

    if (sep_pos == NPOS) {
        return string_from_cstr(".");
    }

    if (sep_pos == 0) {
        return string_from_cstr(PATH_SEP_STR);
    }

    return string_substr(path, 0, (u64)sep_pos);
}

String path_get_name(const String* path) {
    assert(path != NULL);

    if (is_string_empty(path)) {
        return STRING_EMPTY;
    }

    // skip trailing separators
    i64 end = path->len;
    while (end >= 0 && is_path_sep(path->text[end])) {
        --end;
    }

    if (end == NPOS) {
        return string_from_cstr(PATH_SEP_STR);
    }

    i64 start = end;
    while (start >= 0 && !is_path_sep(path->text[start])) {
        --start;
    }

    u64 name_start = (u64)(start + 1);
    u64 name_len = (u64)(end - start);
    return string_substr(path, name_start, name_len);
}

String path_get_ext(const String* path) {
    assert(path != NULL);

    if (is_string_empty(path)) {
        return STRING_EMPTY;
    }

    i64 last_sep_pos = NPOS;
    for (i64 i = (i64)path->len; i-- > 0; ) {
        if (is_path_sep(path->text[i])) {
            last_sep_pos = i;
            break;
        }
    }

    u64 base_start = (last_sep_pos != NPOS) ? (u64)(last_sep_pos + 1) : 0;
    u64 base_len = path->len - base_start;

    if (base_len == 0) {
        return STRING_EMPTY;
    }

    // If "." or ".."
    if (base_len == 1 && path->text[base_start] == '.') {
        return STRING_EMPTY;
    }
    if (base_len == 2 && path->text[base_start] == '.' && path->text[base_start + 1] == '.') {
        return STRING_EMPTY;
    }

    i64 last_dot_pos = NPOS;
    for (i64 i = (base_start + base_len); i-- > (i64)base_start; ) {
        if (path->text[i] == '.') {
            last_dot_pos = i;
            break;
        }
    }

    if (last_dot_pos == NPOS) {
        return STRING_EMPTY;
    }

    // Hidden files like '.git'
    if ((u64)last_dot_pos == base_start) {
        return STRING_EMPTY;
    }

    u64 ext_start = (u64)last_dot_pos + 1;
    u64 ext_len = path->len - ext_start;

    if (ext_len == 0) {
        return STRING_EMPTY;
    }

    return string_substr(path, ext_start, ext_len);
}

String path_join_cstr(u32 count, const char* paths[]) {
    assert(paths != NULL && count > 0);

    StringBuilder sb = string_builder_create(64);
    for (u32 i = 0; i < count; ++i) {
        string_builder_append_cstr(&sb, paths[i]);
        if (i + 1 < count) {
            string_builder_append_c(&sb, PATH_SEP);
        }
    }
    String result = string_builder_get_str(&sb);
    string_builder_destroy(&sb);
    return result;
}

String path_join_str(u32 count, const String* paths[]) {
    assert(paths != NULL && count > 0);

    StringBuilder sb = string_builder_create(64);
    for (u32 i = 0; i < count; ++i) {
        string_builder_append_str(&sb, paths[i]);
        if (i + 1 < count) {
            string_builder_append_c(&sb, PATH_SEP);
        }
    }
    String result = string_builder_get_str(&sb);
    string_builder_destroy(&sb);
    return result;
}

String path_build(const String* dir, const String* name, const String* ext) {
    StringBuilder sb = string_builder_create(64);

    if (dir != NULL) {
        string_builder_append_str(&sb, dir);
    }
    if (name != NULL) {
        if (dir != NULL) {
            string_builder_append_c(&sb, PATH_SEP);
        }
        string_builder_append_str(&sb, name);
    }
    if (ext != NULL) {
        string_builder_append_str(&sb, ext);
    }

    String result = string_builder_get_str(&sb);
    string_builder_destroy(&sb);
    return result;
}

bool is_path_absolute(const String* path) {
    assert(path != NULL);

    if (is_string_empty(path)) {
        return false;
    }

#if defined(PLATFORM_WINDOWS)
    if (path->len >= 2 && is_alpha(path->text[0]) && path->text[1] == ':') {
        if (path->len >= 3 && is_path_sep(path->text[2])) {
            return true;
        }
    }
    return false;
#else
    return path->text[0] == '/';
#endif
}

static bool path_stat(const String* path, struct stat* out) {
    assert(path != NULL && out != NULL);

    if (is_string_empty(path)) {
        return false;
    }

    return stat(path->text, out) == 0;
}

bool is_path_exist(const String* path) {
    struct stat st;
    return path_stat(path, &st);
}

bool is_path_dir(const String* path) {
    struct stat st;
    return path_stat(path, &st) && S_ISDIR(st.st_mode);
}

bool is_path_file(const String* path) {
    struct stat st;
    return path_stat(path, &st) && S_ISREG(st.st_mode);
}
