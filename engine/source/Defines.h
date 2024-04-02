#pragma once

//unsigned int types
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

//singed int types
typedef signed char         i8;
typedef signed short        i16;
typedef signed int          i32;
typedef signed long long    i64;

//floating point types
typedef float               f32;
typedef double              f64;

//bool types
typedef int                 b32;
typedef char                b8;

//define static asserts
#if defined(__clang__) || defined(__gcc__)
#   define STATIC_ASSERT _Static_assert
#else
#   define STATIC_ASSERT static_assert
#endif

//ensure all types are the expected size
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.\n");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.\n");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.\n");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.\n");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.\n");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.\n");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.\n");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.\n");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.\n");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.\n");

#define TRUE    1
#define FALSE   0

//platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define CPLATFORM_WINDOWS 1
#   ifndef _WIN64
#       error "64-bit is required by windows"
#   endif
#elif defined(__linux__) || defined(__gnu_linux__)
//linux os
#   define CPLATFORM_LINUX 1
#   if defined(__ANDROID__)
#       define CPLATFORM_ANDROID 1
#   endif
#elif defined(__unix__)
//catch anything not caught by above
#   define CPLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
//posix
#   define CPLATFORM_POSIX 1
#elif __APPLE__
//apple platforms
#   define CPLATFORM_APPLE 1
#   include <TargetConditionals.h>
#   if TARGET_IPHONE_SIMULATOR
//iOs simulator
#       define CPLATFORM_IOS 1
#       define CPLATFORM_IOS_SIMULATOR 1
#   elif TARGET_OS_IPHONE
#       define CPLATFORM_IOS 1
//iOS device
#   elif TARGET_OS_MAC
//other kinds of mac OS
#   else
#       error "Unknown Apple platform"
#   endif
#else
#   error "Unknown platform"
#endif

//export and import
#ifdef CEXPORT
// Exports
#   ifdef _MSC_VER
#       define CAPI __declspec(dllexport)
#   else
#       define CAPI __attribute__((visibility("default")))
#   endif
#else
// Imports
#   ifdef _MSC_VER
#       define CAPI __declspec(dllimport)
#   else
#       define CAPI
#   endif
#endif