#include "vane/diagnostic/report_collector.h"

#include <stdio.h>

#define REPORT_COLLECTOR_DEFAULT_REPORTS_SIZE 4
#define REPORT_DEFAULT_TRACE_SIZE 4

ReportCollector report_collector_create(bool colored) {
    return (ReportCollector) {
        .reports = vector_create(
            REPORT_COLLECTOR_DEFAULT_REPORTS_SIZE,
            VECTOR_ITEM_SPECS(Report*, &report_destroy)
        ),
        .trace = { 0 },
        .severity_count = { 0 },
        .colored_output = colored,
    };
}

void report_collector_destroy(ReportCollector* rc) {
    if (rc == NULL) {
        return;
    }

    vector_destroy(&rc->reports);
    vector_destroy(&rc->trace);
}

void report_collector_append_report(ReportCollector* rc, DiagnosticSeverity severity, String msg, SourceLoc loc) {
    assert(rc != NULL);

    rc->severity_count[severity]++;

    Report* report = report_create(severity, msg, loc, rc->trace);
    rc->trace = (Vector){ 0 };

    vector_push_back(&rc->reports, &report);
}

void report_collector_append_report_trace(ReportCollector* rc, String msg, SourceLoc loc) {
    assert(rc != NULL);

    if (rc->trace.raw == NULL) {
        rc->trace = vector_create(
            REPORT_DEFAULT_TRACE_SIZE,
            VECTOR_ITEM_SPECS(ReportTrace, &report_trace_destroy)
        );
    }
    ReportTrace trace = report_trace_create(msg, loc);
    vector_push_back(&rc->trace, &trace);
}

void report_collector_print(const ReportCollector* rc) {
    assert(rc != NULL);

    for (u32 i = rc->reports.size; i-- != 0;) {
        const Report* report = vector_at(&rc->reports, i);
        report_print(report, rc->colored_output);
    }

    if (rc->trace.raw != NULL) {
        printf("unknown report trace:\n");
        report_trace_print(&rc->trace);
    }
}