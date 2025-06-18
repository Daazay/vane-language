#include "vane/utils/string.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#pragma region UTILITIES

static void string_replace_cstr_impl(String* s, const char* find, const u64 find_len, const char* rep, const u64 rep_len) {
    assert(s != NULL && find != NULL);

    char* src = s->text;

    u64 count = 0;
    for (char* p = strstr(src, find); p != NULL; p = strstr(p + find_len, find)) {
        count++;
    }

    if (count == 0) {
        return;
    }

    u64 new_len = s->len + count * (rep_len - find_len);
    if (new_len == 0) {
        string_destroy(s);
        return;
    }

    char* new_text = malloc(new_len + 1);
    assert(new_text != NULL);

    new_text[new_len] = '\0';

    char* dst = new_text;
    while (src != NULL) {
        char* pos = strstr(src, find);
        if (pos == NULL) {
            break;
        }

        u64 segment = (u64)(pos - src);
        memcpy(dst, src, segment);
        dst += segment;

        if (rep != NULL && rep_len > 0) {
            memcpy(dst, rep, rep_len);
            dst += rep_len;
        }

        src = pos + find_len;
    }

    // copy remaining
    u64 remaining = s->len - (src - s->text);
    memcpy(dst, src, remaining);

    free(s->text);
    s->text = new_text;
    s->len = new_len;
}

#pragma endregion

String string_create(char* text, u64 len) {
    return (String) {
        .text = text,
        .len = len,
    };
}

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

    memcpy(text, cstr, len);
    text[len] = '\0';

    return string_create(text, len);
}

String string_from_format(const char* format, ...) {
    assert(format != NULL);

    va_list va;
    va_start(va, format);
    String s = string_from_format_va(format, va);
    va_end(va);

    return s;
}

String string_from_format_va(const char* format, va_list _va) {
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

    text[(u64)len] = '\0';

    va_copy(va, _va);
    vsnprintf(text, (u64)len + 1, format, va);
    va_end(va);

    return string_create(text, len);
}

String string_clone(const String* s) {
    assert(s != NULL);

    if (is_string_empty(s)) {
        return STRING_EMPTY;
    }

    char* text = malloc(s->len + 1);
    assert(text != NULL);

    memcpy(text, s->text, s->len);
    text[s->len] = '\0';

    return string_create(text, s->len);
}

void string_destroy(String* s) {
    if (s == NULL || s->text == NULL) {
        return;
    }

    free(s->text);

    *s = STRING_EMPTY;
}

// Utilities

bool is_string_empty(const String* s) {
    assert(s != NULL);

    return s->text == NULL && s->len == 0;
}

String string_substr(const String* s, u64 offset, u64 len) {
    assert(s != NULL);

    if (is_string_empty(s) || offset >= s->len || len == 0) {
        return STRING_EMPTY;
    }

    const u64 reslen = s->len > (offset + len)
        ? len
        : s->len - offset;

    char* text = malloc(reslen + 1);
    assert(text != NULL);

    memcpy(text, s->text + offset, reslen);
    text[reslen] = '\0';

    return string_create(text, reslen);
}

String string_concat(const String* s1, const String* s2) {
    assert(s1 != NULL && s2 != NULL);

    if (is_string_empty(s1)) {
        return is_string_empty(s2) ? STRING_EMPTY : string_clone(s2);
    }
    else if (is_string_empty(s2)) {
        return string_clone(s1);
    }

    const u64 len = s1->len + s2->len;

    char* text = malloc(len + 1);
    assert(text != NULL);

    memcpy(text, s1->text, s1->len);
    memcpy(text + s1->len, s2->text, s2->len);
    text[len] = '\0';

    return string_create(text, len);
}

// Replacment

void string_replace_c(String* s, char find, char replace) {
    assert(s != NULL && find != '\0');

    if (is_string_empty(s)) {
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

    if (is_string_empty(s) || (find == NULL)) {
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

    if (is_string_empty(s) || is_string_empty(find)) {
        return;
    }

    if (find->len == 1 && replace->len == 1) {
        string_replace_c(s, find->text[0], replace->text[0]);
        return;
    }

    string_replace_cstr_impl(s, find->text, find->len, replace->text, replace->len);
}

// Comparison

bool string_eq_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (is_string_empty(s) || cstr == NULL) {
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

    if (is_string_empty(s1) || is_string_empty(s2) || (s1->len != s2->len)) {
        return false;
    }

    return memcmp(s1->text, s2->text, s2->len) == 0;
}

bool string_has_prefix_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (is_string_empty(s) || cstr == NULL) {
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

    if (is_string_empty(s1) || is_string_empty(s2) || (s1->len < s2->len)) {
        return false;
    }

    return memcmp(s1->text, s2->text, s2->len) == 0;
}

bool string_has_suffix_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (is_string_empty(s) || cstr == NULL) {
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

    if (is_string_empty(s1) || is_string_empty(s2) || (s1->len < s2->len)) {
        return false;
    }

    return memcmp(s1->text + s1->len - s2->len, s2->text, s2->len) == 0;
}

// Search

i64 string_find_c(const String* s, char c) {
    assert(s != NULL);

    if (is_string_empty(s)) {
        return NPOS;
    }

    for (u64 i = 0; i < s->len; ++i) {
        if (s->text[i] == c) {
            return (i64)i;
        }
    }

    return NPOS;
}

i64 string_find_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (is_string_empty(s) || cstr == NULL) {
        return NPOS;
    }

    const u64 len = strlen(cstr);
    if (len == 0 || s->len < len) {
        return NPOS;
    }

    for (u64 i = 0; i <= s->len - len; ++i) {
        if (s->text[i] == cstr[0]) {
            if (memcmp(s->text + i, cstr, len) == 0) {
                return (i64)i;
            }
        }
    }

    return NPOS;
}

i64 string_find_str(const String* s1, const String* s2) {
    assert(s1 != NULL && s2 != NULL);

    if (is_string_empty(s1) || is_string_empty(s2) || (s1->len < s2->len)) {
        return NPOS;
    }

    for (u64 i = 0; i <= s1->len - s2->len; ++i) {
        if (s1->text[i] == s2->text[0]) {
            if (memcmp(s1->text + i, s2->text, s2->len) == 0) {
                return (i64)i;
            }
        }
    }

    return NPOS;
}

i64 string_find_last_c(const String* s, char c) {
    assert(s != NULL);

    if (is_string_empty(s)) {
        return NPOS;
    }

    for (u64 i = s->len; i-- > 0; ) {
        if (s->text[i] == c) {
            return (i64)i;
        }
    }

    return NPOS;
}

i64 string_find_last_cstr(const String* s, const char* cstr) {
    assert(s != NULL);

    if (is_string_empty(s) || cstr == NULL) {
        return NPOS;
    }

    const u64 len = strlen(cstr);
    if (len == 0 || s->len < len) {
        return NPOS;
    }

    for (u64 i = s->len - len + 1; i-- > 0; ) {
        if (s->text[i] == cstr[0]) {
            if (memcmp(s->text + i, cstr, len) == 0) {
                return (i64)i;
            }
        }
    }

    return NPOS;
}

i64 string_find_last_str(const String* s1, const String* s2) {
    assert(s1 != NULL && s2 != NULL);

    if (is_string_empty(s1) || is_string_empty(s2) || (s1->len < s2->len)) {
        return NPOS;
    }

    for (u64 i = s1->len - s2->len + 1; i-- > 0; ) {
        if (s1->text[i] == s2->text[0]) {
            if (memcmp(s1->text + i, s2->text, s2->len) == 0) {
                return (i64)i;
            }
        }
    }

    return NPOS;
}