#include "vane/diagnostic/report.h"

#include <stdlib.h>
#include <stdio.h>

#include "vane/utils/terminal.h"

#define REPORT_DEFAULT_INFOS_SIZE 4

ReportTrace report_trace_create(String msg, SourceLoc loc) {
    return (ReportTrace) {
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

Report* report_create(DiagnosticSeverity severity, String msg, SourceLoc loc, Vector trace) {
    Report* report = malloc(sizeof(Report));
    assert(report != NULL);

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

    printf("[%s]", get_diagnostic_severity_name(report->severity));

    if (colored) {
        terminal_reset_format();
    }

    printf(" %.*s", (i32)report->msg.len, report->msg.text);

    if (report->loc.path != NULL) {
        printf("at %.*s:%u:%u:%u:%u\n",
            (i32)report->loc.path->len, report->loc.path->text,
            report->loc.begin.line, report->loc.begin.column,
            report->loc.end.line, report->loc.end.column
        );
    }
    else {
        printf(" at <source>:%d:%d\n",
            (i32)report->loc.begin.line, (i32)report->loc.begin.column
        );
    }

    report_trace_print(&report->trace);
}

void report_trace_print(const Vector* trace) {
    assert(trace != NULL);

    for (u32 i = trace->size; i-- != 0;) {
        const ReportTrace* t = vector_at(trace, i);

        printf("    Cause: %.*s", (i32)t->msg.len, t->msg.text);

        if (t->loc.path != NULL) {
            printf("at %.*s:%u:%u:%u:%u\n",
                (i32)t->loc.path->len, t->loc.path->text,
                t->loc.begin.line, t->loc.begin.column,
                t->loc.end.line, t->loc.end.column
            );
        }
        else {
            printf(" at <source>:%u:%u:%u:%u\n",
                t->loc.begin.line, t->loc.begin.column,
                t->loc.end.line, t->loc.end.column
            );
        }
    }
}