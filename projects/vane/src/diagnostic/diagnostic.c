#include "vane/diagnostic/diagnostic.h"

const char* get_diagnostic_severity_name(DiagnosticSeverity severity) {
    switch (severity) {
    case DIAG_SEVERITY_NONE:  return "";
    case DIAG_SEVERITY_DEBUG: return "debug";
    case DIAG_SEVERITY_INFO:  return "info";
    case DIAG_SEVERITY_WARN:  return "warn";
    case DIAG_SEVERITY_ERROR: return "error";
    default:
        unreachable();
        return NULL;
    }
}

const char* get_diagnostic_kind_name(DiagnosticKind kind) {
    switch (kind) {
    case DIAG_KIND_NONE:     return "";
    case DIAG_KIND_INTERNAL: return "internal";
    case DIAG_KIND_LEXICAL:  return "lexical";
    case DIAG_KIND_SYNTAX:   return "syntax";
    case DIAG_KIND_SEMANTIC: return "semantic";
    default:
        unreachable();
        return NULL;
    }
}