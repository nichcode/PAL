
/**

Copyright (C) 2025 Nicholas Agbo
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

/**
 * @file pal_defines.h
 * 
 * Header file for defines and shared macros for all systems
 * 
 */

#ifndef _PAL_DEFINES_H
#define _PAL_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Set up shared library dependencies
#ifdef _WIN32
#define _PCALL __stdcall
#ifdef _PAL_EXPORT
#define _PAPI __declspec(dllexport)
#else 
#define _PAPI __declspec(dllimport)
#endif // PAL_EXPORT
#else
// other platforms
#define _PCALL
#ifdef PAL_EXPORT
#define _PAPI extern "C" __attribute__((visibility("default")))
#else 
#define _PAPI
#endif // PAL_EXPORT
#endif // _WIN32

// systems reflection
#ifdef _PAL_BUILD_VIDEO
#define PAL_HAS_VIDEO 1
#else 
#define PAL_HAS_VIDEO 0
#endif // 

// Set up typedefs for C
#ifndef __cplusplus
#define nullptr ((void *)0)
#define true 1
#define false 0

/**
 * @brief A bool
 * @warning This works from C99
 */
typedef _Bool bool;
#endif // __cplusplus

/**
 * @brief An unsigned 8-bit integer
 */
typedef unsigned char Uint8;

/**
 * @brief An signed 8-bit integer
 */
typedef signed char Int8;

/**
 * @brief Ann unsigned 16-bit integer
 */
typedef unsigned short Uint16;

/**
 * @brief An signed 16-bit integer
 */
typedef signed short Int16;

/**
 * @brief Ann unsigned 32-bit integer
 */
typedef unsigned int Uint32;

/**
 * @brief An signed 32-bit integer
 */
typedef signed int Int32;

/**
 * @brief Ann unsigned 64-bit integer
 */
typedef unsigned long long Uint64;

/**
 * @brief Ann unsigned 64-bit integer
 */
typedef signed long long Int64;

/**
 * @enum PalResult
 * @brief codes returned by PAL functions.
 * 
 * Aside from the core system, PAL functions return a PalResult.
 * 
 * PAL_SUCCESS code means the operation completed successfully. 
 * Any other value indicates an error.
 * 
 * @note For clarity, always use the named constants like 
 * `PAL_RESULT_OUT_OF_MEMORY` not their numeric values.
 * 
 * @note All result codes follow the format `PAL_RESULT_**` for consistency and API use.
 */
typedef enum {
    PAL_SUCCESS,                          /** < Operation was successful.*/
    PAL_RESULT_NULL_POINTER,              /** < A nullptr was used where it is not allowed.*/
    PAL_RESULT_INVALID_ARGUMENT,          /** < One or more arguments were invalid.*/
    PAL_RESULT_OUT_OF_MEMORY,             /** < Out of Memory or memory allocation failed.*/
    PAL_RESULT_INVALID_ALLOCATOR,         /** < A partially defined custom allocator.*/
    PAL_RESULT_ACCESS_DENIED              /** < OS denied PAL access.*/
} PalResult;

/**
 * @brief Return a human readable string for a specified result code.
 * 
 * This returns a constant, null-terminated string describing the result.
 * 
 * @param[in] result The result code to describe
 * @return A pointer to the null-terminated result string.
 * 
 * @note This function is thread-safe and the pointer should not be freed.
 * @sa PalResult
 */
_PAPI const char* _PCALL palResultToString(PalResult result);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _PAL_DEFINES_H