#include <stdio.h>

#include <vane/compiler/build_options.h>
#include <vane/compiler/compiler.h>

#include <vane/utils/file_utils.h>
#include <vane/utils/path.h>

#include <vane/ast/visitors/ast_dot_visitor.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    BuildOptions build_options = { 0 };

    if (!build_options_parse_args(&build_options, argc, argv)) {
        return 1;
    }

    if (build_options.command == BUILD_COMMAND_HELP) {
        print_usage(argv[0]);

        build_options_destroy(&build_options);
        return 0;
    }

    Compiler compiler = compiler_create(&build_options);

    Package* root_package = compiler_load_package(&compiler, &build_options.root_path);
    if (root_package == NULL) {
        compiler_destroy(&compiler);
        build_options_destroy(&build_options);

        return 1;
    }

    if (!compiler_parse_source_files(&compiler)) {
        report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

        compiler_destroy(&compiler);
        build_options_destroy(&build_options);

        return 1;
    }

    if (build_options.command == BUILD_COMMAND_PARSE_AST) {
        compiler_print_ast(&compiler);

        compiler_destroy(&compiler);
        build_options_destroy(&build_options);

        return 0;
    }

    if (!compiler_resolve_imports(&compiler)) {
        report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

        compiler_destroy(&compiler);
        build_options_destroy(&build_options);

        return 1;
    }


    // Process futher
    // Analyze
    // Sym table
    // etc


    compiler_destroy(&compiler);
    build_options_destroy(&build_options);

    return 0;
}