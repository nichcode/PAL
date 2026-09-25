
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

#ifndef PAL_CORE_DEFINES_H
#define PAL_CORE_DEFINES_H

#include <stdint.h>

#ifdef __cplusplus
#define PAL_EXTERN_C extern "C"
#else
#define PAL_EXTERN_C
#define nullptr ((void*)0)
#endif // __cplusplus

#ifdef _WIN32
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllexport)
#else
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllimport)
#endif // PAL_EXPORT
#else
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
#define PAL_API PAL_EXTERN_C
#endif // _PAL_BUILD_DLL

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define PAL_BIG_ENDIAN 1
#else
#define PAL_BIG_ENDIAN 0
#endif // __ORDER_BIG_ENDIAN__

#define PAL_INFINITE UINT32_MAX

#define PAL_TRUE 1
#define PAL_FALSE 0

/**
 * @typedef PalBool
 * @brief A boolean type
 * 
 * Possible values are PAL_TRUE or PAL_FALSE.
 * 
 * These are constants for 1 and 0 and any equivalent constant can be used.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef uint32_t PalBool;

#endif // PAL_CORE_DEFINES_H