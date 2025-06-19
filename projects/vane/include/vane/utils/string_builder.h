#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

typedef struct StringBuilder StringBuilder;

struct StringBuilder {
    char* buf;
    u64 len;
    u64 cap;
};

StringBuilder string_builder_create(u64 init_cap);

void string_builder_destroy(StringBuilder* sb);

void string_builder_resize(StringBuilder* sb, u64 new_cap);

void string_builder_append_left_c(StringBuilder* sb, char c);

void string_builder_append_left_cstr(StringBuilder* sb, const char* cstr);

void string_builder_append_left_str(StringBuilder* sb, const String* s);

void string_builder_append_left_format(StringBuilder* sb, const char* format, ...);

void string_builder_append_left_format_va(StringBuilder* sb, const char* format, va_list va);

void string_builder_append_right_c(StringBuilder* sb, char c);

void string_builder_append_right_cstr(StringBuilder* sb, const char* cstr);

void string_builder_append_right_str(StringBuilder* sb, const String* s);

void string_builder_append_right_format(StringBuilder* sb, const char* format, ...);

void string_builder_append_right_format_va(StringBuilder* sb, const char* format, va_list va);

String string_builder_get_str(const StringBuilder* sb);

//

#define string_builder_append_c(SB, C) string_builder_append_right_c(SB, C)

#define string_builder_append_cstr(SB, CSTR) string_builder_append_right_cstr(SB, CSTR)

#define string_builder_append_str(SB, S) string_builder_append_right_str(SB, S)

#define string_builder_append_format(SB, FORMAT, ...) string_builder_append_right_format(SB, FORMAT, ##__VA_ARGS__)

#define string_builder_append_format_va(SB, FORMAT, VA) string_builder_append_right_format_va(SB, FORMAT, VA)