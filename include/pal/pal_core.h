
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
 * @defgroup pal_core Core
 * Core PAL functionality such as versioning, memory allocation, logging, and
 * timing.
 *
 * @{
 */

#ifndef _PAL_CORE_H
#define _PAL_CORE_H

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
 * @ingroup pal_core
 */
typedef _Bool bool;
#endif // __cplusplus

// Set up shared library dependencies
#ifdef _WIN32
#define PAL_CALL __stdcall
#ifdef _PAL_BUILD_DLL // shared library
#ifdef _PAL_EXPORT
#define PAL_API PAL_EXTERN_C __declspec(dllexport)
#else
#define PAL_API PAL_EXTERN_C __declspec(dllimport)
#endif // PAL_EXPORT 
#else
#define PAL_API PAL_EXTERN_C // static library 
#endif // _PAL_BUILD_DLL
#else
// other platforms
#define PAL_CALL
#ifdef _PAL_BUILD_DLL // shared library
#ifdef _PAL_EXPORT
#define PAL_API PAL_EXTERN_C __attribute__((visibility("default")))
#else
#define PAL_API PAL_EXTERN_C
#endif // PAL_EXPORT 
#else
#define PAL_API PAL_EXTERN_C // static library 
#endif // _PAL_BUILD_DLL
#endif // _WIN32

#define PAL_BIT(x) 1 << x

/**
 * @brief A signed 8-bit integer
 * @since 1.0
 * @ingroup pal_core
 */
typedef int8_t Int8;

/**
 * @brief A signed 16-bit integer
 * @since 1.0
 * @ingroup pal_core
 */
typedef int16_t Int16;

/**
 * @brief A signed 32-bit integer
 * @since 1.0
 * @ingroup pal_core
 */
typedef int32_t Int32;

/**
 * @brief A signed 64-bit integer
 * @since 1.0
 * @ingroup pal_core
 */
typedef int64_t Int64;

/**
 * @brief A signed 64-bit integer pointer
 * @since 1.0
 * @ingroup pal_core
 */
typedef intptr_t IntPtr;

/**
 * @brief An unsigned 8-bit integer
 * @since 1.0
 * @ingroup pal_core
 */
typedef uint8_t Uint8;

/**
 * @brief An unsigned 16-bit integer
 * @since 1.0
 * @ingroup pal_core
 */
typedef uint16_t Uint16;

/**
 * @brief An unsigned 32-bit integer
 * @since 1.0
 * @ingroup pal_core
 */
typedef uint32_t Uint32;

/**
 * @brief An unsigned 64-bit integer
 * @since 1.0
 * @ingroup pal_core
 */
typedef uint64_t Uint64;

/**
 * @brief An unsigned 64-bit integer pointer
 * @since 1.0
 * @ingroup pal_core
 */
typedef uintptr_t UintPtr;

/**
 * @typedef PalAllocateFn
 * @brief Function pointer type used for memory allocations.
 *
 * @param[in] userData Optional pointer to user data passed from ::PalAllocator.
 * Can be nullptr.
 * @param[in] size Number of bytes to allocate.
 * @param[in] alignment Must be power of two. Set to 0 to use default.
 *
 * @return Pointer to the allocated memory on success or nullptr on failure.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa PalFreeFn
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    Uint64 size,
    Uint64 alignment);

/**
 * @typedef PalFreeFn
 * @brief Function pointer type used for memory deallocations.
 *
 * @param[in] userData Optional pointer to user data passed from ::PalAllocator.
 * Can be nullptr.
 * @param[in] ptr Pointer to memory previously allocated by PalAllocateFn.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa PalAllocateFn
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * @typedef PalLogCallback
 * @brief Function pointer type used for log callbacks.
 *
 * @param userData Optional pointer to user data passed from ::PalLogger. Can be
 * nullptr.
 * @param msg Null-terminated UTF-8 log message.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palLog
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);

/**
 * @enum PalResult
 * @brief Codes returned by most PAL functions. This is not a bitmask.
 *
 * All result codes follow the format `PAL_RESULT_**` for consistency and API
 * use.
 *
 * @since 1.0
 * @ingroup pal_core
 */
typedef enum {
    PAL_RESULT_SUCCESS,
    PAL_RESULT_NULL_POINTER,
    PAL_RESULT_INVALID_ARGUMENT,
    PAL_RESULT_OUT_OF_MEMORY,
    PAL_RESULT_PLATFORM_FAILURE,
    PAL_RESULT_INVALID_ALLOCATOR,
    PAL_RESULT_ACCESS_DENIED,
    PAL_RESULT_TIMEOUT,
    PAL_RESULT_INSUFFICIENT_BUFFER,
    PAL_RESULT_INVALID_THREAD,
    PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED,
    PAL_RESULT_VIDEO_NOT_INITIALIZED,
    PAL_RESULT_INVALID_MONITOR,
    PAL_RESULT_INVALID_MONITOR_MODE,
    PAL_RESULT_INVALID_WINDOW,
    PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED,
    PAL_RESULT_INVALID_KEYCODE,
    PAL_RESULT_INVALID_SCANCODE,
    PAL_RESULT_INVALID_MOUSE_BUTTON,
    PAL_RESULT_INVALID_ORIENTATION,
    PAL_RESULT_GL_NOT_INITIALIZED,
    PAL_RESULT_INVALID_GL_WINDOW,
    PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED,
    PAL_RESULT_INVALID_GL_FBCONFIG,
    PAL_RESULT_INVALID_GL_VERSION,
    PAL_RESULT_INVALID_GL_PROFILE,
    PAL_RESULT_INVALID_GL_CONTEXT
} PalResult;

/**
 * @struct PalVersion
 * @brief Describes the version of PAL.
 *
 * @since 1.0
 * @ingroup pal_core
 */
typedef struct {
    Uint32 major; /**< Major version (breaking changes).*/
    Uint32 minor; /**< Minor version (adding features).*/
    Uint32 build; /**< Build version (bug fixes).*/
} PalVersion;

/**
 * @struct PalAllocator
 * @brief Custom memory allocator.
 *
 * Provides user-defined memory allocation and free functions.
 *
 * @since 1.0
 * @ingroup pal_core
 */
typedef struct {
    PalAllocateFn allocate;
    PalFreeFn free;
    void* userData; /**< Optional user-provided data. Can be nullptr.*/
} PalAllocator;

/**
 * @struct PalLogger
 * @brief Logging configuration.
 *
 * Provides a callback and user data for handling log messages.
 *
 * @since 1.0
 * @ingroup pal_core
 */
typedef struct {
    PalLogCallback callback;
    void* userData; /** Optional user-provided data. Can be nullptr.*/
} PalLogger;

/**
 * Retrieve the PAL version number.
 *
 * @return PAL version (major, minor, build).
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palGetVersionString
 */
PAL_API PalVersion PAL_CALL palGetVersion();

/**
 * Retrieve the PAL version string.
 *
 * @return Null-terminated string containing the PAL version.
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palGetVersion
 */
PAL_API const char* PAL_CALL palGetVersionString();

/**
 * Convert a result code to a human-readable string.
 *
 * @param result The PalResult code to format.
 *
 * @return Null-terminated static string describing the result.
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 */
PAL_API const char* PAL_CALL palFormatResult(PalResult result);

/**
 * Allocate memory using the provided allocator.
 *
 * @param allocator The allocator to use. Set to nullptr to use default.
 * @param size Number of bytes to allocate.
 * @param alignment Alignment in bytes. Must be a power of two.
 *
 * @return Pointer to allocated memory on success, or nullptr on failure.
 *
 * Thread safety: Thread safe only if the provided allocator is thread
 * safe. The default allocator is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palFree
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment);

/**
 * Free memory allocated by palAllocate.
 *
 * @param allocator The allocator used to allocate the memory. Set to nullptr to
 * use default.
 * @param ptr Pointer to memory to free. If nullptr, the function returns
 * silently.
 *
 * Thread safety: Thread safe only if the provided allocator is thread
 * safe. The default allocator is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palAllocate
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

/**
 * Log a formatted message.
 *
 * @param logger Logger instance. Must not be NULL.
 * @param fmt printf-style format string.
 * @param ... Arguments for the format string.
 *
 * Thread safety: This function is thread safe, but log output and
 * callbacks may be invoked concurrently. The user must ensure the callback
 * implementation is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palFormatResult
 */
PAL_API void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...);

/**
 * Query a high-resolution performance counter value.
 *
 * @return Current performance counter value.
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palGetPerformanceFrequency
 */
PAL_API Uint64 PAL_CALL palGetPerformanceCounter();

/**
 * Query the frequency of the high-resolution performance counter.
 *
 * @return Performance counter frequency, in counts per second.
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palGetPerformanceCounter
 */
PAL_API Uint64 PAL_CALL palGetPerformanceFrequency();

/**
 * @brief Combine two 32-bit unsigned integers into a single 64-bit signed
 * integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palUnpackUint32
 */
static inline Int64 PAL_CALL palPackUint32(
    Uint32 low,
    Uint32 high)
{
    return (Int64)(((Uint64)high << 32) | (Uint64)low);
}

/**
 * @brief Combine two 32-bit signed integers into a single 64-bit signed
 * integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palUnpackInt32
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
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palUnpackPointer
 */
static inline Int64 PAL_CALL palPackPointer(void* ptr)
{
    return (Int64)(UintPtr)ptr;
}

/**
 * @brief Retrieve two 32-bit unsigned integers from a 64-bit signed integer.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palPackUint32
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
 * @brief Retrieve two 32-bit signed integers from a 64-bit signed integer.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * Thread safety: This function is thread-safe if `outLow` and `outHigh` are
 * thread local.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palPackInt32
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
 * Thread safety: This function is thread safe.
 *
 * @since 1.0
 * @ingroup pal_core
 * @sa palPackPointer
 */
static inline void* PAL_CALL palUnpackPointer(Int64 data)
{
    return (void*)(UintPtr)data;
}

/** @} */ // end of pal_core group

#endif // _PAL_CORE_H