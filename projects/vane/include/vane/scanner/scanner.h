#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/source_loc.h"

#include "vane/scanner/token.h"

#define SCANNER_DEFAULT_LINE_POS   1
#define SCANNER_DEFAULT_COLUMN_POS 1

typedef struct Scanner Scanner;
struct Scanner {
    String content;
    u64 pos;

    SourceLoc loc;
};

Scanner scanner_create(const String* path, byte* data, u64 size);

void scanner_destroy(Scanner* scanner);

Token scanner_scan_next(Scanner* scanner);