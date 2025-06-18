#include <stdio.h>

#include <vane/compiler/build_options.h>
#include <vane/compiler/compiler.h>

#include <vane/ast/visitors/ast_dot_visitor.h>

static void print_ast_nodes(const Package* package) {
    assert(package != NULL);

    printf("[%.*s]:\n",
        (i32)package->name.len, package->name.text
    );

    if (package->source_files.size > 0) {
        HashmapIterator it = hashmap_get_it(&package->source_files);
        while (hashmap_it_next(&it)) {
            const SourceFile* source_file = it.value;

            if (source_file == NULL) {
                continue;
            }

            for (u32 i = 0; i < source_file->ast_nodes.size; ++i) {
                const ASTNode* ast = vector_at(&source_file->ast_nodes, i);

                ast_print_dot(ast, stdout);
            }
        }
    }

    if (package->subpackages.size > 0) {
        for (u32 i = 0; i < package->subpackages.size; ++i) {
            const Package* subpackage = vector_at(&package->subpackages, i);
            print_ast_nodes(subpackage);
        }
    }
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
        compiler_destroy(&compiler);
        build_options_destroy(&build_options);

        return 1;
    }

    if (!compiler_parse_source_files(&compiler)) {
        report_collector_print_all(&compiler.rc, build_options.colored_output, build_options.debug);

        compiler_destroy(&compiler);
        build_options_destroy(&build_options);

        return 1;
    }

    print_ast_nodes(root_package);


    // Process futher
    // Analyze
    // Sym table
    // etc


    compiler_destroy(&compiler);
    build_options_destroy(&build_options);

    return 0;
}