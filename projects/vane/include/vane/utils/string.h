#pragma once

#include <stdarg.h>

#include "vane/utils/defines.h"

#define STRING_NPOS U64_MAX
#define STRING_EMPTY (String) { .text = NULL, .len = 0 }

typedef struct String String;
struct String {
    char* text;
    u64 len;
};

String string_from_cstr(const char* cstr);

String string_from_fmt_va(const char* format, va_list va);

String string_from_fmt(const char* format, ...);

String string_clone(const String* s);

void string_destroy(String* s);

String string_substr(const String* s, u64 offset, u64 len);

void string_replace_c(String* s, char find, char replace);

void string_replace_cstr(String* s, const char* find, const char* replace);

void string_replace_str(String* s, const String* find, const String* replace);

bool string_is_empty(const String* s);

bool string_eq_cstr(const String* s, const char* cstr);

bool string_eq_str(const String* s1, const String* s2);

bool string_has_prefix_cstr(const String* s, const char* cstr);

bool string_has_prefix_str(const String* s1, const String* s2);

bool string_has_suffix_cstr(const String* s, const char* cstr);

bool string_has_suffix_str(const String* s1, const String* s2);

u64 string_find_c(const String* s, char c);

u64 string_find_cstr(const String* s, const char* cstr);

u64 string_find_str(const String* s1, const String* s2);

u64 string_find_last_c(const String* s, char c);

u64 string_find_last_cstr(const String* s, const char* cstr);

u64 string_find_last_str(const String* s1, const String* s2);