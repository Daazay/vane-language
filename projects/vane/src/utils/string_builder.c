#include "vane/utils/string_builder.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#pragma region UTILITIES

#define SB_DEFAULT_CAPACITY (32)
#define SB_CAPACITY_MULT    (1.5)

static void string_builder_ensure_capacity(StringBuilder* sb, u64 add_len) {
    assert(sb != NULL);

    if (sb->len + add_len > sb->cap) {
        u64 new_cap = sb->cap;
        while (sb->len + add_len > new_cap) {
            new_cap = (u64)((f64)new_cap * SB_CAPACITY_MULT);
        }
        string_builder_resize(sb, new_cap);
    }
}

static void string_builder_append_cstr_impl(StringBuilder* sb, const char* cstr, u64 len, bool left) {
    assert(sb != NULL && cstr != NULL);

    string_builder_ensure_capacity(sb, len);

    if (left) {
        memmove(sb->buf + len, sb->buf, sb->len);
        memcpy(sb->buf, cstr, len);
    }
    else {
        memcpy(sb->buf + sb->len, cstr, len);
    }

    sb->len += len;
    sb->buf[sb->len] = '\0';
}

static void string_builder_append_format_va_impl(StringBuilder* sb, const char* format, va_list _va, bool left) {
    assert(sb != NULL && format != NULL);

    va_list args_copy;
    va_copy(args_copy, _va);
    int len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    assert(len >= 0);

    if (len == 0) {
        return;
    }

    string_builder_ensure_capacity(sb, (u64)len);

    if (left) {
        memmove(sb->buf + (u64)len, sb->buf, sb->len + 1);
        vsnprintf(sb->buf, (u64)len + 1, format, _va);
    }
    else {
        vsnprintf(sb->buf + sb->len, (u64)len + 1, format, _va);
    }

    sb->len += (u64)len;
}

#pragma endregion

StringBuilder string_builder_create(u64 init_cap) {
    u64 cap = (init_cap > 0)
        ? init_cap
        : SB_DEFAULT_CAPACITY;

    char* buf = malloc(init_cap + 1);
    assert(buf != NULL);

    buf[0] = '\0';

    return (StringBuilder) {
        .buf = buf,
        .cap = cap,
        .len = 0,
    };
}

void string_builder_destroy(StringBuilder* sb) {
    if (sb == NULL || sb->buf == NULL) {
        return;
    }

    free(sb->buf);

    sb->buf = NULL;
    sb->cap = 0;
    sb->len = 0;
}

void string_builder_resize(StringBuilder* sb, u64 new_cap) {
    assert(sb != NULL && new_cap > 0);

    if (new_cap == sb->cap) {
        return;
    }

    char* buf = realloc(sb->buf, new_cap + 1);
    assert(buf != NULL);

    sb->buf = buf;
    sb->cap = new_cap;
}

void string_builder_append_left_c(StringBuilder* sb, char c) {
    assert(sb != NULL);

    string_builder_append_cstr_impl(sb, &c, 1, true);
}

void string_builder_append_left_cstr(StringBuilder* sb, const char* cstr) {
    assert(sb != NULL);

    string_builder_append_cstr_impl(sb, cstr, strlen(cstr), true);
}

void string_builder_append_left_str(StringBuilder* sb, const String* s) {
    assert(sb != NULL && s != NULL);

    if (is_string_empty(s)) {
        return;
    }

    string_builder_append_cstr_impl(sb, s->text, s->len, true);
}

void string_builder_append_left_format(StringBuilder* sb, const char* format, ...) {
    assert(sb != NULL && format != NULL);

    va_list va;
    va_start(va, format);
    string_builder_append_format_va_impl(sb, format, va, true);
    va_end(va);
}

void string_builder_append_left_format_va(StringBuilder* sb, const char* format, va_list _va) {
    assert(sb != NULL && format != NULL);

    string_builder_append_format_va_impl(sb, format, _va, true);
}

void string_builder_append_right_c(StringBuilder* sb, char c) {
    assert(sb != NULL);

    string_builder_append_cstr_impl(sb, &c, 1, false);
}

void string_builder_append_right_cstr(StringBuilder* sb, const char* cstr) {
    assert(sb != NULL);

    string_builder_append_cstr_impl(sb, cstr, strlen(cstr), false);
}

void string_builder_append_right_str(StringBuilder* sb, const String* s) {
    assert(sb != NULL && s != NULL);

    if (is_string_empty(s)) {
        return;
    }

    string_builder_append_cstr_impl(sb, s->text, s->len, false);
}

void string_builder_append_right_format(StringBuilder* sb, const char* format, ...) {
    assert(sb != NULL && format != NULL);

    va_list va;
    va_start(va, format);
    string_builder_append_format_va_impl(sb, format, va, false);
    va_end(va);
}

void string_builder_append_right_format_va(StringBuilder* sb, const char* format, va_list _va) {
    assert(sb != NULL && format != NULL);

    string_builder_append_format_va_impl(sb, format, _va, false);
}

String string_builder_get_str(const StringBuilder* sb) {
    assert(sb != NULL);

    if (sb->len == 0) {
        return STRING_EMPTY;
    }

    char* text = malloc(sb->len + 1);
    assert(text != NULL);

    memcpy(text, sb->buf, sb->len);
    text[sb->len] = '\0';

    return string_create(text, sb->len);
}