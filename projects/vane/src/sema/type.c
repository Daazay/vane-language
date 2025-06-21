#include "vane/sema/type.h"

#include <stdlib.h>

#include "vane/sema/type_system.h"

TypeBuiltinKind get_type_builtin_kind_from_token_kind(TokenKind kind) {
    switch (kind) {
    case TOKEN_KEYWORD_VOID: return TYPE_BUILTIN_VOID;
    case TOKEN_KEYWORD_U8:   return TYPE_BUILTIN_U8;
    case TOKEN_KEYWORD_I8:   return TYPE_BUILTIN_I8;
    case TOKEN_KEYWORD_U16:  return TYPE_BUILTIN_U16;
    case TOKEN_KEYWORD_I16:  return TYPE_BUILTIN_I16;
    case TOKEN_KEYWORD_U32:  return TYPE_BUILTIN_U32;
    case TOKEN_KEYWORD_I32:  return TYPE_BUILTIN_I32;
    case TOKEN_KEYWORD_U64:  return TYPE_BUILTIN_U64;
    case TOKEN_KEYWORD_I64:  return TYPE_BUILTIN_I64;
    case TOKEN_KEYWORD_ANY:  return TYPE_BUILTIN_ANY;
    default:
        unreachable();
        return TYPE_BUILTIN_UNKNOWN;
    }
}

Type* type_create(TypeKind kind, u32 size, u32 alignment) {
    Type* type = malloc(sizeof(Type));
    assert(type != NULL);

    type->kind = kind;
    type->size = size;
    type->alignment = alignment;

    return type;
}

void type_destroy(Type* type) {
    if (type == NULL) {
        return;
    }

    if (type->kind == TYPE_BUILTIN) {
        return;
    }

    if (type->kind == TYPE_FUNCTION) {
        free(type->as.function.param_types);
    }

    free(type);
}

Type* type_pointer_create(Type* to) {
    assert(to != NULL);

    Type* type = type_create(TYPE_POINTER, to->size, to->alignment);
    type->as.pointer.to = to;
    return type;
}

Type* type_alias_create(Type* to) {
    assert(to != NULL);

    Type* type = type_create(TYPE_ALIAS, to->size, to->alignment);
    type->as.alias.to = to;
    return type;
}

Type* type_array_create(Type* element_type, u32 length) {
    assert(element_type != NULL);

    u32 size = element_type->size * length;

    Type* type = type_create(TYPE_ARRAY, size, element_type->alignment);
    type->as.array.element_type = element_type;
    type->as.array.length = length;
    return type;
}

Type* type_slice_create(const TypeSystem* ts, Type* element_type) {
    assert(ts != NULL && element_type != NULL);

    // pointer + length
    u32 size = ts->target.pointer_size * 2;

    Type* type = type_create(TYPE_SLICE, size, ts->target.pointer_size);
    type->as.slice.element_type = element_type;
    return type;
}

Type* type_function_create(Type** param_types, u32 params_count, Type* return_type) {
    assert(return_type != NULL);

    Type* type = type_create(TYPE_FUNCTION, 0, 0);
    type->as.function.return_type = return_type;
    type->as.function.param_types = param_types;
    type->as.function.params_count = params_count;
    return type;
}

const Type* type_alias_unwrap(const Type* type) {
    assert(type != NULL);

    while (type != NULL && type->kind == TYPE_ALIAS) {
        type = type->as.alias.to;
    }

    return type;
}

bool type_eq_type(const Type* type1, const Type* type2) {
    if (type1 == NULL || type2 == NULL) {
        return false;
    }

    const Type* t1 = type_alias_unwrap(type1);
    const Type* t2 = type_alias_unwrap(type2);

    if (t1->kind != t2->kind) {
        return false;
    }

    switch (t1->kind) {
    case TYPE_UNRESOLVED:
        return true;

    case TYPE_BUILTIN:
        return t1->as.builtin.kind == t2->as.builtin.kind;

    case TYPE_POINTER:
        return type_eq_type(t1->as.pointer.to, t2->as.pointer.to);

    case TYPE_ARRAY:
        return t1->as.array.length == t2->as.array.length &&
            type_eq_type(t1->as.array.element_type, t2->as.array.element_type);

    case TYPE_SLICE:
        return type_eq_type(t1->as.slice.element_type, t2->as.slice.element_type);

    case TYPE_FUNCTION:
        const Type* ret1 = t1->as.function.return_type;
        const Type* ret2 = t2->as.function.return_type;

        if (!type_eq_type(ret1, ret2)) {
            return false;
        }

        if (t1->as.function.params_count != t2->as.function.params_count) {
            return false;
        }

        for (u32 i = 0; i < t1->as.function.params_count; ++i) {
            const Type* p1 = t1->as.function.param_types[i];
            const Type* p2 = t2->as.function.param_types[i];

            if (!type_eq_type(p1, p2)) {
                return false;
            }
        }

        return true;
    default:
        unreachable();
        return false;
    }
}