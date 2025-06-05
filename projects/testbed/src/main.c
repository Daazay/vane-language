#include <stdio.h>

#include <vane/ast/ast_parser.h>
#include <vane/ast/ast_node_utils.h>
#include <vane/utils/terminal.h>

int main() {
    String content = string_from_cstr("((10 + 14) - a++ * -5 + 24)(16 - a(c * 11 -)) % cast(a, 2)");

    ReportCollector rc = report_collector_create(is_terminal_support_colors());
    TokenStream ts = ts_create(32, NULL, (byte*)content.text, content.len, &rc);
    ASTParser ast_parser = ast_parser_create(&ts, &rc);

    ASTNode* node = ast_parser_parse_expr(&ast_parser);
    String s = ast_node_to_dot(node);

    printf("%.*s", (i32)s.len, s.text);
    string_destroy(&s);

    report_collector_print(&rc);

    ast_node_destroy(node);
    ast_parser_destroy(&ast_parser);
    ts_destroy(&ts);
    report_collector_destroy(&rc);
    string_destroy(&content);

    return 0;
}
/*
((10 + 14) - a++ - * -5 + 24)(16) % cast(a, 2)
*/