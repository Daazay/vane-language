#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"

#include "vane/diagnostic/source_loc.h"
#include "vane/diagnostic/diagnostic.h"

typedef struct Report Report;
typedef struct ReportTrace ReportTrace;

struct ReportTrace {
    SourceLoc loc;
    String msg;
};

struct ReportNote {
    String msg;
};

struct Report {
    DiagnosticKind kind;
    DiagnosticSeverity severity;
    String msg;
    String note;
    Vector traces;
    SourceLoc loc;
};

ReportTrace report_trace_create(SourceLoc loc, String msg);

void report_trace_destroy(ReportTrace* trace);

void report_trace_print(const ReportTrace* trace, bool with_loc);

Report* report_create();

void report_destroy(Report* report);

void report_print(const Report* report, bool colored);