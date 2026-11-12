
/**
 * @file base.h
 * 
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

#ifndef PAL_CORE_BASE_H
#define PAL_CORE_BASE_H

#include <stdint.h>

/**
 * @typedef PalBool
 * @brief Boolean type.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef uint32_t PalBool;

#ifdef __cplusplus
/**
 * @brief C linkage when compiled as C++.
 * @ingroup pal_core
 */
#define PAL_EXTERN_C extern "C"
#else
/**
 * @brief Empty when compiled as C.
 * @ingroup pal_core
 */
#define PAL_EXTERN_C
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ <= 201710L
/**
 * @brief A value representing `NULL`.
 * @ingroup pal_core
 */
#define nullptr ((void*)0)
#endif // __STDC_VERSION__
#endif // __cplusplus

#ifdef _WIN32
/**
 * @brief Calling convention of PAL functions and callbacks.
 * @ingroup pal_core
 */
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
/**
 * @brief Exports symbols when building the PAL shared library.
 * @ingroup pal_core
 */
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllexport)
#else
/**
 * @brief Imports symbols from the PAL shared library.
 * @ingroup pal_core
 */
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllimport)
#endif // _PAL_EXPORT
#else
/**
 * @brief Calling convention of PAL functions and callbacks.
 * @ingroup pal_core
 */
#define PAL_CALL
#ifdef _PAL_EXPORT
/**
 * @brief Exports symbols when building the PAL shared library.
 * @ingroup pal_core
 */
#define PAL_DECLSPEC PAL_EXTERN_C __attribute__((visibility("default")))
#else
/**
 * @brief Imports symbols from the PAL shared library.
 * @ingroup pal_core
 */
#define PAL_DECLSPEC PAL_EXTERN_C
#endif // _PAL_EXPORT
#endif // _WIN32

#ifdef _PAL_BUILD_DLL
/**
 * @brief Marks a function as part of the PAL API
 * @ingroup pal_core
 */
#define PAL_API PAL_EXTERN_C PAL_DECLSPEC
#else
/**
 * @brief Marks a function as part of the PAL API
 * @ingroup pal_core
 */
#define PAL_API PAL_EXTERN_C
#endif // _PAL_BUILD_DLL

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
/**
 * @brief `1` if the target platform is big-endian.
 * @ingroup pal_core
 */
#define PAL_BIG_ENDIAN 1
#else
/**
 * @brief `0` if the target platform is little-endian.
 * @ingroup pal_core
 */
#define PAL_BIG_ENDIAN 0
#endif // __ORDER_BIG_ENDIAN__

/**
 * @brief A value representing boolean true or `1`.
 * @ingroup pal_core
 */
#define PAL_TRUE 1

/**
 * @brief A value representing boolean false or `0`.
 * @ingroup pal_core
 */
#define PAL_FALSE 0

/**
 * @brief A value representing infinite time or number.
 * @ingroup pal_core
 */
#define PAL_INFINITE UINT32_MAX

#endif // PAL_CORE_BASE_H