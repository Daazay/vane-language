#include "vane/utils/string_builder.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#pragma region UTILITIES

#define SB_DEFAULT_CAPACITY (32)
#define SB_CAPACITY_MULT    (1.5)

static void sb_append_cstr_impl(StringBuilder* sb, const char* cstr, u64 len, bool left) {
    if (sb->len + len > sb->cap) {
        u64 new_cap = sb->cap;
        while (sb->len + len > new_cap) {
            new_cap = (u64)((f64)new_cap * SB_CAPACITY_MULT);
        }
        sb_resize(sb, new_cap);
    }

    if (left) {
        memmove(sb->buf + len, sb->buf, sb->len + 1);
        memcpy(sb->buf, cstr, len);
    }
    else {
        memcpy(sb->buf + sb->len, cstr, len + 1);
    }

    sb->len = sb->len + len;
}

static void sb_append_format_va_impl(StringBuilder* sb, const char* format, va_list _va, bool left) {
    va_list va;
    va_copy(va, _va);
    const i32 len = vsnprintf(NULL, 0, format, va);
    va_end(va);

    assert(len >= 0);

    if (len == 0) {
        return;
    }

    if (sb->len + len > sb->cap) {
        u64 new_cap = sb->cap;
        while (sb->len + len > new_cap) {
            new_cap = (u64)((f64)sb->cap * SB_CAPACITY_MULT);
        }
        sb_resize(sb, new_cap);
    }

    if (left) {
        const char tmp = sb->buf[0];
        memmove(sb->buf + (u64)len, sb->buf, sb->len + 1);

        va_copy(va, _va);
        vsnprintf(sb->buf, (u64)len + 1, format, va);
        va_end(va);

        // vsnprintf insert terminating zero
        sb->buf[(u64)len] = tmp;
    }
    else {
        va_copy(va, _va);
        vsnprintf(sb->buf + sb->len, (u64)len + 1, format, va);
        va_end(va);
    }

    sb->len = sb->len + len;
}

#pragma endregion

StringBuilder sb_create(u64 init_cap) {
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

void sb_destroy(StringBuilder* sb) {
    if (sb == NULL) {
        return;
    }

    free(sb->buf);

    sb->buf = NULL;
    sb->len = 0;
}

void sb_resize(StringBuilder* sb, u64 new_cap) {
    assert(sb != NULL && new_cap > 0);

    if (new_cap == sb->cap) {
        return;
    }

    char* buf = realloc(sb->buf, new_cap + 1);
    assert(buf != NULL);

    sb->buf = buf;
    sb->cap = new_cap;
}

void sb_append_left_c(StringBuilder* sb, char c) {
    assert(sb != NULL);

    if (sb->len == sb->cap) {
        u64 new_cap = (u64)((f64)sb->cap * SB_CAPACITY_MULT);
        sb_resize(sb, new_cap);
    }

    memmove(sb->buf + 1, sb->buf, sb->len + 1);
    sb->buf[0] = c;

    sb->len++;
}

void sb_append_left_cstr(StringBuilder* sb, const char* cstr) {
    assert(sb != NULL);

    const u64 len = strlen(cstr);
    if (len == 0) {
        return;
    }

    if (len == 1) {
        sb_append_left_c(sb, cstr[0]);
        return;
    }
    sb_append_cstr_impl(sb, cstr, len, true);
}

void sb_append_left_str(StringBuilder* sb, const String* s) {
    assert(sb != NULL && s != NULL);

    if (string_is_empty(s)) {
        return;
    }

    if (s->len == 1) {
        sb_append_left_c(sb, s->text[0]);
        return;
    }
    sb_append_cstr_impl(sb, s->text, s->len, true);
}

void sb_append_left_format_va(StringBuilder* sb, const char* format, va_list _va) {
    assert(sb != NULL && format != NULL);

    va_list va;
    va_copy(va, _va);
    sb_append_format_va_impl(sb, format, va, true);
    va_end(va);
}

void sb_append_left_format(StringBuilder* sb, const char* format, ...) {
    assert(sb != NULL && format != NULL);

    va_list va;
    va_start(va, format);
    sb_append_format_va_impl(sb, format, va, true);
    va_end(va);
}

void sb_append_right_c(StringBuilder* sb, char c) {
    assert(sb != NULL);

    if (sb->len == sb->cap) {
        u64 new_cap = (u64)((f64)sb->cap * SB_CAPACITY_MULT);
        sb_resize(sb, new_cap);
    }

    sb->buf[sb->len] = c;
    sb->len++;
    sb->buf[sb->len] = '\0';
}

void sb_append_right_cstr(StringBuilder* sb, const char* cstr) {
    assert(sb != NULL);

    const u64 len = strlen(cstr);
    if (len == 0) {
        return;
    }

    if (len == 1) {
        sb_append_right_c(sb, cstr[0]);
        return;
    }
    sb_append_cstr_impl(sb, cstr, len, false);
}

void sb_append_right_str(StringBuilder* sb, const String* s) {
    assert(sb != NULL && s != NULL);

    if (string_is_empty(s)) {
        return;
    }

    if (s->len == 1) {
        sb_append_right_c(sb, s->text[0]);
        return;
    }
    sb_append_cstr_impl(sb, s->text, s->len, false);
}

void sb_append_right_format_va(StringBuilder* sb, const char* format, va_list _va) {
    assert(sb != NULL && format != NULL);

    va_list va;
    va_copy(va, _va);
    sb_append_format_va_impl(sb, format, va, false);
    va_end(va);
}

void sb_append_right_format(StringBuilder* sb, const char* format, ...) {
    assert(sb != NULL && format != NULL);

    va_list va;
    va_start(va, format);
    sb_append_format_va_impl(sb, format, va, false);
    va_end(va);
}

String sb_get_str(const StringBuilder* sb) {
    assert(sb != NULL);

    if (sb->len == 0) {
        return STRING_EMPTY;
    }

    char* text = malloc(sb->len + 1);
    assert(text != NULL);

    memcpy(text, sb->buf, sb->len + 1);
    return (String) { .text = text, .len = sb->len };
}