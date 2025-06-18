#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/vector.h"

#include "vane/diagnostic/diagnostic.h"
#include "vane/diagnostic/source_loc.h"

typedef struct ReportCollector ReportCollector;

struct ReportCollector {
    Vector reports;

    u32 severity_count[DIAG_SEVERITY_COUNT];
    u32 kind_count[DIAG_KIND_COUNT];

    struct Report* current_report;
};

ReportCollector report_collector_create();

void report_collector_destroy(ReportCollector* rc);

void report_collector_append_trace(ReportCollector* rc, SourceLoc loc, String msg);

void report_collector_append_trace_from_format(ReportCollector* rc, SourceLoc loc, const char* format, ...);

void report_collector_append_note(ReportCollector* rc, String msg);

void report_collector_append_note_from_format(ReportCollector* rc, const char* format, ...);

void report_collector_append_report(ReportCollector* rc, DiagnosticKind kind, DiagnosticSeverity severity, SourceLoc loc, String msg);

void report_collector_append_report_from_format(ReportCollector* rc, DiagnosticKind kind, DiagnosticSeverity severity, SourceLoc loc, const char* format, ...);

void report_collector_print_all(const ReportCollector* rc, bool colored, bool debug);

#define RC_TRACE(RC, LOC, FORMAT, ...) report_collector_append_trace_from_format(RC, LOC, FORMAT, ##__VA_ARGS__)
#define RC_NOTE(RC, FORMAT, ...)       report_collector_append_note_from_format(RC, FORMAT, ##__VA_ARGS__)

#define RC_REPORT_IO(RC, SEV, PATH, FORMAT, ...)  report_collector_append_report_from_format(RC, DIAG_KIND_IO, SEV, (SourceLoc) { .path = PATH }, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_IO_DEBUG(RC, PATH, FORMAT, ...)  RC_REPORT_IO(RC, DIAG_SEVERITY_DEBUG, PATH, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_IO_INFO(RC, PATH, FORMAT, ...)  RC_REPORT_IO(RC, DIAG_SEVERITY_INFO, PATH, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_IO_WARN(RC, PATH, FORMAT, ...)  RC_REPORT_IO(RC, DIAG_SEVERITY_WARN, PATH, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_IO_ERROR(RC, PATH, FORMAT, ...) RC_REPORT_IO(RC, DIAG_SEVERITY_ERROR, PATH, FORMAT, ##__VA_ARGS__)

#define RC_REPORT_LEXICAL(RC, SEV, LOC, FORMAT, ...)  report_collector_append_report_from_format(RC, DIAG_KIND_LEXICAL, SEV, LOC, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_LEXICAL_INFO(RC, LOC, FORMAT, ...)  RC_REPORT_LEXICAL(RC, DIAG_SEVERITY_INFO, LOC, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_LEXICAL_WARN(RC, LOC, FORMAT, ...)  RC_REPORT_LEXICAL(RC, DIAG_SEVERITY_WARN, LOC, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_LEXICAL_ERROR(RC, LOC, FORMAT, ...) RC_REPORT_LEXICAL(RC, DIAG_SEVERITY_ERROR, LOC, FORMAT, ##__VA_ARGS__)

#define RC_REPORT_SYNTAX(RC, SEV, LOC, FORMAT, ...)  report_collector_append_report_from_format(RC, DIAG_KIND_SYNTAX, SEV, LOC, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_SYNTAX_INFO(RC, LOC, FORMAT, ...)  RC_REPORT_SYNTAX(RC, DIAG_SEVERITY_INFO, LOC, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_SYNTAX_WARN(RC, LOC, FORMAT, ...)  RC_REPORT_SYNTAX(RC, DIAG_SEVERITY_WARN, LOC, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_SYNTAX_ERROR(RC, LOC, FORMAT, ...) RC_REPORT_SYNTAX(RC, DIAG_SEVERITY_ERROR, LOC, FORMAT, ##__VA_ARGS__)

#define RC_REPORT_INTERNAL(RC, SEV, FORMAT, ...)  report_collector_append_report_from_format(RC, DIAG_KIND_INTERNAL, SEV, (SourceLoc){ 0 }, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_INTERNAL_INFO(RC, FORMAT, ...)  RC_REPORT_INTERNAL(RC, DIAG_SEVERITY_INFO, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_INTERNAL_WARN(RC, FORMAT, ...)  RC_REPORT_INTERNAL(RC, DIAG_SEVERITY_WARN, FORMAT, ##__VA_ARGS__)
#define RC_REPORT_INTERNAL_ERROR(RC, FORMAT, ...) RC_REPORT_INTERNAL(RC, DIAG_SEVERITY_ERROR, FORMAT, ##__VA_ARGS__)