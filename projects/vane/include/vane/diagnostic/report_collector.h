#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"

#include "vane/diagnostic/report.h"

typedef struct ReportCollector ReportCollector;
struct ReportCollector {
    Vector reports;
    Vector trace;
    u32 severity_count[3];
    bool colored_output;
};

ReportCollector report_collector_create(bool colored);

void report_collector_destroy(ReportCollector* rc);

void report_collector_append_report(ReportCollector* rc, DiagnosticSeverity severity, String msg, SourceLoc loc);

void report_collector_append_report_trace(ReportCollector* rc, String msg, SourceLoc loc);

void report_collector_print(const ReportCollector* rc);