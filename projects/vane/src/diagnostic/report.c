#include "vane/diagnostic/report.h"

#include <stdlib.h>
#include <stdio.h>

#include "vane/utils/terminal.h"

#define REPORT_DEFAULT_INFOS_SIZE 4

ReportTrace report_trace_create(DiagnosticKind kind, String msg, SourceLoc loc) {
    return (ReportTrace) {
        .kind = kind,
        .msg = msg,
        .loc = loc,
    };
}

void report_trace_destroy(ReportTrace* trace) {
    if (trace == NULL) {
        return;
    }

    string_destroy(&trace->msg);
}

Report* report_create(DiagnosticKind kind, DiagnosticSeverity severity, String msg, SourceLoc loc, Vector trace) {
    Report* report = malloc(sizeof(Report));
    assert(report != NULL);

    report->kind = kind;
    report->severity = severity;
    report->msg = msg;
    report->loc = loc;
    report->trace = trace;

    return report;
}

void report_destroy(Report* report) {
    if (report == NULL) {
        return;
    }

    string_destroy(&report->msg);
    vector_destroy(&report->trace);
    free(report);
}

static void print_loc(DiagnosticKind kind, const SourceLoc* loc) {
    assert(loc != NULL);

    switch (kind) {
    case DIAG_KIND_INTERNAL:
        break;
    case DIAG_KIND_IO:
        printf(" - `%.*s`", (i32)loc->path->len, loc->path->text);
        break;
    case DIAG_KIND_LEX:
    case DIAG_KIND_SYNTAX:
    case DIAG_KIND_SEMANTIC:
        printf(" at %.*s:%d:%d",
            (i32)loc->path->len, loc->path->text,
            (i32)loc->begin.line, (i32)loc->begin.column
        );
        break;
    default:
        unreachable();
        break;
    }
    printf("\n");
}

void report_print(const Report* report, bool colored) {
    assert(report != NULL);

    if (colored) {
        TerminalColor font_color = TERMINAL_COLOR_WHITE;
        TerminalColor bg_color = TERMINAL_COLOR_BLACK;
        switch (report->severity) {
        case DIAG_SEVERITY_INFO:
            font_color = TERMINAL_COLOR_GREEN;
            break;
        case DIAG_SEVERITY_WARN:
            font_color = TERMINAL_COLOR_YELLOW;
            break;
        case DIAG_SEVERITY_ERROR:
            font_color = TERMINAL_COLOR_RED;
            break;
        default:
            unreachable();
            break;
        }

        terminal_set_color(font_color, bg_color);
    }

    printf("[%s:%s]",
        get_diagnostic_kind_name(report->kind),
        get_diagnostic_severity_name(report->severity)
    );

    if (colored) {
        terminal_reset_format();
    }

    printf(" %.*s", (i32)report->msg.len, report->msg.text);

    print_loc(report->kind, &report->loc);

    report_trace_print(&report->trace);
}

void report_trace_print(const Vector* trace) {
    assert(trace != NULL);

    for (u32 i = trace->size; i-- != 0;) {
        const ReportTrace* t = vector_at(trace, i);

        printf("    -> %.*s", (i32)t->msg.len, t->msg.text);

        print_loc(t->kind, &t->loc);
    }
}