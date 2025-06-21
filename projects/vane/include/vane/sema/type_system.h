#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/hashset.h"

#include "vane/sema/type.h"
#include "vane/sema/target.h"

typedef struct TypeSystem TypeSystem;

struct TypeSystem {
    TargetInfo target;

    Type builtin_types[TYPE_BUILTIN_COUNT];
    Type unresolved_type;

    Hashset pointer_types;
    Hashset array_types;
    Hashset slice_types;
    Hashset alias_types;
    Hashset function_types;
};

void type_system_init(TypeSystem* ts, TargetInfo target);

void type_system_destroy(TypeSystem* ts);