#pragma once

#include "vane/utils/defines.h"

typedef enum TargetArchitecture TargetArchitecture;
typedef struct TargetInfo TargetInfo;

enum TargetArchitecture {
    TARGET_ARCH_X86_64,
    TARGET_ARCH_AARCH64,

    TARGETS_COUNT
};

struct TargetInfo {
    TargetArchitecture arch;
    u8 pointer_size;
    u8 int_size;
    u8 max_alignment;
};

extern TargetInfo target_infos[TARGETS_COUNT];