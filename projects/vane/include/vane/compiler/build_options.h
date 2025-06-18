#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/hashmap.h"

typedef enum BuildCommand BuildCommand;
typedef struct BuildOptions BuildOptions;

enum BuildCommand {
    BUILD_COMMAND_MISSING = 0,
    BUILD_COMMAND_HELP,
    BUILD_COMMAND_BUILD,
};

struct BuildOptions {
    BuildCommand command;

    String root_path;

    // k: [name, String, &string_destroy]
    // k: [path, String, &string_destroy]
    Hashmap collections;

    bool debug;
    bool colored_output;
};

void print_usage(const char* argv0);

bool build_options_init(BuildOptions* build_options);

bool build_options_parse_args(BuildOptions* options, int argc, char** argv);

void build_options_destroy(BuildOptions* options);