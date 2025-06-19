#include <stdio.h>

#include <vane/compiler/build_options.h>
#include <vane/compiler/compiler.h>

#include <vane/utils/file_utils.h>
#include <vane/utils/path.h>

#include <vane/ast/visitors/ast_dot_visitor.h>

u32 indent = 0;

static void print_indent() {
    for (u32 i = 0; i < indent; ++i) {
        printf("  ");
    }
}

static void print_symtable(const Scope* scope) {
    if (scope == NULL) {
        return;
    }

    print_indent();
    printf("[scope:%s]\n", scope->name.text);


    if (scope->symbols.size == 0) {
        return;
    }

    indent++;
    print_indent();
    printf("[symbols]\n");

    indent++;
    HashmapIterator sym_it = hashmap_get_it(&scope->symbols);
    while (hashmap_it_next(&sym_it)) {
        const Symbol* symbol = sym_it.value;

        print_indent();
        printf("[%d:%s]\n", symbol->kind, symbol->name.text);
    }

    for (u32 i = 0; i < scope->scopes.size; ++i) {
        const Scope* subscope = vector_at(&scope->scopes, i);

        print_symtable(subscope);
    }
    indent -= 2;
}

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
        report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

        compiler_destroy(&compiler);
        return 1;
    }

    if (!compiler_parse_source_files(&compiler)) {
        report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

        compiler_destroy(&compiler);
        return 1;
    }

    if (build_options.command == BUILD_COMMAND_PARSE_AST) {
        report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

        compiler_print_ast(&compiler);

        compiler_destroy(&compiler);
        return 0;
    }

    //if (!compiler_resolve_imports(&compiler)) {
    //    report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

    //    compiler_destroy(&compiler);
    //    return 1;
    //}
    compiler_resolve_imports(&compiler);

    if (build_options.command == BUILD_COMMAND_SHOW_IMPORTS) {
        report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

        compiler_show_imports(&compiler);

        compiler_destroy(&compiler);
        return 0;
    }

    if (!compiler_resolve_identifiers(&compiler)) {
        report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

        compiler_destroy(&compiler);
        return 1;
    }

    HashmapIterator package_it = hashmap_get_it(&compiler.packages);
    while (hashmap_it_next(&package_it)) {
        const Package* package = package_it.value;
        if (package == NULL) {
            continue;
        }

        print_symtable(package->scope);
    }


    //if (!compiler_resolve_imports(&compiler)) {
    //    report_collector_print_all(&compiler.rc, build_options.general_options.colored_output, build_options.general_options.debug);

    //    compiler_destroy(&compiler);
    //    return 1;
    //}


    // Process futher
    // Analyze
    // Sym table
    // etc




    compiler_destroy(&compiler);
    return 0;
}