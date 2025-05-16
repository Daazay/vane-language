#pragma once

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef char               i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;

typedef float              f32;
typedef double             f64;

typedef u8                 byte;

#define U8_MAX  (0xFF)
#define I8_MAX  (0xFF)
#define U16_MAX (0xFFFF)
#define I16_MAX (0xFFFF)
#define U32_MAX (0xFFFFFFFF)
#define I32_MAX (0xFFFFFFFF)
#define U64_MAX (0xFFFFFFFFFFFFFFFF)
#define I64_MAX (0xFFFFFFFFFFFFFFFF)

#ifndef NULL
#define NULL 0
#endif

#ifndef bool
#define bool u8
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef assert
#include <assert.h>
#endif

#ifndef unreachable
#define unreachable() assert(false && "reached unreachable")
#endif

#define IS_TYPE_PTR(TYPE) ((#TYPE)[sizeof(#TYPE) / sizeof(char) - 2] == '*')

#define ARR_SIZE(ARR) (sizeof(ARR) / sizeof(ARR[0]))