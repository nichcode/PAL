
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
 * @defgroup defines
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

// Set up typedefs for C/C++
#ifdef __cplusplus
/**
 * @brief A typedef or alias for `nullptr`
 * @ingroup defines
 */
#define PAL_NULL nullptr

/**
 * @brief A typedef or alias for `true` or `1`
 * @ingroup defines
 */
#define PAL_TRUE true

/**
 * @brief A typedef or alias for `false` or `0`
 * @ingroup defines
 */
#define PAL_FALSE false
#else
/**
 * @brief A typedef or alias for `nullptr`
 * @ingroup defines
 */

#define PAL_NULL ((void *)0)

/**
 * @brief A typedef or alias for `true` or `1`
 * @ingroup defines
 */
#define PAL_TRUE 1

/**
 * @brief A typedef or alias for `false` or `0`
 * @ingroup defines
 */
#define PAL_FALSE 0

/**
 * @brief A typedef or alias for `bool`
 * @warning This works from C99
 * @ingroup defines
 */
typedef _Bool bool;
#endif // __cplusplus

#define PAL_DECLARE_HANDLE_TYPE(name) typedef struct name##_T* name;
#define PAL_DECLARE_HANDLE(name) typedef struct name *name;

/**
 * @brief A typedef or alias for an unsigned 8-bit integer
 * @ingroup defines
 */
typedef unsigned char Uint8;

/**
 * @brief A typedef or alias for a signed 8-bit integer
 * @ingroup defines
 */
typedef signed char Int8;

/**
 * @brief A typedef or alias for an unsigned 16-bit integer
 * @ingroup defines
 */
typedef unsigned short Uint16;

/**
 * @brief A typedef or alias for a signed 16-bit integer
 * @ingroup defines
 */
typedef signed short Int16;

/**
 * @brief A typedef or alias for an unsigned 32-bit integer
 * @ingroup defines
 */
typedef unsigned int Uint32;

/**
 * @brief A typedef or alias for a signed 32-bit integer
 * @ingroup defines
 */
typedef signed int Int32;

/**
 * @brief A typedef or alias for an unsigned 64-bit integer
 * @ingroup defines
 */
typedef unsigned long long Uint64;

/**
 * @brief A typedef or alias for an unsigned 64-bit integer
 * @ingroup defines
 */
typedef signed long long Int64;

/**
 * @enum PalResult
 * @brief A type used to identify return codes from functions
 * All result codes start from `PAL_RESULT_**` for consistency and API use
 * @ingroup defines
 */
typedef enum {
   PAL_SUCCESS,                          /** < Operation was successful*/
   PAL_RESULT_NULL_POINTER,              /** < Pointer was invalid*/
   PAL_RESULT_INVALID_ARGUMENT,          /** < Argument was invalid*/
   PAL_RESULT_OUT_OF_MEMORY,             /** < Out of Memory*/
} PalResult;

/**
 * @brief Convert the result code to a UTF-8 encoding string
 * 
 * @param result The result code
 * @note This function is thread-safe and the memory should not be freed.
 * 
 * @return A string in UTF-8 encoding
 */
_PAPI const char* _PCALL palResultToString(PalResult result);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _PAL_DEFINES_H