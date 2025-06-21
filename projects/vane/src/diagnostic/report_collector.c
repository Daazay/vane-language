#include "vane/diagnostic/report_collector.h"

#include <stdarg.h>

#define REPORT_COLLECTOR_DEFAULT_REPORTS_SIZE (4)

#include "vane/diagnostic/report.h"

ReportCollector report_collector_create() {
    ReportCollector rc = { 0 };

    rc.reports = vector_create(
        REPORT_COLLECTOR_DEFAULT_REPORTS_SIZE,
        VECTOR_ITEM_SPECS(Report*, &report_destroy)
    );

    rc.current_report = report_create();
    vector_push_back(&rc.reports, &rc.current_report);

    return rc;
}

void report_collector_destroy(ReportCollector* rc) {
    if (rc == NULL) {
        return;
    }

    vector_destroy(&rc->reports);
}

void report_collector_append_trace(ReportCollector* rc, SourceLoc loc, String msg) {
    assert(rc != NULL);

    ReportTrace trace = report_trace_create(loc, msg);
    vector_push_back(&rc->current_report->traces, &trace);
}

void report_collector_append_trace_from_format(ReportCollector* rc, SourceLoc loc, const char* format, ...) {
    assert(rc != NULL && format != NULL);

    va_list va;
    va_start(va, format);
    String msg = string_from_format_va(format, va);
    va_end(va);

    report_collector_append_trace(rc, loc, msg);
}

void report_collector_append_note(ReportCollector* rc, String msg) {
    assert(rc != NULL);

    assert(is_string_empty(&rc->current_report->note));

    rc->current_report->note = msg;
}

void report_collector_append_note_from_format(ReportCollector* rc, const char* format, ...) {
    assert(rc != NULL && format != NULL);

    va_list va;
    va_start(va, format);
    String msg = string_from_format_va(format, va);
    va_end(va);

    report_collector_append_note(rc, msg);
}

void report_collector_append_report(ReportCollector* rc, DiagnosticKind kind, DiagnosticSeverity severity, SourceLoc loc, String msg) {
    assert(rc != NULL);

    rc->severity_count[severity]++;
    rc->kind_count[kind]++;

    rc->current_report->kind = kind;
    rc->current_report->severity = severity;
    rc->current_report->loc = loc;
    rc->current_report->msg = msg;

    rc->current_report = report_create();
    vector_push_back(&rc->reports, &rc->current_report);
}

void report_collector_append_report_from_format(ReportCollector* rc, DiagnosticKind kind, DiagnosticSeverity severity, SourceLoc loc, const char* format, ...) {
    assert(rc != NULL && format != NULL);

    va_list va;
    va_start(va, format);
    String msg = string_from_format_va(format, va);
    va_end(va);

    report_collector_append_report(rc, kind, severity, loc, msg);
}

void report_collector_print_all(const ReportCollector* rc, bool colored) {
    assert(rc != NULL);

    for (u32 i = 0; i < rc->reports.size; ++i) {
        const Report* report = vector_at(&rc->reports, i);

        report_print(report, colored);
    }
}