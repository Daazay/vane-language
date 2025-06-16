#include "vane/utils/path.h"

#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#endif

#include "vane/utils/vector.h"
#include "vane/utils/string_builder.h"
#include "vane/utils/string_utils.h"

String get_absolute_path(const String* path) {
    assert(path != NULL);

    if (string_is_empty(path)) {
        return STRING_EMPTY;
    }
    if (is_path_absolute(path)) {
        return string_clone(path);
    }

#if defined(PLATFORM_WINDOWS)
    u64 len = GetFullPathNameA(path->text, 0, NULL, NULL);
    if (len == 0) {
        return STRING_EMPTY;
    }

    char* text = malloc(len);
    assert(text != NULL);

    GetFullPathNameA(path->text, (DWORD)len, text, NULL);

    // len - 1 becouse GetFullPathNameA return len + 1 for null terminator
    return (String) { .text = text, .len = len - 1 };
#else
    char cwd[1024] = { 0 };
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return STRING_EMPTY;
    }

    u64 cwd_len = strlen(cwd);

    char* text = malloc(cwd_len + path->len + 2);
    assert(text != NULL);

    memcpy(text, cwd, cwd_len);
    text[cwd_len] = PATH_SEP;
    memcpy(text + cwd_len + 1, path->text, path->len + 1);

    String tmp = (String){ .text = text, .len = cwd_len + path->len + 1 };
    String norm = get_normalized_path(&tmp);

    free(text);

    return norm;
#endif
}

String get_normalized_path(const String* path) {
    assert(path != NULL);

    if (string_is_empty(path)) {
        return STRING_EMPTY;
    }

    Vector components = vector_create(4, VECTOR_ITEM_SPECS(String, NULL));

    for (u64 i = 0; i < path->len; ++i) {
        if (is_path_sep(path->text[i])) {
            continue;
        }

        const u64 prev_path_sep = i;
        while (i < path->len && !is_path_sep(path->text[i])) {
            ++i;
        }

        String component = (String){
            .text = path->text + prev_path_sep,
            .len = i - prev_path_sep,
        };

        // skip thing like '.'
        if (string_eq_cstr(&component, ".")) {
            continue;
        }
        // remove previous component if current is '..'
        else if (string_eq_cstr(&component, "..") && (components.size > 0)) {
            const String* last = vector_at_back(&components);
            if (!string_eq_cstr(last, "..")) {
                vector_remove(&components, components.size - 1);
                continue;
            }
        }

        vector_push_back(&components, &component);
    }

    StringBuilder sb = sb_create(32);

#if defined(PLATFORM_LINUX)
    if (is_path_absolute(path)) {
        sb_append_c(&sb, PATH_SEP);
    }
#endif

    for (u32 i = 0; i < components.size; ++i) {
        const String* s = vector_at(&components, i);
        sb_append_str(&sb, s);

        if (i + 1 < components.size) {
            sb_append_c(&sb, PATH_SEP);
        }
    }

    String normalized = sb_get_str(&sb);

    sb_destroy(&sb);
    vector_destroy(&components);

    return normalized;
}

String get_current_working_dir() {
#if defined PLATFORM_WINDOWS
    char buf[MAX_PATH] = { 0 };
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

static Vector split_path_into_components(const String* p) {
    assert(p != NULL);

    if (string_is_empty(p)) {
        return (Vector) { 0 };
    }

    Vector components = vector_create(8, VECTOR_ITEM_SPECS(String, NULL));

    u64 start = 0;
    for (u64 i = 0; i <= p->len; ++i) {
        if (i == p->len || is_path_sep(p->text[i])) {
            if (i > start) {
                String part = (String){ .text = p->text + start, i - start };
                vector_push_back(&components, &part);
            }
            start = i + 1;
        }
    }

    return components;
}

String get_relative_path(const String* from, const String* to) {
    assert(from != NULL && to != NULL);
    assert(is_path_absolute(from) && is_path_absolute(to));


    if (string_is_empty(from) || string_is_empty(to)) {
        return STRING_EMPTY;
    }

#if defined PLATFORM_WINDOWS
    if (from->text[0] != to->text[0]) {
        return STRING_EMPTY;
    }
#endif

    Vector from_components = split_path_into_components(from);
    Vector to_components = split_path_into_components(to);

    u32 common = 0;

    while (common < from_components.size && common < to_components.size) {
        const String* a = vector_at(&from_components, common);
        const String* b = vector_at(&to_components, common);

        if (!string_eq_str(a, b)) {
            break;
        }
        ++common;
    }

    StringBuilder sb = sb_create(64);

    for (u32 i = common; i < from_components.size; ++i) {
        if (sb.len > 0) {
            sb_append_c(&sb, PATH_SEP);
        }
        sb_append_cstr(&sb, "..");
    }

    for (u32 i = common; i < to_components.size; ++i) {
        if (sb.len > 0) {
            sb_append_c(&sb, PATH_SEP);
        }
        const String* comp = vector_at(&to_components, i);
        sb_append_str(&sb, comp);
    }

    vector_destroy(&from_components);
    vector_destroy(&to_components);

    if (sb.len == 0) {
        sb_destroy(&sb);
        return string_from_cstr(".");
    }

    String res = sb_get_str(&sb);
    sb_destroy(&sb);

    return res;
}

String get_path_name(const String* path) {
    assert(path != NULL);

    if (string_is_empty(path)) {
        return STRING_EMPTY;
    }

    u64 end = path->len;
    while (end > 1 && is_path_sep(path->text[end - 1])) {
        --end;
    }

    // Find last path separator before `end`
    u64 start = end;
    while (start > 0 && !is_path_sep(path->text[start - 1])) {
        --start;
    }

    return string_substr(path, start, end - start);
}

String path_join_str(u32 count, const String* paths[]) {
    assert(paths != NULL);

    if (count == 0) {
        return STRING_EMPTY;
    }

    StringBuilder sb = sb_create(32);

    for (u32 i = 0; i < count; ++i) {
        const String* p = paths[i];
        sb_append_str(&sb, p);
        if (i + 1 < count) {
            sb_append_c(&sb, PATH_SEP);
        }
    }

    String tmp = (String){ .text = sb.buf, .len = sb.len };
    String normalized = get_normalized_path(&tmp);

    sb_destroy(&sb);

    return normalized;
}

String path_join_cstr(u32 count, const char* paths[]) {
    assert(paths != NULL);

    if (count == 0) {
        return STRING_EMPTY;
    }

    StringBuilder sb = sb_create(32);

    for (u32 i = 0; i < count; ++i) {
        const char* p = paths[i];
        sb_append_cstr(&sb, p);
        if (i + 1 < count) {
            sb_append_c(&sb, PATH_SEP);
        }
    }

    String tmp = (String){ .text = sb.buf, .len = sb.len };

    String normalized = get_normalized_path(&tmp);
    sb_destroy(&sb);

    return normalized;
}

bool is_path_absolute(const String* path) {
    assert(path != NULL);

    if (string_is_empty(path)) {
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

bool is_path_exists(const String* path) {
    assert(path != NULL);

    if (string_is_empty(path)) {
        return false;
    }

#if defined(PLATFORM_WINDOWS)
    DWORD attr = GetFileAttributesA(path->text);
    return (attr != INVALID_FILE_ATTRIBUTES);
#else
    struct stat st = { 0 };
    if (stat(path->text, &st) != 0) {
        return false;
    }
    return true;
#endif
}

bool is_path_a_dir(const String* path) {
    assert(path != NULL);

    if (string_is_empty(path)) {
        return false;
    }

#if defined(PLATFORM_WINDOWS)
    DWORD attr = GetFileAttributesA(path->text);
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st = { 0 };
    if (stat(path->text, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
#endif
}

bool is_path_a_file(const String* path) {
    assert(path != NULL);

    if (string_is_empty(path)) {
        return false;
    }

#if defined(PLATFORM_WINDOWS)
    DWORD attr = GetFileAttributesA(path->text);
    return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st = { 0 };
    if (stat(path->text, &st) != 0) {
        return false;
    }
    return S_ISREG(st.st_mode);
#endif
}