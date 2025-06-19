#include "vane/diagnostic/report.h"

#include <stdlib.h>
#include <stdio.h>

#include "vane/utils/terminal.h"

#define REPORT_DEFAULT_TRACES_SIZE 1
#define REPORT_TRACES_PRINT_THESHOLD 7
#define REPORT_TRACES_PRINT_HEAD_COUNT 2
#define REPORT_TRACES_PRINT_TAIL_COUNT 2

static void print_at_loc(const SourceLoc* loc) {
    assert(loc != NULL);

    if (loc->path == NULL) {
        return;
    }

    printf(" at %.*s", (i32)loc->path->len, loc->path->text);

    if (loc->range.begin.line > 0) {
        printf(":%d", loc->range.begin.line);
    }
    if (loc->range.begin.column > 0) {
        printf(":%d", loc->range.begin.column);
    }
}

ReportTrace report_trace_create(SourceLoc loc, String msg) {
    return (ReportTrace) {
        .loc = loc,
        .msg = msg,
    };
}

void report_trace_destroy(ReportTrace* trace) {
    if (trace == NULL) {
        return;
    }

    string_destroy(&trace->msg);
}

void report_trace_print(const ReportTrace* trace, bool with_loc) {
    assert(trace != NULL);

    printf("%.*s", (i32)trace->msg.len, trace->msg.text);

    if (with_loc) {
        print_at_loc(&trace->loc);
    }
    printf("\n");
}

Report* report_create() {
    Report* report = malloc(sizeof(Report));
    assert(report != NULL);

    report->kind     = DIAG_KIND_NONE;
    report->severity = DIAG_SEVERITY_NONE;

    report->msg      = STRING_EMPTY;
    report->note     = STRING_EMPTY;
    report->traces   = vector_create(
        REPORT_DEFAULT_TRACES_SIZE,
        VECTOR_ITEM_SPECS(ReportTrace, &report_trace_destroy)
    );

    report->loc.path = NULL;
    report->loc.range.begin.line   = 0;
    report->loc.range.begin.column = 0;
    report->loc.range.end = report->loc.range.begin;

    return report;
}

void report_destroy(Report* report) {
    if (report == NULL) {
        return;
    }

    string_destroy(&report->msg);
    string_destroy(&report->note);
    vector_destroy(&report->traces);

    free(report);
}

static void report_header_print(const Report* report, bool colored) {
    assert(report != NULL);

    if (colored) {
        TerminalColor font_color = TERMINAL_COLOR_WHITE;
        TerminalColor bg_color = TERMINAL_COLOR_BLACK;

        switch (report->severity) {
        case DIAG_SEVERITY_NONE: break;
        case DIAG_SEVERITY_DEBUG:
            font_color = TERMINAL_COLOR_BLUE;
            break;
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

    bool print_semicolon = false;
    if (report->severity != DIAG_SEVERITY_NONE) {
        printf("%s", get_diagnostic_severity_name(report->severity));
        print_semicolon = true;
    }

    if (colored) {
        terminal_reset_format();
    }

    if (report->kind != DIAG_KIND_NONE) {
        printf("[%s]", get_diagnostic_kind_name(report->kind));
        print_semicolon = true;
    }

    if (print_semicolon) {
        printf(": ");
    }

    printf("%.*s", (i32)report->msg.len, report->msg.text);

    print_at_loc(&report->loc);

    printf("\n");
}

static void report_traces_print(const Report* report, bool colored) {
    assert(report != NULL);

    (void)colored;

    if (report->traces.size == 0) {
        return;
    }

    printf("caused by:\n");

    u32 total = report->traces.size;

    if (total <= REPORT_TRACES_PRINT_THESHOLD) {
        for (u32 i = report->traces.size; i-- > 0; ) {
            const ReportTrace* t = vector_at(&report->traces, i);
            printf("    -> ");
            report_trace_print(t, i == 0);
        }
    }
    else {
        // print first few
        for (u32 i = total; i-- > total - REPORT_TRACES_PRINT_HEAD_COUNT; ) {
            const ReportTrace* t = vector_at(&report->traces, i);
            printf("    -> ");
            report_trace_print(t, false);
        }

        u32 hidden_count = total - (REPORT_TRACES_PRINT_HEAD_COUNT + REPORT_TRACES_PRINT_TAIL_COUNT);
        if (hidden_count > 0) {
            printf("    ...\n    [%u hidden trace%s]\n    ...\n", hidden_count, (hidden_count == 1) ? "" : "s");
        }

        for (u32 i = REPORT_TRACES_PRINT_TAIL_COUNT; i-- > 0; ) {
            const ReportTrace* t = vector_at(&report->traces, i);
            printf("    -> ");
            report_trace_print(t, i == 0);
        }
    }
}

static void report_note_print(const Report* report, bool colored) {
    assert(report != NULL);

    (void)colored;

    if (is_string_empty(&report->note)) {
        return;
    }

    printf("note: %.*s\n", (i32)report->note.len, report->note.text);
}

void report_print(const Report* report, bool colored) {
    assert(report != NULL);

    report_header_print(report, colored);

    report_traces_print(report, colored);

    report_note_print(report, colored);

    printf("\n");
}