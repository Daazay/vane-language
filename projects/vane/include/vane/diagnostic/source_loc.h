#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"

typedef struct SourcePos SourcePos;
typedef struct SourceRange SourceRange;
typedef struct SourceLoc SourceLoc;

struct SourcePos {
    i32 line;
    i32 column;
};

struct SourceRange {
    SourcePos begin;
    SourcePos end;
};

struct SourceLoc {
    const String* path;
    SourceRange  range;
};

SourcePos source_pos_create(u32 line, u32 column);

SourceRange source_range_create(SourcePos begin, SourcePos end);

SourceLoc source_loc_create(const String* path, SourceRange range);