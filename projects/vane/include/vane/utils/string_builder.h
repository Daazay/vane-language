#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

typedef struct StringBuilder StringBuilder;

struct StringBuilder {
    char* buf;
    u64 len;
    u64 cap;
};

StringBuilder sb_create(u64 init_cap);

void sb_destroy(StringBuilder* sb);

void sb_resize(StringBuilder* sb, u64 new_cap);

void sb_append_left_c(StringBuilder* sb, char c);

void sb_append_left_cstr(StringBuilder* sb, const char* cstr);

void sb_append_left_str(StringBuilder* sb, const String* s);

void sb_append_left_format_va(StringBuilder* sb, const char* format, va_list va);

void sb_append_left_format(StringBuilder* sb, const char* format, ...);

void sb_append_right_c(StringBuilder* sb, char c);

void sb_append_right_cstr(StringBuilder* sb, const char* cstr);

void sb_append_right_str(StringBuilder* sb, const String* s);

void sb_append_right_format_va(StringBuilder* sb, const char* format, va_list va);

void sb_append_right_format(StringBuilder* sb, const char* format, ...);

String sb_get_str(const StringBuilder* sb);

//

#define sb_append_c(SB, C) sb_append_right_c(SB, C)

#define sb_append_cstr(SB, CSTR) sb_append_right_cstr(SB, CSTR)

#define sb_append_str(SB, S) sb_append_right_str(SB, S)

#define sb_append_format_va(SB, FORMAT, VA) sb_append_right_format_va(SB, FORMAT, VA)

#define sb_append_format(SB, FORMAT, ...) sb_append_right_format(SB, FORMAT, __VA_ARGS__)