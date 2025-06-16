#include <stdio.h>

#include <vane/compiler/compiler.h>
#include <vane/compiler/build_options.h>

#include <vane/ast/visitors/ast_console_printer.h>
#include <vane/cfg/cfg_builder.h>
#include <vane/cfg/visitors/cfg_dot_printer.h>

u32 indent = 0;

void print_indent() {
    for (u32 i = 0; i < indent; ++i) {
        printf("  ");
    }
}

void print_package_hierarchy(const Package* pkg) {
    indent++;

    print_indent();
    printf("[%.*s]:\n", (i32)pkg->name.len, pkg->name.text);

    indent++;
    if (pkg->source_files.size > 0) {
        print_indent();
        printf("files:\n");

        indent++;
        HashmapIterator sf_it = { 0 };
        while (hashmap_it_next(&pkg->source_files, &sf_it)) {
            const String* path = sf_it.entry.key;

            if (path == NULL) {
                continue;
            }

            print_indent();
            printf("- %.*s\n", (i32)path->len, path->text);
        }
        indent--;
    }

    if (pkg->subpackages.size > 0) {
        print_indent();
        printf("subpackages:\n");

        //HashmapIterator sf_it = { 0 };
        //while (hashmap_it_next(&pkg->subpackages, &sf_it)) {
        //    const Package* subpkg = sf_it.entry.value;

        //    if (subpkg == NULL) {
        //        continue;
        //    }

        //    print_package_hierarchy(subpkg);
        //}

        indent++;
        for (u32 i = 0; i < pkg->subpackages.size; ++i) {
            const Package* subpkg = vector_at(&pkg->subpackages, i);
            print_package_hierarchy(subpkg);
        }
        indent--;
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

    Package* package = compiler_load_package(&compiler, &build_options.root_path);

    print_package_hierarchy(package);

    //HashmapIterator pkg_it = { 0 };
    //while (hashmap_it_next(&compiler.packages, &pkg_it)) {
    //    HashmapEntry pkg_entry = pkg_it.entry;
    //    const String* pkg_path = pkg_entry.key;
    //    const Package* pkg = pkg_entry.value;

    //    printf("pkg [%.*s]:\n", (i32)pkg->name.len, pkg->name.text);

    //    printf("    files:\n");
    //    if (pkg != NULL) {
    //        HashmapIterator sf_it = { 0 };
    //        while (hashmap_it_next(&pkg->source_files, &sf_it)) {
    //            HashmapEntry sf_entry = sf_it.entry;
    //            const String* sf_path = sf_entry.key;
    //            const SourceFile* sf = sf_entry.value;

    //            printf("    file [%.*s]:\n", (i32)sf_path->len, sf_path->text);
    //        }
    //    }
    //}

    report_collector_print(&compiler.rc);

    compiler_destroy(&compiler);
    build_options_destroy(&build_options);
    return 0;
}