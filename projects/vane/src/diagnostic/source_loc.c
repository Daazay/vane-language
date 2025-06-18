#include "vane/diagnostic/source_loc.h"

SourcePos source_pos_create(u32 line, u32 column) {
    return (SourcePos) {
        .line = line,
        .column = column,
    };
}

SourceRange source_range_create(SourcePos begin, SourcePos end) {
    return (SourceRange) {
        .begin = begin,
        .end = end,
    };
}

SourceLoc source_loc_create(const String* path, SourceRange range) {
    return (SourceLoc) {
        .path = path,
        .range = range,
    };
}