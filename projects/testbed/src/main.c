#include <stdio.h>

#include <vane/ast/ast_parser.h>
//#include <vane/ast/ast_node_utils.h>
#include <vane/utils/terminal.h>
#include <vane/ast/visitors/ast_console_printer.h>

int main() {
    String content = string_from_cstr(""
        "fun main(args: []string): int\n"
        "    io.println(\"Hello, world\");\n"
        "    return 0;\n"
        "end\n"
    );
    FileContent fc = {
        .content = (byte*)content.text,
        .size = content.len,
        .path = NULL,
    };

    ReportCollector rc = report_collector_create(is_terminal_support_colors());
    TokenStream ts = token_stream_create(32, &fc, &rc);
    ASTParser ast_parser = ast_parser_create(&ts, &rc);

    ASTNode* node = ast_parser_parse_fun_decl(&ast_parser);
    ast_console_printer_print_node(node);

    //String s = ast_node_to_dot(node);

    //printf("%.*s", (i32)s.len, s.text);
    //string_destroy(&s);

    report_collector_print(&rc);

    ast_node_destroy(node);
    ast_parser_destroy(&ast_parser);
    token_stream_destroy(&ts);
    report_collector_destroy(&rc);
    string_destroy(&content);

    return 0;
}
/*
((10 + 14) - a++ * -5 + 24)(16 - a(c * 11)) % cast(a, 2)
fun main(args: []string): int
    io.println("Hello, world");
    return 0;
end
*/