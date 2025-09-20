
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
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef _Bool bool;

#endif // __cplusplus

#define PAL_BIT(x) 1 << x

/**
 * @brief An signed 8-bit integer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef int8_t Int8;

/**
 * @brief An signed 16-bit integer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef int16_t Int16;

/**
 * @brief An signed 32-bit integer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef int32_t Int32;

/**
 * @brief An signed 64-bit integer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef int64_t Int64;

/**
 * @brief An signed 64-bit integer pointer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef intptr_t IntPtr;

/**
 * @brief An unsigned 8-bit integer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef uint8_t Uint8;

/**
 * @brief An unsigned 16-bit integer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef uint16_t Uint16;

/**
 * @brief An unsigned 32-bit integer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef uint32_t Uint32;

/**
 * @brief An unsigned 64-bit integer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef uint64_t Uint64;

/**
 * @brief An unsigned 64-bit integer pointer
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef uintptr_t UintPtr;

/**
 * @typedef PalAllocateFn
 * @brief Function pointer type used for CPU memory allocations.
 *
 * Allocates `size` bytes with `alignment` and return a pointer to the memory
 * block on success or nullptr on failure.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[in] size Size in bytes to allocate.
 * @param[in] alignment Must be power of two. It is recommended that it should
 * atleast be 8 or 16. Set to 0 to use default.
 *
 * @return Pointer to the allocated memory block on success or nullptr if
 * allocation failed.
 *
 * @sa PalAllocator
 * @sa PalFreeFn
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    Uint64 size,
    Uint64 alignment);

/**
 * @typedef PalFreeFn
 * @brief Function pointer type used for CPU memory deallocations.
 *
 * Deallocates memory allocated by PalAllocateFn. Passing nullptr or an invalid
 * pointer must be safe.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 * @param[in] ptr Pointer to the memory block to free. This function should do
 * nothing if `ptr` is nullptr.
 *
 * @sa PalAllocateFn
 * @sa PalAllocator
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * @typedef PalLogCallback
 * @brief Function pointer type used for log callbacks.
 *
 * This provides control over how log messages are handled (eg. redirect the log
 * message to a file). The `msg` is only valid for the duration of the callback,
 * copy if you need to store it.
 *
 * @param userData Optional pointer to user data. Can be nullptr.
 * @param msg A constant null-terminated UTF-8 encoding string containing the
 * message.
 *
 * @sa PalLogger
 * @sa palLog()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);

/**
 * @enum PalResult
 * @brief Codes returned by most PAL functions to specify success or failure.
 * This is not a bitmask.
 *
 * `PAL_RESULT_SUCCESS` code means the operation completed successfully. Any
 * other value indicates an error.
 *
 * @note All results follow the format `PAL_RESULT_**` for consistency and API
 * use.
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef enum {
    /** Operation was successful.*/
    PAL_RESULT_SUCCESS,

    /** One or more pointers were nullptr or invalid.*/
    PAL_RESULT_NULL_POINTER,

    /** One or more arguments were invalid.*/
    PAL_RESULT_INVALID_PARAMETER,

    /** Out of memory.*/
    PAL_RESULT_OUT_OF_MEMORY,

    /** An error occured on the plaform (OS) level.*/
    PAL_RESULT_PLATFORM_FAILURE,

    /** A partially defined custom allocator.*/
    PAL_RESULT_INVALID_ALLOCATOR,

    /** Platform (OS) denied PAL access.*/
    PAL_RESULT_ACCESS_DENIED,

    /** Timeout expired.*/
    PAL_RESULT_TIMEOUT,

    /** The buffer provided for write operation is too small.*/
    PAL_RESULT_INSUFFICIENT_BUFFER,

    /** An invalid thread.*/
    PAL_RESULT_INVALID_THREAD,

    /** An unsupported thread feature was used.*/
    PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED,

    /** Video system is not initialized.*/
    PAL_RESULT_VIDEO_NOT_INITIALIZED,

    /** An invalid monitor.*/
    PAL_RESULT_INVALID_MONITOR,

    /** An invalid monitor display mode.*/
    PAL_RESULT_INVALID_MONITOR_MODE,

    /** An invalid window.*/
    PAL_RESULT_INVALID_WINDOW,

    /** An unsupported video feature was used.*/
    PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED,

    /** An invalid keycode.*/
    PAL_RESULT_INVALID_KEYCODE,

    /** An invalid scancode.*/
    PAL_RESULT_INVALID_SCANCODE,

    /** An invalid mouse button.*/
    PAL_RESULT_INVALID_MOUSE_BUTTON,

    /** An invalid orientation.*/
    PAL_RESULT_INVALID_ORIENTATION,

    /** Opengl system is not initialized.*/
    PAL_RESULT_GL_NOT_INITIALIZED,

    /** An invalid opengl window.*/
    PAL_RESULT_INVALID_GL_WINDOW,

    /** An unsupported opengl extension was used.*/
    PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED,

    /** An invalid opengl framebuffer config.*/
    PAL_RESULT_INVALID_GL_FBCONFIG,

    /** The requested opengl version is not supported.*/
    PAL_RESULT_INVALID_GL_VERSION,

    /** The requested opengl profile is not supported.*/
    PAL_RESULT_INVALID_GL_PROFILE,

    /** An invalid opengl context.*/
    PAL_RESULT_INVALID_GL_CONTEXT
} PalResult;

/**
 * @struct PalVersion
 * @brief Describes the version of the PAL runtime.
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef struct {
    /** Major version (breaking changes).*/
    Uint32 major;

    /** Minor version (adding features).*/
    Uint32 minor;

    /** Build version (bug fixes). */
    Uint32 build;
} PalVersion;

/**
 * @struct PalAllocator
 * @brief Describes a user provided CPU memory allocator.
 *
 * Allows the user to override the default CPU allocator for PAL.
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef struct {
    /** CPU memory allocation function pointer.*/
    PalAllocateFn allocate;

    /** CPU memory deallocation function pointer.*/
    PalFreeFn free;

    /** Optional pointer to user data passed into allocation functions. Can be
     * nullptr.*/
    void* userData;
} PalAllocator;

/**
 * @struct PalLogger
 * @brief Describes a user provided logger for redirecting log messages.
 *
 * Allows the user to override the default log output.
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
typedef struct {
    /** Log callback for log redirecting.*/
    PalLogCallback callback;

    /** Optional pointer to user data passed into log callback. Can be
     * nullptr.*/
    void* userData;
} PalLogger;

/**
 * @brief Get the runtime version of PAL.
 *
 * This returns a copy of PALVersion struct, which contains the major, minor and
 * build of PAL runtime.
 *
 * @return A copy of PAL runtime version.
 *
 * @note This function is thread-safe and may be called from any thread.
 *
 * @sa palGetVersionString()
 * @sa PalVersion
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
PAL_API PalVersion PAL_CALL palGetVersion();

/**
 * @brief Get the human readable string representation of PAL runtime.
 *
 * This returns a constant, null-terminated UTF-8 encoding string describing the
 * version in the format: `1.2.3`, where `1` is the major, `2` is the minor and
 * `3` is the build respectively.
 *
 * @return Pointer to the null-terminated UTF-8 encoding string.
 *
 * @note This function is thread-safe and may be called from any thread.
 * @note The returned pointer must not be freed.
 *
 * @sa palGetVersion()
 * @sa PalVersion
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
PAL_API const char* PAL_CALL palGetVersionString();

/**
 * @brief Return a string description for the specified result.
 *
 * This returns a constant, null-terminated UTF-8 encoding string describing the
 * result.
 *
 * @param[in] result The result code
 * @return Pointer to the null-terminated UTF-8 encoding string.
 *
 * @note This function is thread-safe and may be called from any thread.
 * @note The returned pointer must not be freed.
 *
 * @sa PalResult
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
PAL_API const char* PAL_CALL palFormatResult(PalResult result);

/**
 * @brief Allocate CPU memory with a custom or default allocator.
 *
 * If a valid allocator is provided, its `allocate` function is called with the
 * provided size and user data. Otherwise, the default allocator is used.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator
 * will be used if it is nullptr.
 * @param[in] size Size in bytes to allocate.
 * @param[in] alignment Must be power of two. It is recommended that it should
 * atleast be 8 or 16. Set to 0 to use default.
 *
 * @return Pointer to the allocated CPU memory block on success or nullptr if
 * allocation failed.
 *
 * @note This does not initialize the allocated memory.
 * @note This function is thread safe and may be called from any thread if the
 * provided allocator is thread safe. The default allocator is thread safe.
 *
 * @sa palFree()
 * @sa PalAllocator
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment);

/**
 * @brief Deallocate CPU memory with a custom or default allocator.
 *
 * If a valid allocator is provided, its `free` function is called with the
 * provided memory block and user data. Otherwise, the default allocator is
 * used.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator
 * will be used if it is a nullptr.
 * @param[in] ptr Pointer to the memory block to free.
 *
 * @note This function is thread safe and may be called from any thread if the
 * provided allocator is thread safe. The default allocator is thread safe.
 *
 * @sa PalAllocator
 * @sa palAllocate()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

/**
 * @brief Log a formatted message with a custom or default logger.
 *
 * If the provided logger does not have a valid `callback`, PAL will discard it
 * and use its default logger. Unicode characters are supported.
 *
 * @param[in] logger Optional pointer to a logger. The default logger will be
 * used if it is nullptr.
 * @param[in] fmt UTF-8 encoding printf-style format string.
 *
 * Example:
 * @code
 * palLog(&myLogger, "%s - %f", string,
 * float);
 * @endcode
 *
 * @note This function is thread-safe and may be called from any thread.
 * @note PAL default logger will not log if there is no console. On Windows, if
 * there is no console, PAL will log to the debug console.
 *
 * @sa PalLogger
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
PAL_API void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...);

/**
 * @brief Get the high-resolution performance value.
 *
 * The counter is monotonically increasing while the system is running and is
 * typically used for precise time measurements.
 *
 * @return The high-resolution counter value.
 *
 * @note This function is thread-safe and may be called from any thread.
 *
 * @sa palGetPerformanceFrequency()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
PAL_API Uint64 PAL_CALL palGetPerformanceCounter();

/**
 * @brief Get the frequency of the high-resolution performance counter.
 *
 * This function returns the frequency of the high-resolution performance
 * counter in counts per second.
 *
 * @return The high-resolution performance frequency in counts per second.
 *
 * @note This function is thread-safe and may be called from any thread.
 * @note The returned frequency is constant during the lifetime of the
 * application.
 *
 * @sa palGetPerformanceCounter()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
PAL_API Uint64 PAL_CALL palGetPerformanceFrequency();

/**
 * @brief Combine two 32-bit unsigned integers into a single 64-bit signed
 * integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * @note This function is thread-safe and may be called from any thread.
 *
 * @sa palPackInt32()
 * @sa palUnpackUint32()
 * @sa palUnpackInt32()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
static inline Int64 PAL_CALL palPackUint32(
    Uint32 low,
    Uint32 high)
{
    return (Int64)(((Uint64)high << 32) | (Uint64)low);
}

/**
 * @brief Combine two 32-bit signed integers into a 64-bit signed integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * @note This function is thread-safe and may be called from any thread.
 *
 * @sa palPackInt32()
 * @sa palUnpackUint32()
 * @sa palUnpackInt32()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
static inline Int64 PAL_CALL palPackInt32(
    Int32 low,
    Int32 high)
{
    return ((Int64)(Uint32)high << 32) | (Uint32)low;
}

/**
 * @brief Pack a pointer into a 64-bit signed integer.
 *
 * @return The packed 64-bit signed integer.
 *
 * @note This function is thread-safe and may be called from any thread.
 *
 * @sa palUnpackPointer()
 *
 * @since Added in version 1.0.0.
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
 * @note This function is thread-safe and may be called from any thread.
 *
 * @sa palPackUint32()
 * @sa palUnpackInt32()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
static inline void PAL_CALL palUnpackUint32(
    Int64 data,
    Uint32* outLow,
    Uint32* outHigh)
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
 * @note This function is thread-safe if `outLow` and `outHigh` are thread
 * local.
 *
 * @sa palUnpackInt32()
 * @sa palPackUint32()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
static inline void PAL_CALL palUnpackInt32(
    Int64 data,
    Int32* outLow,
    Int32* outHigh)
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
 * @note This function is thread-safe and may be called from any thread.
 *
 * @sa palPackPointer()
 *
 * @since Added in version 1.0.0.
 * @ingroup core
 */
static inline void* PAL_CALL palUnpackPointer(Int64 data)
{
    return (void*)(UintPtr)data;
}

#endif // _PAL_CORE_H