#pragma once

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef signed char        i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;

typedef float              f32;
typedef double             f64;

typedef u8                 byte;

#define U8_MAX  (0xFFu)
#define I8_MAX  (0x7F)
#define U16_MAX (0xFFFFu)
#define I16_MAX (0x7FFF)
#define U32_MAX (0xFFFFFFFFu)
#define I32_MAX (0x7FFFFFFF)
#define U64_MAX (0xFFFFFFFFFFFFFFFFull)
#define I64_MAX (0x7FFFFFFFFFFFFFFFll)

#ifndef NULL
#define NULL ((void*)0)
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

#ifndef NPOS
#define NPOS (-1)
#endif

#ifndef assert
#include <assert.h>
#endif

#ifndef unreachable
#define unreachable() assert(false && "reached unreachable")
#endif

#define ARR(TYPE, ...) ((const TYPE[]){ __VA_ARGS__ })

#ifndef ARR_SIZE
#define ARR_SIZE(ARR) (sizeof(ARR) / sizeof((ARR)[0]))
#endif

#ifndef __CONCAT
#define __CONCAT(A, B) A##B
#endif

#ifndef CONCAT
#define CONCAT(A, B) __CONCAT(A, B)
#endif

#ifndef __ARG_COUNT
#define __ARG_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, COUNT, ...) COUNT
#endif

#ifndef ARG_COUNT
#define ARG_COUNT(...) __ARG_COUNT(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#endif