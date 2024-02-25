#pragma once

// Tip: This file is included by all examples to define common macros and includes.
#include <stdint.h>
#include <stddef.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef bool b8;
typedef int32_t b32;
static_assert(sizeof(b8) == sizeof(char), "b8 and char must be the same size");

typedef u32 ut;
typedef i32 it;

typedef size_t usize;

typedef uintptr_t uptr;
typedef intptr_t iptr;

typedef char c;
typedef char8_t c8;
typedef char16_t c16;
typedef char32_t c32;

typedef const char* cstr;
typedef const char8_t* cstr8;
typedef const char16_t* cstr16;
typedef const char32_t* cstr32;

typedef char* str;
typedef char8_t* str8;
typedef char16_t* str16;
typedef char32_t* str32;

#include <stdio.h>
#include <stdlib.h>

#define EMPTY

#if _DEBUG || NDEBUG
#define ON_DEBUG(x) x
#else
	#define ON_DEBUG(x)
#endif

#if defined(_MSC_VER)
#define MSVC_COMPILER 1
#elif defined(__GNUC__)
    #define GCC_COMPILER 1
#elif defined(__clang__)
    #define CLANG_COMPILER 1
#else
    #error "Unknown compiler"
#endif

#if MSVC_COMPILER
#define FORCE_INLINE __forceinline
#define DEBUG_BREAK  __debugbreak()
#define EXPORTED_SYMBOL __declspec(dllexport)
#elif GCC_COMPILER || CLANG_COMPILER
    #define FORCE_INLINE __attribute__((always_inline))
    #define DEBUG_BREAK  __builtin_trap()
    #define EXPORTED_SYMBOL __attribute__((visibility("default")))
#else
	#error "Unknown compiler"
#endif

#define _LOG_(err_str, file, line, msg)                                                  \
	fprintf (stderr, "\n" err_str " in \'%s\':%d :: %s", file, line, msg)

#define LOG_FATAL(msg)                                                                   \
	{                                                                                    \
		_LOG_ ("[FATAL]", __FILE__, __LINE__, msg), ON_DEBUG (DEBUG_BREAK);              \
		exit (EXIT_FAILURE);                                                             \
	}

#define LOG_ERROR_NO_BREAK(msg) _LOG_ ("[ERROR]", __FILE__, __LINE__, msg)

#define LOG_ERROR(msg) LOG_ERROR_NO_BREAK(msg), ON_DEBUG (DEBUG_BREAK)
#define LOG_WARN(msg)  _LOG_ ("[WARN]", __FILE__, __LINE__, msg)
#define LOG_INFO(msg)  _LOG_ ("[INFO]", __FILE__, __LINE__, msg)
#define LOG_DEBUG(msg) _LOG_ ("[DEBUG]", __FILE__, __LINE__, msg)

#define LOG_ERROR_FORMAT(fmt, ...)                                                       \
    {                                                                                    \
        char msg[1024];                                                                  \
        snprintf (msg, sizeof (msg), fmt, __VA_ARGS__);                                  \
        LOG_ERROR (msg);                                                                 \
    }
#define LOG_WARN_FORMAT(fmt, ...)                                                        \
    {                                                                                    \
        char msg[1024];                                                                  \
        snprintf (msg, sizeof (msg), fmt, __VA_ARGS__);                                  \
        LOG_WARN (msg);                                                                  \
    }
#define LOG_INFO_FORMAT(fmt, ...)                                                        \
    {                                                                                    \
        char msg[1024];                                                                  \
        snprintf (msg, sizeof (msg), fmt, __VA_ARGS__);                                  \
        LOG_INFO (msg);                                                                  \
    }
#define LOG_DEBUG_FORMAT(fmt, ...)                                                       \
    {                                                                                    \
        char msg[1024];                                                                  \
        snprintf (msg, sizeof (msg), fmt, __VA_ARGS__);                                  \
        LOG_DEBUG (msg);                                                                 \
    }

#define _ASSERT_BEGIN_(type, expr)                                                       \
    if (! (expr)) {                                                                      \
        _LOG_ ("[" #type "]", __FILE__, __LINE__, "Assertion failed: " #expr);

#define _ASSERT_END_(hard_crash)                                                         \
        ON_DEBUG (DEBUG_BREAK);                                                          \
        if (hard_crash) {                                                                \
            exit (EXIT_FAILURE);                                                         \
        }                                                                                \
    }


#define SOFT_ASSERT_ALWAYS(expr) _ASSERT_BEGIN_(ERROR, expr) _ASSERT_END_(false)
#define SOFT_ASSERT(expr)        ON_DEBUG(SOFT_ASSERT_ALWAYS(expr))
#define ASSERT_ALWAYS(expr)      _ASSERT_BEGIN_(FATAL, expr) _ASSERT_END_(true)
#define ASSERT(expr)             ON_DEBUG(ASSERT_ALWAYS(expr))

#define SOFT_ASSERT_MSG_ALWAYS(expr, msg) _ASSERT_BEGIN_(ERROR, expr) LOG_ERROR_NO_BREAK(msg) _ASSERT_END_(false)
#define SOFT_ASSERT_MSG(expr, msg)        ON_DEBUG(SOFT_ASSERT_MSG_ALWAYS(expr, msg))
#define ASSERT_MSG_ALWAYS(expr, msg)      _ASSERT_BEGIN_(FATAL, expr) LOG_FATAL(msg) _ASSERT_END_(false)
#define ASSERT_MSG(expr, msg)             ON_DEBUG(ASSERT_MSG_ALWAYS(expr, msg))
