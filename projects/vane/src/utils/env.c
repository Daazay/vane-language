#include "vane/utils/env.h"

#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

String get_env_variable(const char* name) {
    assert(name != NULL);

#if defined(PLATFORM_WINDOWS)
    DWORD len = GetEnvironmentVariableA(name, NULL, 0);
    if (len == 0) {
        return STRING_EMPTY;
    }

    char* text = malloc((u64)len + 1);
    assert(text != NULL);
    text[len] = '\0';

    DWORD ret = GetEnvironmentVariableA(name, text, len);
    if (ret == 0 || ret >= len) {
        free(text);
        return STRING_EMPTY;
    }

    return string_create(text, (u64)ret);
#else
    const char* env = getenv(name);
    if (env == NULL) {
        return STRING_EMPTY;
    }

    return string_create(env, strlen(env));
#endif
}