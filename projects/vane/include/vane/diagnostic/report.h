#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/source_loc.h"
#include "vane/utils/vector.h"

#include "vane/diagnostic/diagnostic.h"

typedef struct Report Report;
typedef struct ReportTrace ReportTrace;

struct ReportTrace {
    DiagnosticKind kind;
    String msg;
    SourceLoc loc;
};

struct Report {
    DiagnosticKind kind;
    DiagnosticSeverity severity;
    String msg;
    SourceLoc loc;
    /* [ReportTrace, &report_trace_destroy] */
    Vector trace;
};

ReportTrace report_trace_create(DiagnosticKind kind, String msg, SourceLoc loc);

void report_trace_destroy(ReportTrace* trace);

Report* report_create(DiagnosticKind kind, DiagnosticSeverity severity, String msg, SourceLoc loc, Vector trace);

void report_destroy(Report* report);

void report_print(const Report* report, bool colored);

void report_trace_print(const Vector* trace);