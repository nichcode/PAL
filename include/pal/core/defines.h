
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup defines Base defines
 * @ingroup pal_core
 * @{
 */

#ifndef _CORE_DEFINES_H
#define _CORE_DEFINES_H

#include <stdint.h>

#ifdef __cplusplus
#define PAL_EXTERN_C extern "C"
#else
#define PAL_EXTERN_C
#define nullptr ((void*)0)
#define true 1
#define false 0

/**
 * @brief A bool
 * @since 1.0
 */
typedef _Bool bool;
#endif // __cplusplus

// Set up shared library dependencies
#ifdef _WIN32
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllexport)
#else
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllimport)
#endif // PAL_EXPORT
#else
// other plafforms
#define PAL_CALL
#ifdef _PAL_EXPORT
#define PAL_DECLSPEC PAL_EXTERN_C __attribute__((visibility("default")))
#else
#define PAL_DECLSPEC PAL_EXTERN_C
#endif // PAL_EXPORT
#endif // _WIN32

#ifdef _PAL_BUILD_DLL
#define PAL_API PAL_EXTERN_C PAL_DECLSPEC
#else
// static library
#define PAL_API PAL_EXTERN_C
#endif // _PAL_BUILD_DLL

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define PAL_BIG_ENDIAN 1
#else
#define PAL_BIG_ENDIAN 0
#endif // __ORDER_BIG_ENDIAN__

#define PAL_BIT(x) 1 << x
#define PAL_BIT64(x) 1ULL << x
#define PAL_INFINITE UINT32_MAX

/**
 * @brief A signed 8-bit integer
 * @since 1.0
 */
typedef int8_t Int8;

/**
 * @brief A signed 16-bit integer
 * @since 1.0
 */
typedef int16_t Int16;

/**
 * @brief A signed 32-bit integer
 * @since 1.0
 */
typedef int32_t Int32;

/**
 * @brief A signed 64-bit integer
 * @since 1.0
 */
typedef int64_t Int64;

/**
 * @brief A signed 64-bit integer pointer
 * @since 1.0
 */
typedef intptr_t IntPtr;

/**
 * @brief An unsigned 8-bit integer
 * @since 1.0
 */
typedef uint8_t Uint8;

/**
 * @brief An unsigned 16-bit integer
 * @since 1.0
 */
typedef uint16_t Uint16;

/**
 * @brief An unsigned 32-bit integer
 * @since 1.0
 */
typedef uint32_t Uint32;

/**
 * @brief An unsigned 64-bit integer
 * @since 1.0
 */
typedef uint64_t Uint64;

/**
 * @brief An unsigned 64-bit integer pointer
 * @since 1.0
 */
typedef uintptr_t UintPtr;

#endif // _CORE_DEFINES_H

/** @} */