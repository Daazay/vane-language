#pragma once

#include "vane/cfg/cfg_node.h"

void cfg_dot_printer_pre_fn(CFGNode* node, CFGNode* next, CFGNode* alt, void* ctx);

void cfg_dot_printer_print(const CFGNode* node);