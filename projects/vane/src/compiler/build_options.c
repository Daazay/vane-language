#include "vane/compiler/build_options.h"

#include <stdio.h>
#include <string.h>

#include "vane/utils/hash.h"
#include "vane/utils/path.h"
#include "vane/utils/terminal.h"

#define PRINT_LINE(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#define PRINT_ERROR_LINE(msg, ...) printf("ERROR: " msg "\n", ##__VA_ARGS__)

#define VANE_VENV_NAME "VANE_LANG_ROOT"

void print_usage(const char* argv0) {
    PRINT_LINE("Usage: %s [GENERAL OPTIONS] COMMAND [ARGUMENTS] [COMMAND OPTIONS]", argv0);
    PRINT_LINE("");
    PRINT_LINE("General options:");
    PRINT_LINE("  --output_dir <PATH>       Set output directory path.");
    PRINT_LINE("  --debug                   Print debug reports.");
    PRINT_LINE("  --collection <NAME:PATH>  Adds collection path.");
    PRINT_LINE("");
    PRINT_LINE("Commands:");
    PRINT_LINE("  parse_ast <PATH>          Parse project to abstract syntax tree.");
    PRINT_LINE("  build_cfg <PATH>          Build control flow graph(s) for functions in project.");
    PRINT_LINE("  show_imports <PATH>       Prints resolved/unresolved imports for project.");
    PRINT_LINE("  help                      Prints this help.");
    PRINT_LINE("");
    PRINT_LINE("Command specific options:");
    PRINT_LINE("  parse_ast:");
    PRINT_LINE("    --save                  Save parsed ast to file.");
}

typedef struct ArgParser ArgParser;
struct ArgParser {
    BuildOptions* options;

    const u32 args_count;
    char** args;

    u32 idx;
    const char* current_arg;
};

static inline bool is_option(const char* arg) {
    return arg[0] == '-' && arg[1] == '-';
}

static inline bool match_arg(const char* arg, const char* expected) {
    return strcmp(arg, expected) == 0;
}

static inline bool has_next_arg(const ArgParser* parser) {
    return parser->idx + 1 < parser->args_count;
}

static inline bool is_next_option(const ArgParser* parser) {
    return is_option(parser->args[parser->idx + 1]);
}

static inline const char* advance_arg(ArgParser* parser) {
    return parser->args[++parser->idx];
}

static bool parse_option(ArgParser* parser) {
    const char* op = parser->current_arg + 2; // skip '--'
    switch (parser->options->command) {
    case BUILD_COMMAND_MISSING:
        // GENERAL OPTIONS
        if (match_arg(op, "output_dir")) {
            if (has_next_arg(parser) && !is_next_option(parser)) {
                if (!is_string_empty(&parser->options->general_options.output_dir)) {
                    string_destroy(&parser->options->general_options.output_dir);
                }
                const char* arg = advance_arg(parser);
                parser->options->general_options.output_dir = string_from_cstr(arg);
                return true;
            }

            PRINT_ERROR_LINE("the argument for 'collection' option was not provided.");
            return false;
        }
        else if (match_arg(op, "collection")) {
            if (has_next_arg(parser) && !is_next_option(parser)) {
                const char* arg_ = advance_arg(parser);
                String arg = string_create(arg_, strlen(arg_));

                i64 colon_pos = string_find_c(&arg, ':');
                if ((colon_pos == NPOS) || (colon_pos == 0) || ((u64)colon_pos == arg.len)) {
                    PRINT_ERROR_LINE("invalid argument for 'collection' option.");
                    return false;
                }

                String name = string_substr(&arg, 0, colon_pos);
                String path = string_substr(&arg, colon_pos + 1, arg.len - colon_pos);

                hashmap_put(&parser->options->collections, &name, &path);
                return true;
            }

            PRINT_ERROR_LINE("the argument for 'collection' option was not provided.");
            return false;
        }
        else if (match_arg(op, "debug")) {
            parser->options->general_options.debug = true;
            return true;
        }
        break;
    case BUILD_COMMAND_HELP:
        return true;
    case BUILD_COMMAND_PARSE_AST:
        if (match_arg(op, "save")) {
            parser->options->command_options.parse_ast.save_to_file = true;
            return false;
        }
        return true;
    case BUILD_COMMAND_SHOW_IMPORTS:
        return true;
    case BUILD_COMMAND_BUILD_CFG:
        return true;
    default:
        break;
    }

    PRINT_ERROR_LINE("found an unknown option '%s'.", parser->current_arg);
    return false;
}

static bool parse_command_args(ArgParser* parser) {
    switch (parser->options->command) {
    case BUILD_COMMAND_MISSING:
        unreachable();
        break;
    case BUILD_COMMAND_HELP:
        break;
    case BUILD_COMMAND_PARSE_AST:
    case BUILD_COMMAND_SHOW_IMPORTS:
    case BUILD_COMMAND_BUILD_CFG:
        if (!has_next_arg(parser) || is_next_option(parser)) {
            PRINT_ERROR_LINE("no path povided.");
            return false;
        }

        const char* arg = advance_arg(parser);
        parser->options->root_path = string_from_cstr(arg);
    break;
    default:
        break;
    }
    return true;
}

static bool parse_command(ArgParser* parser) {
    if (match_arg(parser->current_arg, "help")) {
        parser->options->command = BUILD_COMMAND_HELP;
    }
    else if (match_arg(parser->current_arg, "parse_ast")) {
        parser->options->command = BUILD_COMMAND_PARSE_AST;
    }
    else if (match_arg(parser->current_arg, "show_imports")) {
        parser->options->command = BUILD_COMMAND_SHOW_IMPORTS;
    }
    else if (match_arg(parser->current_arg, "build_cfg")) {
        parser->options->command = BUILD_COMMAND_BUILD_CFG;
    }

    if (parser->options->command == BUILD_COMMAND_MISSING) {
        PRINT_ERROR_LINE("found an unknown command '%s'.", parser->current_arg);
        return false;
    }

    return parse_command_args(parser);
}


bool build_options_init(BuildOptions* options) {
    assert(options != NULL);

    options->command = BUILD_COMMAND_MISSING;
    options->root_path = (String){ 0 };

    options->collections = hashmap_create(4,
        HASHMAP_KEY_SPECS(String, &get_string_hash, &string_eq_str, &string_destroy),
        HASHMAP_VALUE_SPECS(String, &string_destroy)
    );

    // General options
    options->general_options.output_dir = string_from_cstr("./build");
    options->general_options.debug = false;
    options->general_options.colored_output = is_terminal_support_colors();

    // Command Specific Options

    // :PARSE_AST
    options->command_options.parse_ast.save_to_file = false;

    return true;
}

bool build_options_parse_args(BuildOptions* options, int argc, char** argv) {
    assert(options != NULL);

    if (argc < 2) {
        options->command = BUILD_COMMAND_HELP;
        return true;
    }

    if (!build_options_init(options)) {
        build_options_destroy(options);
        return false;
    }

    ArgParser arg_parser = {
        .options = options,
        .args = argv,
        .args_count = argc,
        .idx = 1,
        .current_arg = NULL,
    };

    bool good = true;

    for (; arg_parser.idx < arg_parser.args_count; ++arg_parser.idx) {
        arg_parser.current_arg = arg_parser.args[arg_parser.idx];

        if (is_option(arg_parser.current_arg)) {
            if (!parse_option(&arg_parser)) {
                good = false;
                break;
            }
            continue;
        }
        else if (options->command == BUILD_COMMAND_MISSING) {
            if (!parse_command(&arg_parser)) {
                good = false;
                break;
            }
            continue;
        }

        PRINT_ERROR_LINE("found an unexpected argument '%s'.", arg_parser.current_arg);
        good = false;
        break;
    }

    if (options->command == BUILD_COMMAND_MISSING) {
        PRINT_ERROR_LINE("the command is not specified.");
        good = false;
    }

    if (!good) {
        build_options_destroy(options);
    }
    return good;
}

void build_options_destroy(BuildOptions* options) {
    if (options == NULL) return;

    string_destroy(&options->root_path);
    hashmap_destroy(&options->collections);

    // General options
    string_destroy(&options->general_options.output_dir);

    // Command specific options
}