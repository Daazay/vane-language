#pragma once

#include "vane/utils/defines.h"

typedef enum DiagnosticSeverity DiagnosticSeverity;
enum DiagnosticSeverity {
    DIAG_SEVERITY_INFO,
    DIAG_SEVERITY_WARN,
    DIAG_SEVERITY_ERROR,
};

const char* get_diagnostic_severity_name(DiagnosticSeverity severity);