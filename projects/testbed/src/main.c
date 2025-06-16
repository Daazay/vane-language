#include <stdio.h>

#include <vane/ast/ast_parser.h>
//#include <vane/ast/ast_node_utils.h>
#include <vane/utils/terminal.h>
#include <vane/ast/visitors/ast_console_printer.h>

#include <vane/cfg/cfg_builder.h>
#include <vane/cfg/visitors/cfg_dot_printer.h>

int main(int argc, char** argv) {
    String path = string_from_cstr(argv[1]);
    FileContent fc = { 0 };

    if (file_content_load(&fc, &path) != IO_STATUS_OK) {
        return 1;
    }

    ReportCollector rc = report_collector_create(is_terminal_support_colors());
    TokenStream ts = token_stream_create(32, &fc, &rc);
    ASTParser ast_parser = ast_parser_create(&ts, &rc);

    ASTNode* node = ast_parser_parse_fun_decl(&ast_parser);
    //ast_console_printer_print_node(node);
    if (node->kind != AST_NODE_ERROR) {
        CFGResult cfg_result = build_cfg_for_fun_decl(node);
        cfg_dot_printer_print(cfg_result.entry);
        cfg_result_destroy(&cfg_result);
    }

    //String s = ast_node_to_dot(node);

    //printf("%.*s", (i32)s.len, s.text);
    //string_destroy(&s);

    report_collector_print(&rc);

    ast_node_destroy(node);
    ast_parser_destroy(&ast_parser);
    token_stream_destroy(&ts);
    report_collector_destroy(&rc);
    file_content_destroy(&fc);
    string_destroy(&path);

    return 0;
}
/*
((10 + 14) - a++ * -5 + 24)(16 - a(c * 11)) % cast(a, 2)
fun main(args: []string): int
    io.println("Hello, world");
    return 0;
end
*/