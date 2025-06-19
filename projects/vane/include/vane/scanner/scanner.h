#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/file_utils.h"

#include "vane/diagnostic/source_loc.h"
#include "vane/diagnostic/report_collector.h"

#include "vane/scanner/token.h"

typedef struct Scanner Scanner;

struct Scanner {
    SourceLoc loc;

    const String* content;
    u64 pos;

    ReportCollector* rc;

    bool first_in_line;
};

Scanner scanner_create(const String* path, const String* content, ReportCollector* rc);

void scanner_destroy(Scanner* scanner);

Token scanner_scan_next(Scanner* scanner);