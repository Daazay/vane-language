#include "vane/diagnostic/diagnostic.h"

const char* get_diagnostic_severity_name(DiagnosticSeverity severity) {
    switch (severity) {
    case DIAG_SEVERITY_INFO:  return "INFO";
    case DIAG_SEVERITY_WARN:  return "WARN";
    case DIAG_SEVERITY_ERROR: return "ERROR";
    default:
        return NULL;
    }
}