#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/source_loc.h"
#include "vane/utils/file_utils.h"

#include "vane/scanner/token.h"

#include "vane/diagnostic/report_collector.h"

#define SCANNER_DEFAULT_LINE_POS   1
#define SCANNER_DEFAULT_COLUMN_POS 1

typedef struct Scanner Scanner;

struct Scanner {
    SourceLoc loc;

    String content;
    u64 pos;

    ReportCollector* rc;

    bool first_in_line;
};

Scanner scanner_create(const FileContent* fc, ReportCollector* rc);

void scanner_destroy(Scanner* scanner);

Token scanner_scan_next(Scanner* scanner);