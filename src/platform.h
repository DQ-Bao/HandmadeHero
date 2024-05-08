#pragma once
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;
typedef float f32;
typedef double f64;
typedef size_t memidx;

#define local_persist static
#define global_var static
#define internal static
#define PI32 3.141592653589793f
#define PI64 3.141592653589793
#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)
#ifdef HM_DEBUG
#define Assert(x) { if (!(x)) { *(int*)0 = 0; } }
#else
#define Assert(x)
#endif
#ifdef HM_WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif
