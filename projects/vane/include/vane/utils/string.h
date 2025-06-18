#pragma once

#include <stdarg.h>

#include "vane/utils/defines.h"

typedef struct String String;

#define STRING_EMPTY (String) { .text = NULL, .len = 0, }

struct String {
    char* text;
    u64 len;
};

// -- Creation --

String string_create(char* text, u64 len);

String string_from_cstr(const char* cstr);

String string_from_format(const char* format, ...);

String string_from_format_va(const char* format, va_list va);

String string_clone(const String* s);

void string_destroy(String* s);

// -- Utilities --

bool is_string_empty(const String* s);

String string_substr(const String* s, u64 offset, u64 len);

String string_concat(const String* s1, const String* s2);

// -- Replacment --

void string_replace_c(String* s, char find, char replace);

void string_replace_cstr(String* s, const char* find, const char* replace);

void string_replace_str(String* s, const String* find, const String* replace);

// -- Comparison --

bool string_eq_cstr(const String* s, const char* cstr);

bool string_eq_str(const String* s1, const String* s2);

bool string_has_prefix_cstr(const String* s, const char* cstr);

bool string_has_prefix_str(const String* s1, const String* s2);

bool string_has_suffix_cstr(const String* s, const char* cstr);

bool string_has_suffix_str(const String* s1, const String* s2);

// -- Search --

i64 string_find_c(const String* s, char c);

i64 string_find_cstr(const String* s, const char* cstr);

i64 string_find_str(const String* s1, const String* s2);

i64 string_find_last_c(const String* s, char c);

i64 string_find_last_cstr(const String* s, const char* cstr);

i64 string_find_last_str(const String* s1, const String* s2);