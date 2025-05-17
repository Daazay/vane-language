#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

#define SOURCE_POS_INVALID U32_MAX

typedef struct SourcePos SourcePos;
typedef struct SourceLoc SourceLoc;

struct SourcePos {
    u32 line;
    u32 column;
};

struct SourceLoc {
    const String* path;
    SourcePos begin;
    SourcePos end;
};