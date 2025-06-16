#include <stdio.h>

#include <vane/compiler/compiler.h>
#include <vane/compiler/build_options.h>

#include <vane/ast/visitors/ast_console_printer.h>
#include <vane/cfg/cfg_builder.h>
#include <vane/cfg/visitors/cfg_dot_printer.h>

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

    Package* package = compiler_load_package(&compiler, &build_options.root_path);

    HashmapIterator pkg_it = {0};
    while (hashmap_it_next(&compiler.packages, &pkg_it)) {
        HashmapEntry pkg_entry = pkg_it.entry;
        const String* pkg_path = pkg_entry.key;
        const Package* pkg = pkg_entry.value;

        printf("pkg [%.*s]:\n", (i32)pkg->name.len, pkg->name.text);

        printf("    files:\n");
        if (pkg != NULL) {
            HashmapIterator sf_it = { 0 };
            while (hashmap_it_next(&pkg->source_files, &sf_it)) {
                HashmapEntry sf_entry = sf_it.entry;
                const String* sf_path = sf_entry.key;
                const SourceFile* sf = sf_entry.value;

                printf("    file [%.*s]:\n", (i32)sf_path->len, sf_path->text);
            }
        }
    }

    report_collector_print(&compiler.rc);

    compiler_destroy(&compiler);
    build_options_destroy(&build_options);
    return 0;
}