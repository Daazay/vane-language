#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string_builder.h"

#include "vane/ast/ast_node.h"

String ast_node_to_dot(const ASTNode* node);