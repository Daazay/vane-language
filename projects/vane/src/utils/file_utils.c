#include "vane/utils/file_utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

IOStatus file_content_load(FileContent* fc, const String* path) {
    assert(fc != NULL && path != NULL);

    if (string_is_empty(path)) {
        return IO_STATUS_ERR_INVALID_PATH;
    }

#if defined(PLATFORM_WINDOWS)
    FILE* handle = NULL;
    i32 status = fopen_s(&handle, path->text, "rb");

    if (status != 0 || handle == NULL) {
        return IO_STATUS_ERR_FILE_NOT_FOUND;
    }
#else
    struct stat st = { 0 };
    if (stat(path->text, &st) != 0) {
        return IO_STATUS_ERR_FILE_NOT_FOUND;
    }
    if (S_ISDIR(st.st_mode)) {
        return IO_STATUS_ERR_FILE_NOT_FOUND;
    }

    FILE* handle = fopen(path->text, "rb");
    if (handle == NULL) {
        switch (errno) {
        case ENOENT: return IO_STATUS_ERR_FILE_NOT_FOUND;
        default: return IO_STATUS_ERR_FILE_READ_FAILED;
        }
    }
#endif

    fseek(handle, 0, SEEK_END);
    const u64 fsize = ftell(handle);
    rewind(handle);

    if (fsize == 0) {
        fclose(handle);
        return IO_STATUS_ERR_FILE_EMPTY;
    }

    byte* content = malloc(fsize + 1);
    assert(content != NULL);

    u64 read_len = fread(content, sizeof(byte), fsize, handle);
    fclose(handle);

    if (read_len != fsize) {
        free(content);
        return IO_STATUS_ERR_FILE_READ_FAILED;
    }

    fc->path = path;
    fc->content = content;
    fc->size = fsize;

    return IO_STATUS_OK;
}

void file_content_destroy(FileContent* fc) {
    if (fc == NULL) {
        return;
    }

    free(fc->content);
    fc->content = NULL;

    fc->path = NULL;
    fc->size = 0;
}

IOStatus iterate_directory(const String* dirpath, iterate_directory_fn iterate_fn, void* data) {
    assert(dirpath != NULL && iterate_fn != NULL);

    if (string_is_empty(dirpath)) {
        return IO_STATUS_ERR_INVALID_PATH;
    }

#if defined(PLATFORM_WINDOWS)
    WIN32_FIND_DATAA find_data = { 0 };

    char search_path[MAX_PATH] = { 0 };
    snprintf(search_path, sizeof(search_path), "%s\\*", dirpath->text);

    HANDLE handle = FindFirstFileA(search_path, &find_data);
    if (handle == INVALID_HANDLE_VALUE) {
        switch (GetLastError()) {
        case ERROR_DIRECTORY:
        case ERROR_PATH_NOT_FOUND: return IO_STATUS_ERR_DIR_NOT_FOUND;
        default: return IO_STATUS_ERR_DIR_READ_FAILED;
        }
    }

    do {
        String name = (String){ .text = find_data.cFileName, .len = strlen(find_data.cFileName) };
        if (string_eq_cstr(&name, ".") || string_eq_cstr(&name, "..")) {
            continue;
        }

        bool is_dir = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        u64 size = ((u64)find_data.nFileSizeHigh << 32) | find_data.nFileSizeLow;

        if (!iterate_fn(&name, is_dir, size, data)) {
            break;
        }
    } while (FindNextFileA(handle, &find_data));

    FindClose(handle);
    return IO_STATUS_OK;
#else
    DIR* dir = opendir(dirpath->text);
    if (dir == NULL) {
        switch (errno) {
        case ENOENT: return IO_STATUS_ERR_DIR_NOT_FOUND;
        case ENOTDIR: return IO_STATUS_ERR_DIR_NOT_FOUND;
        default: return IO_STATUS_ERR_DIR_READ_FAILED;
        }
    }

    struct dirent* entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        String name = (String){ .text = entry->d_name, .len = strlen(entry->d_name) };
        if (string_eq_cstr(&name, ".") || string_eq_cstr(&name, "..")) {
            continue;
        }

        char full_path[2048] = {0};
        snprintf(full_path, sizeof(full_path), "%.*s/%s", (i32)dirpath->len, dirpath->text, entry->d_name);

        struct stat st = { 0 };
        if (stat(full_path, &st) != 0) {
            continue;
        }

        bool is_dir = S_ISDIR(st.st_mode);
        u64 size = (u64)st.st_size;

        if (!iterate_fn(&name, is_dir, size, data)) {
            break;
        }
    }

    closedir(dir);
    return IO_STATUS_OK;
#endif
}