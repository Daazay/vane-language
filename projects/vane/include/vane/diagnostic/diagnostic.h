#pragma once

#include "vane/utils/defines.h"

typedef enum DiagnosticSeverity DiagnosticSeverity;
typedef enum DiagnosticKind DiagnosticKind;

enum DiagnosticSeverity {
    DIAG_SEVERITY_NONE,
    DIAG_SEVERITY_DEBUG,
    DIAG_SEVERITY_INFO,
    DIAG_SEVERITY_WARN,
    DIAG_SEVERITY_ERROR,

    DIAG_SEVERITY_COUNT,
};

enum DiagnosticKind {
    DIAG_KIND_NONE = 0,
    DIAG_KIND_INTERNAL,
    DIAG_KIND_LEXICAL,
    DIAG_KIND_SYNTAX,
    DIAG_KIND_SEMANTIC,

    DIAG_KIND_COUNT,
};

const char* get_diagnostic_severity_name(DiagnosticSeverity severity);

const char* get_diagnostic_kind_name(DiagnosticKind kind);