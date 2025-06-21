#include <stdio.h>

#include <vane/compiler/build_options.h>
#include <vane/compiler/compiler.h>

#include <vane/utils/file_utils.h>
#include <vane/utils/path.h>

#include <vane/ast/visitors/ast_dot_visitor.h>

u32 indent = 0;

static void print_type(const Type* type) {
    assert(type != NULL);

    switch (type->kind) {
    case TYPE_UNRESOLVED:
        printf("unresolved");
        break;
    case TYPE_BUILTIN:
        switch (type->as.builtin.kind) {
        case TYPE_BUILTIN_VOID: printf("void"); break;
        case TYPE_BUILTIN_U8:   printf("u8"); break;
        case TYPE_BUILTIN_I8:   printf("i8"); break;
        case TYPE_BUILTIN_U16:  printf("u16"); break;
        case TYPE_BUILTIN_I16:  printf("i16"); break;
        case TYPE_BUILTIN_U32:  printf("u32"); break;
        case TYPE_BUILTIN_I32:  printf("i32"); break;
        case TYPE_BUILTIN_U64:  printf("u64"); break;
        case TYPE_BUILTIN_I64:  printf("i64"); break;
        case TYPE_BUILTIN_ANY:  printf("any"); break;
        default:
            unreachable();
            break;
        }
        break;
    case TYPE_POINTER:
        printf("*");
        print_type(type->as.pointer.to);
        break;
    case TYPE_ALIAS:
        print_type(type->as.alias.to);
        break;
    case TYPE_ARRAY:
        printf("[%d]", (i32)type->as.array.length);
        print_type(type->as.array.element_type);
        break;
    case TYPE_SLICE:
        printf("[..]");
        print_type(type->as.slice.element_type);
        break;
    case TYPE_FUNCTION:
        printf("(");
        for (u32 i = 0; i < type->as.function.params_count; ++i) {
            print_type(type->as.function.param_types[i]);
            if (i + 1 < type->as.function.params_count) {
                printf(",");
            }
        }
        printf(") -> ");
        print_type(type->as.function.return_type);
        break;
    default:
        unreachable();
        break;
    }
}

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
    printf("[scope:%s]\n", get_scope_kind_name(scope->kind));

    if (scope->symbols.size != 0) {
        indent++;
        print_indent();
        printf("[symbols]\n");

        indent++;
        HashmapIterator sym_it = hashmap_get_it(&scope->symbols);
        while (hashmap_it_next(&sym_it)) {
            const Symbol* symbol = sym_it.value;

            print_indent();
            printf("[%s:%s]",
                get_symbol_kind_name(symbol->kind),
                symbol->name.text
            );

            if (symbol->type != NULL) {
                printf(": ");
                print_type(symbol->type);
            }
            printf("\n");
        }
        indent -= 2;
    }

    if (scope->scopes.size != 0) {
        indent++;
        print_indent();
        printf("[subscopes]\n");

        indent++;
        for (u32 i = 0; i < scope->scopes.size; ++i) {
            const Scope* subscope = vector_at(&scope->scopes, i);
            print_symtable(subscope);
        }
        indent -= 2;
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

    // Load root package
    Package* root_package = compiler_load_package(&compiler, &build_options.root_path);
    if (root_package == NULL) {
        report_collector_print_all(&compiler.rc, build_options.general_options.colored_output);
        compiler_destroy(&compiler);
        return compiler.rc.severity_count[DIAG_SEVERITY_ERROR] > 0;
    }

    // Parse all source files
    {
        if (!compiler_parse_source_files(&compiler)) {
            report_collector_print_all(&compiler.rc, build_options.general_options.colored_output);
            compiler_destroy(&compiler);
            return compiler.rc.severity_count[DIAG_SEVERITY_ERROR] > 0;
        }

        if (build_options.command == BUILD_COMMAND_PARSE_AST) {
            report_collector_print_all(&compiler.rc, build_options.general_options.colored_output);
            compiler_print_ast(&compiler);
            compiler_destroy(&compiler);
            return compiler.rc.severity_count[DIAG_SEVERITY_ERROR] > 0;
        }
    }

    // Resolve imports
    {
        compiler_resolve_imports(&compiler);

        if (build_options.command == BUILD_COMMAND_SHOW_IMPORTS) {
            report_collector_print_all(&compiler.rc, build_options.general_options.colored_output);
            compiler_show_imports(&compiler);
            compiler_destroy(&compiler);
            return compiler.rc.severity_count[DIAG_SEVERITY_ERROR] > 0;
        }
    }

    // Resolve symbols
    {
        compiler_resolve_symbols(&compiler);

        // Debug: print symbol tables for all packages
        HashmapIterator package_it = hashmap_get_it(&compiler.packages);
        while (hashmap_it_next(&package_it)) {
            const Package* package = package_it.value;
            if (package != NULL) {
                print_symtable(package->scope);
            }
        }
    }

    // Resolve types
    {
        type_system_init(&compiler.type_system, target_infos[TARGET_ARCH_X86_64]);
        compiler_resolve_types(&compiler);

        // Debug: print symbol tables for all packages
        HashmapIterator package_it = hashmap_get_it(&compiler.packages);
        while (hashmap_it_next(&package_it)) {
            const Package* package = package_it.value;
            if (package != NULL) {
                print_symtable(package->scope);
            }
        }
    }

    /*
      NEXT STEPS:
      - Perform type analysis: resolve and assign types to all symbols
      - Semantic checks: verify types, detect type erros, check function signatures, etc.
      - Build control flow graphs.
      - Code genration
    */

    report_collector_print_all(&compiler.rc, build_options.general_options.colored_output);

    compiler_destroy(&compiler);

    return compiler.rc.severity_count[DIAG_SEVERITY_ERROR] > 0;
}