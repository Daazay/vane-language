#include "vane/utils/file_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include "vane/utils/path.h"

IOStatus file_content_load(const String* path, String* content) {
    assert(path != NULL && content != NULL);

    if (is_string_empty(path)) {
        return IO_ERR_INVALID_PATH;
    }

    FILE* handle = NULL;

#if defined(PLATFORM_WINDOWS)
    i32 status = fopen_s(&handle, path->text, "rb");
    if (status != 0 || handle == NULL) {
#else
    handle = fopen(path->text, "rb");
    if (handle == NULL) {
#endif
        switch (errno) {
        case ENOENT: return IO_ERR_NOT_FOUND;
        case EACCES: return IO_ERR_ACCESS_DENIED;
        default:     return IO_ERR_READ_FAILED;
        }
    }

    if (fseek(handle, 0, SEEK_END) != 0) {
        fclose(handle);
        return IO_ERR_READ_FAILED;
    }

    i64 fsize = ftell(handle);
    if (fsize < 0) {
        fclose(handle);
        return IO_ERR_READ_FAILED;
    }

    rewind(handle);

    byte* data = malloc((u64)fsize + 1);
    assert(data != NULL);
    data[(u64)fsize] = '\0';

    u64 read_len = fread(content, sizeof(byte), fsize, handle);
    fclose(handle);

    if (read_len != (u64)fsize) {
        free(content);
        return IO_ERR_READ_FAILED;
    }

    content->text = (char*)data;
    content->len = (u64)fsize;

    return IO_OK;
}

IOStatus dir_walk(const String* path, dir_walk_callback_fn callback_fn, void* ctx) {
    assert(path != NULL && callback_fn != NULL);

    if (is_string_empty(path)) {
        return IO_ERR_INVALID_PATH;
    }

#if defined(PLATFORM_WINDOWS)
    String pattern = path_join_cstr(2, (const char* []) { path->text, "*" });

    WIN32_FIND_DATAA fd = { 0 };
    HANDLE h = FindFirstFileA(pattern.text, &fd);
    string_destroy(&pattern);

    if (h == INVALID_HANDLE_VALUE) {
        return IO_ERR_NOT_FOUND;
    }

    do {
        String name = string_create(fd.cFileName, strlen(fd.cFileName));
        if (string_eq_cstr(&name, ".") || string_eq_cstr(&name, "..")) {
            continue;
        }
        String fullpath = path_join_str(2, (const String*[]) { path, &name });

        DirEntry entry = {
            .path = fullpath,
            .is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
        };

        DirWalkAction action = callback_fn(&entry, ctx);
        string_destroy(&fullpath);

        if (action == DIR_WALK_STOP) {
            break;
        }
    } while (FindNextFileA(h, &fd));

    FindClose(h);
    return IO_OK;
#else
    DIR* dir = opendir(path->text);
    if (dir == NULL) {
        switch (errno) {
        case ENOENT:  return IO_ERR_NOT_FOUND;
        case ENOTDIR: return IO_ERR_NOT_DIR;
        case EACCES:  return IO_ERR_ACCESS_DENIED;
        default:      return IO_ERR_READ_FAILED;
        }
    }

    struct dirent* entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        String name = string_create(entry->d_name, strlen(entry->d_name));
        if (string_eq_cstr(&name, ".") || string_eq_cstr(&name, "..")) {
            continue;
        }
        String fullpath = path_join_str(2, (const String *[]) { path, &name });

        struct stat st;
        if (stat(fullpath.text, &st) != 0) {
            // Could not stat, ignore this entry
            string_destroy(&fullpath);
            closedir(dir);
            return IO_ERR_READ_FAILED;
        }

        DirEntry de = {
           .path = fullpath,
           .is_dir = S_ISDIR(st.st_mode),
        };

        DirWalkAction action = callback_fn(&de, ctx);
        string_destroy(&fullpath);

        if (action == DIR_WALK_STOP) {
            break;
        }
    }

    closedir(dir);
    return IO_OK;
#endif
}