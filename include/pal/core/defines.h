
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup defines Defines section
 * @ingroup pal_core
 * @{
 */

#ifndef _CORE_DEFINES_H
#define _CORE_DEFINES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
#define PAL_EXTERN_C extern "C"
#else
#define PAL_EXTERN_C
#define nullptr ((void*)0)
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

/**
 * @brief Sentinel representing an infinite number or time.
 * @since 2.0
 */
#define PAL_INFINITE UINT32_MAX

#endif // _CORE_DEFINES_H

/** @} */