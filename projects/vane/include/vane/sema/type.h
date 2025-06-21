#pragma once

#include "vane/utils/defines.h"

#include "vane/scanner/token_kind.h"

typedef enum TypeKind TypeKind;
typedef enum TypeBuiltinKind TypeBuiltinKind;

typedef struct Type Type;
struct TypeSystem;

enum TypeKind {
    TYPE_UNRESOLVED = 0,
    TYPE_BUILTIN,
    TYPE_POINTER,
    TYPE_ALIAS,
    TYPE_ARRAY,    // stack array with fixed size. e.g. [16]u8
    TYPE_SLICE,    // pointer + length
    TYPE_FUNCTION,

    TYPE_COUNT,
};

enum TypeBuiltinKind {
    TYPE_BUILTIN_UNKNOWN = 0,
    TYPE_BUILTIN_VOID,
    TYPE_BUILTIN_U8,
    TYPE_BUILTIN_I8,
    TYPE_BUILTIN_U16,
    TYPE_BUILTIN_I16,
    TYPE_BUILTIN_U32,
    TYPE_BUILTIN_I32,
    TYPE_BUILTIN_U64,
    TYPE_BUILTIN_I64,
    TYPE_BUILTIN_ANY,

    TYPE_BUILTIN_COUNT,
};

struct Type {
    TypeKind kind;

    u32 size;      // in bytes, target specific
    u32 alignment; // target-specific

    union {
        struct {
            TypeBuiltinKind kind;
        } builtin;
        struct {
            Type* to;
        } pointer;
        struct {
            Type* to;
        } alias;
        struct {
            Type* element_type;
            u32 length;
        } array;
        struct {
            Type* element_type;
        } slice;
        struct {
            Type* return_type;
            Type** param_types;
            u32 params_count;
        } function;
    } as;
};

TypeBuiltinKind get_type_builtin_kind_from_token_kind(TokenKind kind);

Type* type_create(TypeKind kind, u32 size, u32 alignment);

void type_destroy(Type* type);

Type* type_pointer_create(Type* to);

Type* type_alias_create(Type* to);

Type* type_array_create(Type* element_type, u32 length);

Type* type_slice_create(const struct TypeSystem* ts, Type* element_type);

Type* type_function_create(Type** param_types, u32 params_count, Type* return_type);

const Type* type_alias_unwrap(const Type* type);

bool type_eq_type(const Type* type1, const Type* type2);