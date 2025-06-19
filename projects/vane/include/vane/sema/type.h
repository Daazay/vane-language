#pragma once

#include "vane/utils/defines.h"

typedef enum TypeKind TypeKind;
typedef struct Type Type;

enum TypeKind {
    TYPE_BUILTIN,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_SLICE,
    TYPE_FUNCTION,
    TYPE_ALIAS,
};