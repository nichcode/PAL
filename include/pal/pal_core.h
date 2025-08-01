
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
 * @file pal_core.h
 *
 * Header file for core functions, macros, enum and structs
 *
 * @defgroup core
 *
 */

#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include "pal_defines.h"

/**
 * @typedef PalAllocateFn
 * @brief A type used to identify an allocate function pointer
 *
 * @param[in] userData The user data
 * @param[in] size The size in bytes of memory to allocate
 * @ingroup core
 */
typedef void* (*PalAllocateFn)(void* userData, Uint64 size);

/**
 * @typedef PalFreeFn
 * @brief A type used to identify a free function pointer
 *
 * @param[in] userData The user data
 * @param[in] ptr The pointer to the memory block to free
 * @ingroup core
 */
typedef void (*PalFreeFn)(void* userData, void* ptr);

/**
 * @struct PalVersion
 * @brief A type used to identify version
 *
 * @ingroup core
 */
typedef struct {
    int major;         /** < Major version*/
    int minor;         /** < Minor version */
    int revision;      /** < Revision version */
} PalVersion;

/**
 * @struct PalAllocator
 * @brief A type used to identify an allocator
 *
 * @ingroup core
 */
typedef struct {
    PalAllocateFn allocate;    /** < Allocate function pointer*/
    PalFreeFn free;            /** < Free function pointer*/
    void* userData;            /** < Data passed in function pointers*/
} PalAllocator;

/**
 * @struct PalTimer
 * @brief A type used to identify a timer
 *
 * @ingroup core
 */
typedef struct {
    Uint64 frequency;        /** < Cycles per second*/
    Uint64 startTime;        /** < Start time of the timer*/
} PalTimer;

/**
 * @brief Get the compiled runtime version of the library
 *
 * @note This function is thread-safe
 *
 * @sa palGetVersionString()
 * @ingroup core
 * @return A copy of the version
 */
_PAPI PalVersion _PCALL palGetVersion();

/**
 * @brief Get the compiled runtime version string of the library.
 *
 * The format of the string will be: `1.2.3`.
 *
 * Where `1` is the major, `2` is the minor and `3` is the revision versions respectively
 *
 * @sa palGetVersion()
 * @ingroup core
 * @note This function is thread-safe
 *
 * @return A copy of the version string
 */
_PAPI const char* _PCALL palGetVersionString();

/**
 * @brief Allocate memory with a custom or default allocator
 *
 * @param[in] allocator This can be custom(user provided) or PAL_NULL to use default
 * @param[in] size The size in bytes of memory to allocate
 *
 * @sa palFree()
 * @note This function is thread-safe
 * @ingroup core
 *
 * @return A memory block or PAL_NULL if failed
 */
_PAPI void* _PCALL palAllocate(
    PalAllocator* allocator,
    Uint64 size);

/**
 * @brief Free memory with a custom or default allocator
 *
 * @warning free the memory with the same allocator used for the allocation
 *
 * @param[in] allocator This can be custom(user provided) or PAL_NULL to use default
 * @param[in] ptr The pointer to the memory block to free
 *
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palFree(
    PalAllocator* allocator,
    void* ptr);

/**
 * @brief Set a block of memory to a specific byte value
 *
 * @param[in] ptr The pointer to the memory block to fill
 * @param[in] value The byte value to set(casted to unsigned char)
 * @param[in] size The number or size of bytes to set
 *
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palSetMemory(
    void* ptr,
    int value,
    Uint64 size);

/**
 * @brief Set a block of memory to zero
 *
 * @param[in] ptr The pointer to the memory block to fill
 * @param[in] size The number or size of bytes to set
 *
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palZeroMemory(
    void* ptr,
    Uint64 size);

/**
 * @brief Copy a block of memory to another block of memory
 *
 * @param[in] dest The pointer to the memory block to copy to
 * @param[in] src The pointer to the memory block to copy from
 * @param[in] size The number or size of bytes to copy
 *
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palCopyMemory(
    void* dest,
    const void* src,
    Uint64 size);

/**
 * @brief Move a block of memory to another block of memory
 *
 * @param[in] dest The pointer to the memory block to move to
 * @param[in] src The pointer to the memory block to move from
 * @param[in] size The number or size of bytes to move
 *
 * @note This function is thread-safe
 * @ingroup core
 */
_PAPI void _PCALL palMoveMemory(
    void* dest,
    const void* src,
    Uint64 size);

/**
 * @brief Log a message on the stdout console or debug console
 *
 * This supports unicode characters
 *
 * @param[in] fmt The UTF-8 encoding printf-style format
 *
 * Example:
 * @code
 * palLog("%s - %f", string, float);
 * @endcode
 *
 * @note This function is thread-safe. This will do nothing if there's no console
 * @ingroup core
 */
_PAPI void _PCALL palLog(const char* fmt, ...);

/**
 * @brief Get the system timer.
 *
 * This is a reflects of the OS current time and frequency
 *
 * @sa palGetTime()
 *
 * @note This function is thread-safe
 * @return The system timer
 * @ingroup core
 */
_PAPI PalTimer _PCALL palGetSystemTimer();

/**
 * @brief Get the current time in seconds using a timer
 *
 * @param[in] timer The timer to use
 * @note This function is thread-safe
 * @return the time in seconds or 0 if the timer is invalid
 * @ingroup core
 */
_PAPI double _PCALL palGetTime(PalTimer* timer);

/**
 * @brief Get the system performance counter
 *
 * @note This function is thread-safe
 * @return the performance counter
 * @ingroup core
 */
_PAPI Uint64 _PCALL palGetPerformanceCounter();

/**
 * @brief Get the system performance frequency
 *
 * @note This function is thread-safe
 * @return the performance frequency
 * @ingroup core
 */
_PAPI Uint64 _PCALL palGetPerformanceFrequency();

#endif // _PAL_CORE_H