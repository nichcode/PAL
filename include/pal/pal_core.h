
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

#include <stdint.h>

// Set up shared library dependencies
#ifdef _WIN32
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
#define PAL_API __declspec(dllexport)
#else
#define PAL_API __declspec(dllimport)
#endif // PAL_EXPORT
#else
// other platforms
#define PAL_CALL
#ifdef PAL_EXPORT
#define PAL_API extern "C" __attribute__((visibility("default")))
#else
#define PAL_API
#endif // PAL_EXPORT
#endif // _WIN32

#ifdef __cplusplus
}
#endif // __cplusplus

// Set up typedefs for C
#ifndef __cplusplus
#define nullptr ((void*)0)
#define true 1
#define false 0

/**
 * @brief A bool
 */
typedef _Bool bool;

#endif // __cplusplus

#define PAL_BIT(x) 1 << x

/**
 * @brief An signed 8-bit integer
 */
typedef int8_t Int8;

/**
 * @brief An signed 16-bit integer
 */
typedef int16_t Int16;

/**
 * @brief An signed 32-bit integer
 */
typedef int32_t Int32;

/**
 * @brief An signed 64-bit integer
 */
typedef int64_t Int64;

/**
 * @brief An signed 64-bit integer pointer
 */
typedef intptr_t IntPtr;

/**
 * @brief An unsigned 8-bit integer
 */
typedef uint8_t Uint8;

/**
 * @brief An unsigned 16-bit integer
 */
typedef uint16_t Uint16;

/**
 * @brief An unsigned 32-bit integer
 */
typedef uint32_t Uint32;

/**
 * @brief An unsigned 64-bit integer
 */
typedef uint64_t Uint64;

/**
 * @brief An unsigned 64-bit integer pointer
 */
typedef uintptr_t UintPtr;

/**
 * @typedef PalAllocateFn
 * @brief Function pointer type used for CPU memory allocations.
 *
 * Allocates `size` bytes with `alignment` and return a pointer to the memory
 * address or nullptr on failure. The `userData` is the same pointer passed into
 * the allocator struct.
 *
 * @param[in] userData Optional pointer to user data. Can be `nullptr`.
 * @param[in] size Size in bytes to allocate.
 * @param[in] alignment Must be power of two. It is recommended that it should
 * atleast `8` or `16`. If zero `16` will be used.
 *
 * @return Pointer to the allocated memory or nullptr if allocation failed.
 *
 * @sa PalAllocator, PalFreeFn, palAllocate
 * @ingroup core
 */
typedef void*(PAL_CALL* PalAllocateFn)(void* userData, Uint64 size, Uint64 alignment);

/**
 * @typedef PalFreeFn
 * @brief Function pointer type used for CPU memory deallocations.
 *
 * Deallocate memory allocated by PalAllocateFn.
 * Passing nullptr or an invalid pointer for `ptr` must be safe.
 *
 * @param[in] userData Optional pointer to user data. Can be `nullptr`.
 * @param[in] ptr Pointer to the memory to free. This function should do nothing
 * if `ptr` is `nullptr`
 *
 * @sa PalAllocateFn, PalAllocator
 * @ingroup core
 */
typedef void(PAL_CALL* PalFreeFn)(void* userData, void* ptr);

/**
 * @typedef PalLogCallback
 * @brief Function pointer type used for log callback.
 *
 * This provides control over how log messages are handled (eg. redirect the log
 * message to a file).
 *
 * The `msg` is only valid for the duration of the callback, copy if you need to
 * store it.
 *
 * @param userData Optional pointer to user data. Can be `nullptr`.
 * @param msg A constant null-terminated UTF-8 encoding string containing the
 * message.
 *
 * @sa PalLogger, palLog()
 * @ingroup core
 */
typedef void(PAL_CALL* PalLogCallback)(void* userData, const char* msg);

/**
 * @enum PalResult
 * @brief codes returned by PAL functions.
 *
 * `PAL_RESULT_SUCCESS` code means the operation completed successfully. Any
 * other value indicates an error.
 *
 * @note For clarity, always use the named constants like
 * `PAL_RESULT_OUT_OF_MEMORY` not their numeric values.
 *
 * @note All results follow the format `PAL_RESULT_**` for consistency and API
 * use.
 */
typedef enum {
    PAL_RESULT_SUCCESS,             /** < Operation was successful.*/
    PAL_RESULT_NULL_POINTER,        /** < One or more pointers were nullptr or invalid.*/
    PAL_RESULT_INVALID_PARAMETER,   /** < One or more arguments were invalid.*/
    PAL_RESULT_OUT_OF_MEMORY,       /** < Out of memory.*/
    PAL_RESULT_PLATFORM_FAILURE,    /** < An error occured on the plaform (OS)
                                       level.*/
    PAL_RESULT_INVALID_ALLOCATOR,   /** < A partially defined custom allocator.*/
    PAL_RESULT_ACCESS_DENIED,       /** < Platform (OS) denied PAL access.*/
    PAL_RESULT_TIMEOUT,             /** < Timeout expired.*/
    PAL_RESULT_INSUFFICIENT_BUFFER, /** < Buffer provided for write operation is
                                       too small.*/

    PAL_RESULT_INVALID_THREAD,               /** < An invalid thread.*/
    PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED, /** < The thread feature used was not
                                                supported*/

    PAL_RESULT_VIDEO_NOT_INITIALIZED,       /** < Video system is not initialized.*/
    PAL_RESULT_INVALID_MONITOR,             /** < An invalid monitor.*/
    PAL_RESULT_INVALID_MONITOR_MODE,        /** < An invalid monitor display mode.*/
    PAL_RESULT_INVALID_WINDOW,              /** < An invalid window.*/
    PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED, /** < The video feature used was not
                                               supported.*/
    PAL_RESULT_INVALID_KEYCODE,             /** < An invalid keycode.*/
    PAL_RESULT_INVALID_SCANCODE,            /** < An invalid scancode.*/
    PAL_RESULT_INVALID_MOUSE_BUTTON,        /** < An invalid mouse button.*/
    PAL_RESULT_INVALID_ORIENTATION,         /** < An invalid orientation.*/

    PAL_RESULT_GL_NOT_INITIALIZED,         /** < Opengl system is not initialized.*/
    PAL_RESULT_INVALID_GL_WINDOW,          /** < An invalid opengl window.*/
    PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED, /** < The opengl extension used was not
                                              supported.*/
    PAL_RESULT_INVALID_GL_FBCONFIG,        /** < An invalid opengl framebuffer config.*/
    PAL_RESULT_INVALID_GL_VERSION,         /** < The requested opengl version is not
                                              supported.*/
    PAL_RESULT_INVALID_GL_PROFILE,         /** < The requested opengl profile is not
                                              supported.*/
    PAL_RESULT_INVALID_GL_CONTEXT          /** < An invalid opengl context.*/
} PalResult;

/**
 * @struct PalVersion
 * @brief Describes the version of the PAL.
 *
 * @ingroup core
 */
typedef struct {
    Uint32 major; /** < Major version (breaking changes).*/
    Uint32 minor; /** < Minor version (adding features).*/
    Uint32 build; /** < Build version (bug fixes). */
} PalVersion;

/**
 * @struct PalAllocator
 * @brief Describes a user provided CPU memory allocator.
 *
 * Allows the user to override the default CPU allocator for PAL.
 *
 * @ingroup core
 */
typedef struct {
    PalAllocateFn allocate; /** < CPU memory allocation function pointer.*/
    PalFreeFn     free;     /** < CPU memory deallocation function pointer.*/
    void*         userData; /** Optional pointer to user data passed into allocation
                               functions. Can be `nullptr`.*/
} PalAllocator;

/**
 * @struct PalLogger
 * @brief Describes a user provided Logger for redirecting log messages.
 *
 * Allows the user to override the default log output.
 *
 * @ingroup core
 */
typedef struct {
    PalLogCallback callback; /** < Log callback for log directing.*/
    void*          userData; /** Optional pointer to user data passed into log callback.
                                Can be `nullptr`.*/
} PalLogger;

/**
 * @brief Get the runtime version of PAL.
 *
 * This returns a copy of PALVersion struct, which contains the major, minor and
 * build of the PAL runtime.
 *
 * @return A copy of PAL runtime version.
 * @note This function is thread-safe
 * @sa palGetVersionString(), PalVersion
 * @ingroup core
 */
PAL_API PalVersion PAL_CALL palGetVersion();

/**
 * @brief Get the human readable string representing the runtime version of PAL.
 *
 * This returns a constant, null-terminated UTF-8 encoding string describing the
 * PAL runtime version in the format: `1.2.3`, where `1` is the major, `2` is
 * the minor and `3` is the build versions respectively.
 *
 * @return A pointer to the null-terminated UTF-8 encoding string.
 * @note This function is thread-safe and the pointer must not be freed.
 * @sa palGetVersion(), PalVersion
 * @ingroup core
 */
PAL_API const char* PAL_CALL palGetVersionString();

/**
 * @brief Return a human readable string for a specified result code.
 *
 * This returns a constant, null-terminated UTF-8 encoding string describing the
 * result.
 *
 * @param[in] result The result code
 * @return A pointer to the null-terminated UTF-5 encoding string.
 *
 * @note This function is thread-safe and the pointer must not be freed.
 * @sa PalResult
 */
PAL_API const char* PAL_CALL palFormatResult(PalResult result);

/**
 * @brief Allocate CPU memory with a custom or PAL's default allocator.
 *
 * If a valid allocator is provided, its `allocate` function is called with the
 * provided size and user data. Otherwise, PAL's default allocator is used.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator
 * will be used if it is `nullptr`.
 * @param[in] size Size in bytes to allocate.
 * @param[in] alignment Must be power of two. It is recommended that it should
 * atleast `8` or `16`. If zero `16` will be used.
 *
 * @return Pointer to the allocated CPU memory or nullptr if allocation failed.
 *
 * @note This does not initialize the allocated memory.
 * This function is thread safe if the provided allocator is thread safe.
 *
 * @sa palFree(), PalAllocator
 * @ingroup core
 */
PAL_API void* PAL_CALL palAllocate(const PalAllocator* allocator, Uint64 size, Uint64 alignment);

/**
 * @brief Free CPU memory with a custom or PAL's default allocator.
 *
 * If a valid allocator is provided, its `free` function is called with the
 * provided ptr and user data. Otherwise, PAL's default allocator is used.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator
 * will be used if it is a `nullptr`.
 * @param[in] ptr The pointer to the memory block to free.
 *
 * @note This function is thread safe if the provided allocator is thread safe.
 *
 * @sa PalAllocator, palAllocate
 * @ingroup core
 */
PAL_API void PAL_CALL palFree(const PalAllocator* allocator, void* ptr);

/**
 * @brief Log a formatted message with a custom or PAL's default logger.
 *
 * If the provided logger does not have a valid `callback`, PAL will discard it
 * and use its default one. To use PAL's default logger, pass in `nullptr`. This
 * supports unicode characters.
 *
 * @param[in] logger Optional pointer to a logger. The default logger will be
 * used if it is `nullptr`.
 * @param[in] fmt UTF-8 encoding printf-style format string.
 *
 * Example:
 *
 * @code
 * palLog(&myLogger, "%s - %f", string, float);
 * @endcode
 *
 * @note This function is thread-safe.
 * This will do nothing if there's no console on the platform (OS).
 * On Windows, PAL will log to the debug console.
 *
 * @ingroup core
 */
PAL_API void PAL_CALL palLog(const PalLogger* logger, const char* fmt, ...);

/**
 * @brief Get the system performance counter value in ticks
 *
 * @return Current tick count from the system
 *
 * @note This function is thread-safe.
 *
 * @sa palGetPerformanceFrequency
 * @ingroup core
 */
PAL_API Uint64 PAL_CALL palGetPerformanceCounter();

/**
 * @brief Get the frequency of the system performance counter.
 *
 * This function returns the number of ticks per second of the system
 * performance counter.
 * @return Frequency in ticks per second.
 *
 * @note The returned frequency is constant for the duration of the application.
 * @note This function is thread-safe.
 *
 * @sa palGetPerformanceCounter
 * @ingroup core
 */
PAL_API Uint64 PAL_CALL palGetPerformanceFrequency();

/**
 * @brief Combine two 32-bit unsigned integers into a 64-bit signed integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * @note This function is thread-safe.
 *
 * @sa palPackInt32(), palUnpackUint32(), palUnpackInt32()
 * @ingroup core
 */
static inline Int64 PAL_CALL palPackUint32(Uint32 low, Uint32 high)
{

    return (Int64)(((Uint64)high << 32) | (Uint64)low);
}

/**
 * @brief Combine two 32-bit signed integers into a 64-bit signed integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * @note This function is thread-safe.
 *
 * @sa palPackInt32(), palUnpackUint32(), palUnpackInt32()
 * @ingroup core
 */
static inline Int64 PAL_CALL palPackInt32(Int32 low, Int32 high)
{

    return ((Int64)(Uint32)high << 32) | (Uint32)low;
}

/**
 * @brief Pack a 32-bit or 64-bit pointer into a 64-bit signed integer.
 *
 * @return The packed 64-bit signed integer.
 *
 * @note This function is thread-safe.
 *
 * @sa palUnpackPointer()
 * @ingroup core
 */
static inline Int64 PAL_CALL palPackPointer(void* ptr)
{

    return (Int64)(UintPtr)ptr;
}

/**
 * @brief Unpack a 64-bit signed integer into two 32-bit unsigned integers.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * @note This function is thread-safe.
 *
 * @sa palPackUint32(), palUnpackInt32()
 * @ingroup core
 */
static inline void PAL_CALL palUnpackUint32(Int64 data, Uint32* outLow, Uint32* outHigh)
{

    if (outLow) {
        *outLow = (Uint32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Uint32)((Uint64)data >> 32);
    }
}

/**
 * @brief Unpack a 64-bit signed integer into two 32-bit signed integers.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * @note This function is thread-safe.
 *
 * @sa palUnpackInt32(), palPackUint32()
 * @ingroup core
 */
static inline void PAL_CALL palUnpackInt32(Int64 data, Int32* outLow, Int32* outHigh)
{

    if (outLow) {
        *outLow = (Int32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Int32)((Uint64)data >> 32);
    }
}

/**
 * @brief Unpack a pointer from a 64-bit signed integer.
 *
 * @return The pointer from the 64-bit signed integer.
 *
 * @note This function is thread-safe.
 *
 * @sa palPackPointer()
 * @ingroup core
 */
static inline void* PAL_CALL palUnpackPointer(Int64 data)
{

    return (void*)(UintPtr)data;
}

#endif // _PAL_CORE_H