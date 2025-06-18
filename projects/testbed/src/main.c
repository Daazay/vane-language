#include <stdio.h>

#include "vane/utils/terminal.h"

#include "vane/diagnostic/report_collector.h"

int main(int argc, char** argv) {
    ReportCollector rc = report_collector_create(is_terminal_support_colors());

    String path = string_from_cstr("./file.vn");
    SourceLoc loc = source_loc_create(&path,
        source_range_create(
            source_pos_create(12, 5),
            source_pos_create(12, 18)
        )
    );

    RC_TRACE(&rc, loc, "Expected `identifier`, but got `.`");
    RC_TRACE(&rc, (SourceLoc) {0}, "Failed to parse `typeref`");
    RC_TRACE(&rc, (SourceLoc) {0}, "Failed to parse `typeref`");
    RC_TRACE(&rc, (SourceLoc) {0}, "Failed to parse `typeref`");
    RC_TRACE(&rc, (SourceLoc) {0}, "Failed to parse `typeref`");
    RC_TRACE(&rc, (SourceLoc) {0}, "Failed to parse `typeref`");
    RC_TRACE(&rc, (SourceLoc) {0}, "Failed to parse `typeref`");
    RC_TRACE(&rc, (SourceLoc) {0}, "Failed to parse `typeref`");
    RC_TRACE(&rc, (SourceLoc) { 0 }, "Failed to parse `func_param`");
    RC_NOTE(&rc, "Something went wrong while parsing function signature");
    RC_REPORT_SYNTAX_ERROR(&rc, loc, "Failed to parse function signature");

    RC_REPORT_IO_ERROR(&rc, &path, "Failed to open a file.");
    RC_REPORT_IO_ERROR(&rc, &path, "Failed to open a file.");
    RC_REPORT_IO_ERROR(&rc, &path, "Failed to open a file.");

    report_collector_print_all(&rc);
    report_collector_destroy(&rc);

    string_destroy(&path);
    return 0;
}