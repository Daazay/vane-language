#pragma once

#include "vane/utils/defines.h"

struct String;

u32 hash_finalizer(u32 hash);

u32 get_cstr_hash(const char* cstr);

u32 get_string_hash(const struct String* s);