#include "vane/utils/string.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#pragma region UTILITIES

#define STRING(TEXT, LEN) (String){ .text = TEXT, .len = LEN }

static void string_replace_cstr_impl(String* s, const char* find, const u64 find_len, const char* rep, const u64 rep_len) {
    char* tmp = NULL;
    char* s_ptr = s->text;

    u64 rep_count = 0;
    while ((tmp = strstr(s_ptr, find)) != NULL) {
        ++rep_count;
        s_ptr = tmp + find_len;
    }

    if (rep_count == 0) {
        return;
    }

    const u64 len = (i64)s->len + ((i64)rep_len - (i64)find_len) * rep_count;
    if (len == 0) {
        string_destroy(s);
        *s = STRING_EMPTY;
        return;
    }

    char* text = malloc(len + 1);
    assert(text != NULL);
    text[len] = '\0';

    s_ptr = s->text;
    tmp = text;

    char* ins_ptr = NULL;
    u64 orig_len = 0;
    u64 offset = 0;

    while (rep_count--) {
        ins_ptr = strstr(s_ptr, find);
        orig_len = ins_ptr - s_ptr;
        memcpy(tmp, s_ptr, orig_len);
        tmp += orig_len;
        memcpy(tmp, rep, rep_len);
        tmp += rep_len;
        s_ptr = ins_ptr + find_len;

        offset = offset + orig_len + find_len;
    }

    // append rest
    memcpy(tmp, s_ptr, s->len - offset);

    free(s->text);
    s->text = text;
    s->len = len;
}

#pragma endregion

String string_from_cstr(const char* cstr) {
    if (cstr == NULL) {
        return STRING_EMPTY;
    }

    const u64 len = strlen(cstr);
    if (len == 0) {
        return STRING_EMPTY;
    }

    char* text = malloc(len + 1);
    assert(text != NULL);

    memcpy(text, cstr, len + 1);

    return STRING(text, len);
}

String string_from_fmt_va(const char* format, va_list _va) {
    assert(format != NULL);

    va_list va;
    va_copy(va, _va);
    i32 len = vsnprintf(NULL, 0, format, va);
    va_end(va);

    assert(len >= 0);

    if (len == 0) {
        return STRING_EMPTY;
    }

    char* text = malloc((u64)len + 1);
    assert(text != NULL);

    va_copy(va, _va);
    vsnprintf(text, (u64)len + 1, format, va);
    va_end(va);

    return STRING(text, len);
}

String string_from_fmt(const char* format, ...) {
    assert(format != NULL);

    va_list va;
    va_start(va, format);
    String s = string_from_fmt_va(format, va);
    va_end(va);

    return s;
}

String string_clone(const String* s) {
    assert(s != NULL);

    if (string_is_empty(s)) {
        return STRING_EMPTY;
    }

    char* text = malloc(s->len + 1);
    assert(text != NULL);

    memcpy(text, s->text, s->len + 1);

    return STRING(text, s->len);
}

void string_destroy(String* s) {
    if (s == NULL) {
        return;
    }

    free(s->text);

    s->text = NULL;
    s->len = 0;
}

String string_substr(const String* s, u64 offset, u64 len) {
    assert(s != NULL);

    if (string_is_empty(s) || offset >= s->len || len == 0) {
        return STRING_EMPTY;
    }

    const u64 reslen = s->len > (offset + len)
        ? len
        : s->len - offset;

    char* text = malloc(reslen + 1);
    assert(text != NULL);

    memcpy(text, s->text + offset, reslen);
    text[reslen] = '\0';

    return STRING(text, reslen);
}

void string_replace_c(String* s, char find, char replace) {
    assert(s != NULL && find != '\0');

    if (string_is_empty(s)) {
        return;
    }

    if (replace == '\0') {
        const char find_cstr[] = { find };

        string_replace_cstr_impl(s, find_cstr, 1, NULL, 0);
        return;
    }

    for (u64 i = 0; i < s->len; ++i) {
        if (s->text[i] == find) {
            s->text[i] = replace;
        }
    }
}

void string_replace_cstr(String* s, const char* find, const char* replace) {
    assert(s != NULL);

    if (string_is_empty(s) || (find == NULL)) {
        return;
    }

    const u64 find_len = strlen(find);
    if (find_len == 0) {
        return;
    }

    const u64 rep_len = (replace != NULL)
        ? strlen(replace)
        : 0;

    if (find_len == 1 && rep_len == 1) {
        string_replace_c(s, find[0], replace[0]);
        return;
    }

    string_replace_cstr_impl(s, find, find_len, replace, rep_len);
}

void string_replace_str(String* s, const String* find, const String* replace) {
    assert(s != NULL && find != NULL && replace != NULL);

    if (string_is_empty(s) || string_is_empty(find)) {
        return;
    }

    if (find->len == 1 && replace->len == 1) {
        string_replace_c(s, find->text[0], replace->text[0]);
        return;
    }

    string_replace_cstr_impl(s, find->text, find->len, replace->text, replace->len);
}

bool string_is_empty(const String* s) {
    assert(s != NULL);

    return s->text == NULL && s->len == 0;
}

bool string_eq_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (string_is_empty(s) || cstr == NULL) {
        return false;
    }

    const u64 len = strlen(cstr);
    if (len == 0 || s->len != len) {
        return false;
    }

    return memcmp(s->text, cstr, len) == 0;
}

bool string_eq_str(const String* s1, const String* s2) {
    assert(s1 != NULL && s2 != NULL);

    if (string_is_empty(s1) || string_is_empty(s2) || (s1->len != s2->len)) {
        return false;
    }

    return memcmp(s1->text, s2->text, s2->len) == 0;
}

bool string_has_prefix_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (string_is_empty(s) || cstr == NULL) {
        return false;
    }

    const u64 len = strlen(cstr);
    if (len == 0 || s->len < len) {
        return false;
    }

    return memcmp(s->text, cstr, len) == 0;
}

bool string_has_prefix_str(const String* s1, const String* s2) {
    assert(s1 != NULL && s2 != NULL);

    if (string_is_empty(s1) || string_is_empty(s2) || (s1->len < s2->len)) {
        return false;
    }

    return memcmp(s1->text, s2->text, s2->len) == 0;
}

bool string_has_suffix_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (string_is_empty(s) || cstr == NULL) {
        return false;
    }

    const u64 len = strlen(cstr);
    if (len == 0 || s->len < len) {
        return false;
    }

    return memcmp(s->text + s->len - len, cstr, len) == 0;
}

bool string_has_suffix_str(const String* s1, const String* s2) {
    assert(s1 != NULL && s2 != NULL);

    if (string_is_empty(s1) || string_is_empty(s2) || (s1->len < s2->len)) {
        return false;
    }

    return memcmp(s1->text + s1->len - s2->len, s2->text, s2->len) == 0;
}

u64 string_find_c(const String* s, char c) {
    assert(s != NULL);

    if (string_is_empty(s)) {
        return STRING_NPOS;
    }

    for (u64 i = 0; i < s->len; ++i) {
        if (s->text[i] == c) {
            return i;
        }
    }

    return STRING_NPOS;
}

u64 string_find_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (string_is_empty(s) || cstr == NULL) {
        return STRING_NPOS;
    }

    const u64 len = strlen(cstr);
    if (len == 0 || s->len < len) {
        return STRING_NPOS;
    }

    for (u64 i = 0; i <= s->len - len; ++i) {
        if (s->text[i] == cstr[0]) {
            if (memcmp(s->text + i, cstr, len) == 0) {
                return i;
            }
        }
    }

    return STRING_NPOS;
}

u64 string_find_str(const String* s1, const String* s2) {
    assert(s1 != NULL && s2 != NULL);

    if (string_is_empty(s1) || string_is_empty(s2) || (s1->len < s2->len)) {
        return STRING_NPOS;
    }

    for (u64 i = 0; i <= s1->len - s2->len; ++i) {
        if (s1->text[i] == s2->text[0]) {
            if (memcmp(s1->text + i, s2->text, s2->len) == 0) {
                return i;
            }
        }
    }

    return STRING_NPOS;
}

u64 string_find_last_c(const String* s, char c) {
    assert(s != NULL);

    if (string_is_empty(s)) {
        return STRING_NPOS;
    }

    for (u64 i = s->len; i-- > 0; ) {
        if (s->text[i] == c) {
            return i;
        }
    }

    return STRING_NPOS;
}

u64 string_find_last_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (string_is_empty(s) || cstr == NULL) {
        return STRING_NPOS;
    }

    const u64 len = strlen(cstr);
    if (len == 0 || s->len < len) {
        return STRING_NPOS;
    }

    for (u64 i = s->len - len + 1; i-- > 0; ) {
        if (s->text[i] == cstr[0]) {
            if (memcmp(s->text + i, cstr, len) == 0) {
                return i;
            }
        }
    }

    return STRING_NPOS;
}

u64 string_find_last_str(const String* s1, const String* s2) {
    assert(s1 != NULL && s2 != NULL);

    if (string_is_empty(s1) || string_is_empty(s2) || (s1->len < s2->len)) {
        return STRING_NPOS;
    }

    for (u64 i = s1->len - s2->len + 1; i-- > 0; ) {
        if (s1->text[i] == s2->text[0]) {
            if (memcmp(s1->text + i, s2->text, s2->len) == 0) {
                return i;
            }
        }
    }

    return STRING_NPOS;
}