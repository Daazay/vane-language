#include <stdio.h>

#include <vane/compiler/build_options.h>
#include <vane/compiler/compiler.h>

u32 indent = 0;

static void print_indent() {
    for (u32 i = 0; i < indent; ++i) {
        printf("  ");
    }
}

static void print_package(const Package* package) {
    assert(package != NULL);

    indent++;

    print_indent();
    printf("[%.*s:%.*s]:\n",
        (i32)package->name.len, package->name.text,
        (i32)package->path->len, package->path->text
    );

    indent++;
    if (package->source_files.size > 0) {
        print_indent();
        printf("source_files:\n");

        indent++;
        HashmapIterator it = hashmap_get_it(&package->source_files);
        while (hashmap_it_next(&it)) {
            const String* path = it.key;

            print_indent();
            printf("- %.*s\n", (i32)path->len, path->text);
        }
        indent--;
    }

    if (package->subpackages.size > 0) {
        print_indent();
        printf("subpackages:\n");

        indent++;
        for (u32 i = 0; i < package->subpackages.size; ++i) {
            const Package* subpackage = vector_at(&package->subpackages, i);
            print_package(subpackage);
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

    Package* root_package = compiler_load_package(&compiler, &build_options.root_path);
    if (root_package != NULL) {
        print_package(root_package);
    }

    report_collector_print_all(&compiler.rc);

    compiler_destroy(&compiler);
    build_options_destroy(&build_options);

    return 0;
}