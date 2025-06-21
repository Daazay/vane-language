#include "vane/sema/type_system.h"

static Type type_builtin_create(TypeBuiltinKind kind, u32 size, u32 alignment) {
    return (Type) {
        .kind = TYPE_BUILTIN,
        .size = size,
        .alignment = alignment,
        .as.builtin.kind = kind,
    };
}

static inline u64 hash_combine(u64 seed, u64 value) {
    return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

static u64 get_type_builtin_hash(const Type* type) {
    assert(type != NULL);
    return (u64)type->as.builtin.kind;
}

static u64 get_type_hash(const Type* type);

static u64 get_type_hash(const Type* type) {
    if (type == NULL) {
        return 0;
    }

    u64 hash = (u64)type->kind;

    switch (type->kind) {
    case TYPE_BUILTIN:
        return hash_combine(hash, get_type_builtin_hash(type));
    case TYPE_POINTER:
        return hash_combine(hash, get_type_hash(type->as.pointer.to));
    case TYPE_ALIAS:
        return hash_combine(hash, get_type_hash(type->as.alias.to));
    case TYPE_ARRAY:
        hash = hash_combine(hash, get_type_builtin_hash(type->as.array.element_type));
        return hash = hash_combine(hash, (u64)type->as.array.length);
    case TYPE_SLICE:
        return hash_combine(hash, get_type_builtin_hash(type->as.slice.element_type));
    case TYPE_FUNCTION:
        hash = hash_combine(hash, get_type_builtin_hash(type->as.function.return_type));
        for (u32 i = 0; i < type->as.function.params_count; ++i) {
            const Type* param_type = type->as.function.param_types[i];
            hash = hash_combine(hash, get_type_hash(param_type));
        }
        return hash;
    default:
        unreachable();
        return 0;
    }
}

void type_system_init(TypeSystem* ts, TargetInfo target) {
    assert(ts != NULL);

    ts->target = target;

    hashset_init(&ts->pointer_types,  8, HASHSET_ITEM_SPECS(Type*, &get_type_hash, &type_eq_type, &type_destroy));
    hashset_init(&ts->alias_types,    8, HASHSET_ITEM_SPECS(Type*, &get_type_hash, &type_eq_type, &type_destroy));
    hashset_init(&ts->array_types,    8, HASHSET_ITEM_SPECS(Type*, &get_type_hash, &type_eq_type, &type_destroy));
    hashset_init(&ts->slice_types,    8, HASHSET_ITEM_SPECS(Type*, &get_type_hash, &type_eq_type, &type_destroy));
    hashset_init(&ts->function_types, 8, HASHSET_ITEM_SPECS(Type*, &get_type_hash, &type_eq_type, &type_destroy));

    ts->unresolved_type = (Type) {
        .kind = TYPE_UNRESOLVED,
        .size = 0,
        .alignment = 0,
    };

    switch (target.arch) {
    case TARGET_ARCH_X86_64: {
        ts->builtin_types[TYPE_BUILTIN_U8]   = type_builtin_create(TYPE_BUILTIN_U8,  1, 1);
        ts->builtin_types[TYPE_BUILTIN_I8]   = type_builtin_create(TYPE_BUILTIN_I8,  1, 1);
        ts->builtin_types[TYPE_BUILTIN_U16]  = type_builtin_create(TYPE_BUILTIN_U16, 2, 2);
        ts->builtin_types[TYPE_BUILTIN_I16]  = type_builtin_create(TYPE_BUILTIN_I16, 2, 2);
        ts->builtin_types[TYPE_BUILTIN_U32]  = type_builtin_create(TYPE_BUILTIN_U32, 4, 4);
        ts->builtin_types[TYPE_BUILTIN_I32]  = type_builtin_create(TYPE_BUILTIN_I32, 4, 4);
        ts->builtin_types[TYPE_BUILTIN_U64]  = type_builtin_create(TYPE_BUILTIN_U64, 8, 8);
        ts->builtin_types[TYPE_BUILTIN_I64]  = type_builtin_create(TYPE_BUILTIN_I64, 8, 8);
        ts->builtin_types[TYPE_BUILTIN_VOID] = type_builtin_create(TYPE_BUILTIN_VOID, 0, 1);
        // `any` is dynamic type. size = sizeof (tag) + sizeof (pointer to payload)
        // for now just align to 2 pointer size
        ts->builtin_types[TYPE_BUILTIN_ANY] = type_builtin_create(TYPE_BUILTIN_ANY, target.pointer_size * 2, target.pointer_size);

    } break;
    default:
        unreachable();
        break;
    }
}

void type_system_destroy(TypeSystem* ts) {
    if (ts == NULL) {
        return;
    }

    hashset_destroy(&ts->pointer_types);
    hashset_destroy(&ts->alias_types);
    hashset_destroy(&ts->array_types);
    hashset_destroy(&ts->slice_types);
    hashset_destroy(&ts->function_types);
}