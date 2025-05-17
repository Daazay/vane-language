#pragma once

#include "vane/utils/defines.h"

static inline bool is_alpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static inline bool is_digit(char c) {
    return ('0' <= c && c <= '9');
}