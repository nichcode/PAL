/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef SHARED_H
#define SHARED_H

#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
#define PLATFORM_POSIX 1
#else
#define PLATFORM_POSIX 0
#endif // POSIX detection

#include <stdarg.h>
#include <stdint.h>

#if defined(__cplusplus)
    #define ATTR_NORETURN [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define ATTR_NORETURN _Noreturn
#elif defined(__GNUC__) || defined(__clang__)
    #define ATTR_NORETURN __attribute__((noreturn))
#else
    #define ATTR_NORETURN
#endif

ATTR_NORETURN void handleAssert(
    const char* file, 
    uint32_t line, 
    const char* expr, 
    const char* msg);

#ifndef NDEBUG
#define ASSERT(expr, msg) do { \
    if (!(expr)) { \
        handleAssert(__FILE__, __LINE__, #expr, msg); \
    } \
} while (0)
#else
#define ASSERT(expr, msg) ((void)0)
#endif // NDEBUG

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define ALIGN_UP(value, alignment) \
    (((value) + ((alignment) - 1)) & ~(((value) - (value)) + ((alignment) - 1)))

void formatMsgArgs(
    const char* fmt, 
    va_list argsList, 
    char* buffer, 
    uint64_t bufferSize);

void formatMsg(
    char* buffer, 
    uint64_t bufferSize, 
    const char* fmt, 
    ...);

#endif // SHARED_H