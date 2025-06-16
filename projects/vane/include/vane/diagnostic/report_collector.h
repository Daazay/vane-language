#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"

#include "vane/diagnostic/report.h"

typedef struct ReportCollector ReportCollector;

struct ReportCollector {
    Vector reports;
    Vector trace;

    u32 severity_count[DIAG_SEVERITY_COUNT];
    u32 kind_count[DIAG_KIND_COUNT];

    bool colored_output;
};

ReportCollector report_collector_create(bool colored);

void report_collector_destroy(ReportCollector* rc);

void report_collector_append_report_fmt(ReportCollector* rc, DiagnosticKind kind, DiagnosticSeverity severity, SourceLoc loc, const char* format, ...);

void report_collector_append_report_trace_fmt(ReportCollector* rc, DiagnosticKind kind, SourceLoc loc, const char* format, ...);

void report_collector_print(const ReportCollector* rc);

//

#define REPORT_COLLECTOR_REPORT_INTERNAL(RC, SEV, FMT, ...)  report_collector_append_report_fmt(RC, DIAG_KIND_INTERNAL, SEV, (SourceLoc){ 0 }, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_INTERNAL_INFO(RC, FMT, ...)  REPORT_COLLECTOR_REPORT_INTERNAL(RC, DIAG_SEVERITY_INFO, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_INTERNAL_WARN(RC, FMT, ...)  REPORT_COLLECTOR_REPORT_INTERNAL(RC, DIAG_SEVERITY_WARN, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_INTERNAL_ERROR(RC, FMT, ...) REPORT_COLLECTOR_REPORT_INTERNAL(RC, DIAG_SEVERITY_ERROR, FMT, ##__VA_ARGS__)

#define REPORT_COLLECTOR_REPORT_IO(RC, SEV, PATH, FMT, ...)  report_collector_append_report_fmt(RC, DIAG_KIND_IO, SEV, (SourceLoc){ .path = PATH, }, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_IO_INFO(RC, PATH, FMT, ...)  REPORT_COLLECTOR_REPORT_IO(RC, DIAG_SEVERITY_INFO, PATH, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_IO_WARN(RC, PATH, FMT, ...)  REPORT_COLLECTOR_REPORT_IO(RC, DIAG_SEVERITY_WARN, PATH, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_IO_ERROR(RC, PATH, FMT, ...) REPORT_COLLECTOR_REPORT_IO(RC, DIAG_SEVERITY_ERROR, PATH, FMT, ##__VA_ARGS__)

#define REPORT_COLLECTOR_REPORT_LEXICAL(RC, SEV, LOC, FMT, ...)  report_collector_append_report_fmt(RC, DIAG_KIND_LEX, SEV, LOC, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_LEXICAL_INFO(RC, LOC, FMT, ...)  REPORT_COLLECTOR_REPORT_LEXICAL(RC, DIAG_SEVERITY_INFO, LOC, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_LEXICAL_WARN(RC, LOC, FMT, ...)  REPORT_COLLECTOR_REPORT_LEXICAL(RC, DIAG_SEVERITY_WARN, LOC, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_LEXICAL_ERROR(RC, LOC, FMT, ...) REPORT_COLLECTOR_REPORT_LEXICAL(RC, DIAG_SEVERITY_ERROR, LOC, FMT, ##__VA_ARGS__)

#define REPORT_COLLECTOR_REPORT_SYNTAX(RC, SEV, LOC, FMT, ...)  report_collector_append_report_fmt(RC, DIAG_KIND_SYNTAX, SEV, LOC, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_SYNTAX_INFO(RC, LOC, FMT, ...)  REPORT_COLLECTOR_REPORT_SYNTAX(RC, DIAG_SEVERITY_INFO, LOC, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_SYNTAX_WARN(RC, LOC, FMT, ...)  REPORT_COLLECTOR_REPORT_SYNTAX(RC, DIAG_SEVERITY_WARN, LOC, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_REPORT_SYNTAX_ERROR(RC, LOC, FMT, ...) REPORT_COLLECTOR_REPORT_SYNTAX(RC, DIAG_SEVERITY_ERROR, LOC, FMT, ##__VA_ARGS__)

#define REPORT_COLLECTOR_TRACE_INTERNAL(RC, FMT, ...)    report_collector_append_report_trace_fmt(RC, DIAG_KIND_INTERNAL, (SourceLoc){ 0 }, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_TRACE_IO(RC, PATH, FMT, ...)    report_collector_append_report_trace_fmt(RC, DIAG_KIND_IO, (SourceLoc){ .path = PATH, }, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_TRACE_LEX(RC, LOC, FMT, ...)    report_collector_append_report_trace_fmt(RC, DIAG_KIND_LEX, LOC, FMT, ##__VA_ARGS__)
#define REPORT_COLLECTOR_TRACE_SYNTAX(RC, LOC, FMT, ...) report_collector_append_report_trace_fmt(RC, DIAG_KIND_SYNTAX, LOC, FMT, ##__VA_ARGS__)