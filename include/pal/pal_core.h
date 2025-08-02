
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
 * @brief Function pointer type used for memory allocations.
 * 
 * Allocates `size` bytes with `alignment` and return a pointer or nullptr on failure.
 * The `userData` is the same pointer passed into the allocator struct.
 * 
 * @param[in] userData Pointer provided by the user.
 * @param[in] size Size in bytes to allocate.
 * @param[in] alignment Must be power of two and at least `8` or `16`. If zero `16` will be used.
 * 
 * @return Pointer to the allocated memory or nullptr if allocation failed.
 * 
 * @sa PalAllocator, PalFreeFn, palAllocate
 * @ingroup core
 */
typedef void* (*PalAllocateFn)(
    void* userData, 
    Uint64 size, 
    Uint64 alignment);

/**
 * @typedef PalFreeFn
 * @brief Function pointer type used for memory deallocations.
 * 
 * Deallocate memory allocated by PalAllocateFn. 
 * Passing nullptr or an invalid pointer for `ptr` must be safe.
 *
 * @param[in] userData Optional pointer provided by the user. May be a nullptr.
 * @param[in] ptr Pointer to the memory to free. May be a nullptr.
 * 
 * @sa PalAllocateFn, PalAllocator
 * @ingroup core
 */
typedef void (*PalFreeFn)(void* userData, void* ptr);

/**
 * @struct PalVersion
 * @brief Describes the version of the PAL library.
 *
 * @ingroup core
 */
typedef struct {
    int major;         /** < Major version (breaking changes).*/
    int minor;         /** < Minor version (features additions).*/
    int patch;         /** < Patch version (bug fixes only). */
} PalVersion;

/**
 * @struct PalAllocator
 * @brief Describes a user provided memory allocator.
 * 
 * Allows the user to override the default allocator for PAL.
 *
 * @ingroup core
 */
typedef struct {
    PalAllocateFn allocate;    /** < Allocation function pointer.*/
    PalFreeFn free;            /** < Free function pointer.*/
    void* userData;            /** < Optional user data passed to allocation functions.*/
} PalAllocator;

/**
 * @struct PalTimer
 * @brief Describes a high resolution timer
 *
 * @ingroup core
 */
typedef struct {
    Uint64 frequency;        /** < Ticks per second.*/
    Uint64 startTime;        /** < Start time of the timer.*/
} PalTimer;

/**
 * @brief Get the runtime version of PAL.
 * 
 * This returns a copy of PALVersion struct, 
 * which contains the major, minor and patch of the PAL runtime version.
 * 
 * This version reflects the compiled PAL library and it can be used to validate compatibility.
 *
 * @return A copy of PAL runtime version.
 * @note This function is thread-safe.
 *
 * @sa palGetVersionString(), PalVersion
 * @ingroup core
 */
_PAPI PalVersion _PCALL palGetVersion();

/**
 * @brief Get the human readable string representing the runtime version PAL.
 *
 * This returns a constant, null-terminated string describing the PAL runtime version
 * in the format: `1.2.3`, where `1` is the major, `2` is the minor and `3` is the patch versions respectively.
 *
 * @return A pointer to the null-terminated version string.
 * @note This function is thread-safe and the pointer should not be freed.
 *
 * @sa palGetVersion(), PalVersion
 * @ingroup core
 */
_PAPI const char* _PCALL palGetVersionString();

/**
 * @brief Allocate memory with a custom or default allocator.
 * 
 * If a valid allocator is provided, 
 * its `allocate` function is called with the provided size and user data.
 * Otherwise, the default allocator is used.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator will be used if it is a `nullptr`.
 * @param[in] size Size in bytes to allocate.
 * @param[in] alignment Must be power of two and at least `8` or `16`. If zero `16` will be used.
 * 
 * @return Pointer to the allocated memory or a nullptr if allocation failed.
 *
 * @note This does not initialize the allocated memory.
 * This function is thread safe if the provided allocator is thread safe.
 * @sa palFree(), PalAllocator
 * @ingroup core
 */
_PAPI void* _PCALL palAllocate(
    PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment);

/**
 * @brief Reallocates a memory block to a new size with a custom or default allocator.
 * 
 * If a valid allocator is provided, 
 * the function allocates and frees using the provided allocator's `allocate` and `free` functions.
 * Otherwise, the default allocator is used.
 * 
 * If `oldPtr` is a `nullptr`, the `newSize` is allocated.
 * If the `newSize` is `0`, the `oldPtr` is freed.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator will be used if it is a `nullptr`.
 * @param[in] oldPtr Pointer to the previously allocated memory.
 * @param[in] newSize Size in bytes to allocate for the new memory.
 * @param[in] alignment Must be power of two and at least `8` or `16`. If zero `16` will be used.
 * 
 * @return Pointer to the allocated memory or nullptr if allocation failed.
 *
 * @note This does not initialize the allocated memory.
 * This function is thread safe if the provided allocator is thread safe.
 * @sa palFree(), palAllocate(), PalAllocator
 * @ingroup core
 */
_PAPI void* _PCALL palRealloc(
    PalAllocator* allocator,
    void* oldPtr,
    Uint64 newSize,
    Uint64 alignment);

/**
 * @brief Free memory with a custom or default allocator.
 * 
 * If a valid allocator is provided, 
 * its `free` function is called with the provided ptr and user data.
 * Otherwise, the default allocator is used.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator will be used if it is a `nullptr`.
 * @param[in] ptr The pointer to the memory block to free.
 *
 * @note This function is thread safe if the provided allocator is thread safe.
 * @sa PalAllocator, palAllocate
 * @ingroup core
 */
_PAPI void _PCALL palFree(
    PalAllocator* allocator,
    void* ptr);

/**
 * @brief Log a formatted message.
 *
 * This supports unicode characters.
 *
 * @param[in] fmt UTF-8 encoding printf-style format string.
 *
 * Example:
 * @code
 * palLog("%s - %f", string, float);
 * @endcode
 *
 * @note This function is thread-safe. 
 * This will do nothing if there's no console on the OS.
 * @ingroup core
 */
_PAPI void _PCALL palLog(const char* fmt, ...);

/**
 * @brief Get the current system timer.
 * 
 * This is used to measure elasped time and for time related calculations.
 * You can use multiple timers for different use cases.
 * 
 * Example: one timer for total application time which you don't reset.
 * And another to calculate delta time which you reset every frame 
 * using palGetTime(). See `tests/src/time_test.c`.
 * 
 * @return A PalTimer struct cintaining the system frequency and current time.
 * 
 * @sa palGetTime(), PalTimer, palGetPerformanceCounter(), palGetPerformanceFrequency()
 *
 * @note This function is thread-safe.
 * @ingroup core
 */
_PAPI PalTimer _PCALL palGetSystemTimer();

/**
 * @brief Get the current time in seconds using a timer.
 * 
 * This returns the current absolute time in seconds using a high resolution timer.
 *
 * @param[in] timer High resolution timer to use.
 * @return Current high resolution time in seconds or `0.0` if `timer` is nullptr.
 * 
 * @note This function is thread-safe. This should not be used to measure calender time
 * @sa palGetPerformanceCounter(), palGetPerformanceFrequency()
 * @ingroup core
 */
_PAPI double _PCALL palGetTime(PalTimer* timer);

/**
 * @brief Get the system performance counter value in ticks
 * 
 * This can be used to measure elasped time when divided by the performance frequency.
 * This is done by `palGetTime()`
 * 
 * @return Current tick count from the system
 * @sa palGetTime(), palGetPerformanceFrequency
 * @note This function is thread-safe.
 * @ingroup core
 */
_PAPI Uint64 _PCALL palGetPerformanceCounter();

/**
 * @brief Get the frequency of the system performance counter.
 * 
 * This function returns the number of ticks per second of the system performance counter.
 * @return Frequency in ticks per second.
 * 
 * @sa palGetTime(), palGetPerformanceCounter
 * @note This function is thread-safe. 
 * 
 * @note The returned frequency is constant for the duration of the application.
 * @ingroup core
 */
_PAPI Uint64 _PCALL palGetPerformanceFrequency();

#endif // _PAL_CORE_H