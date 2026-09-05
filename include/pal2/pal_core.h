
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
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

/**
 * @defgroup pal_core Core
 * @brief PAL Core Functionality and API.
 */

/** @{ */

#ifndef PAL_CORE_H
#define PAL_CORE_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
#define PAL_EXTERN_C extern "C"
#else
#define PAL_EXTERN_C
#define nullptr ((void*)0)
#endif // __cplusplus

/** Set up shared library dependencies */
#ifdef _WIN32
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllexport)
#else
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllimport)
#endif // PAL_EXPORT
#else
/** Other platforms */
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
 * A sentinel representing an infinite time period.
 * 
 * @since 2.0
 */
#define PAL_INFINITE UINT32_MAX

/** 
 * Represents `true` or `1`.
 * 
 * @since 2.0
 */
#define PAL_TRUE 1

/** 
 * Represents `false` or `0`.
 * 
 * @since 2.0
 */
#define PAL_FALSE 0

/**
 * Operation completed successfully.
 * 
 * This is the only value that can be checked directly with standard checks.
 * (eg. result == `PAL_RESULT_SUCCESS`).
 * 
 * @since 2.0
 */
#define PAL_RESULT_SUCCESS 0

/** 
 * No result code.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_NONE 0

/** 
 * The suupplied argument is invalid.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_INVALID_ARGUMENT 1

/** 
 * Memory allocation failed.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_OUT_OF_MEMORY 2

/** 
 * The operation failed due to a platform specific error.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_PLATFORM_FAILURE 3

/** 
 * The operation did not complete within the specified time.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_TIMEOUT 4

/** 
 * The supplied handle is invalid.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_INVALID_HANDLE 5

/** 
 * The requested feature or feature used is not supported.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED 6

/** 
 * The operation performed is invalid for the context.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_INVALID_OPERATION 7

/** 
 * The device has been lost.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_DEVICE_LOST 8

/** 
 * The supplied handle out of date.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_OUT_OF_DATE 9

/** 
 * A sentinel representing the count of result codes.
 * 
 * @since 2.0
 */
#define PAL_RESULT_CODE_COUNT 10

/** 
 * No result source.
 * 
 * @since 2.0
 */
#define PAL_RESULT_SOURCE_NONE 0

/** 
 * Result native code is from win32 `GetLastError()`.
 * 
 * @since 2.0
 */
#define PAL_RESULT_SOURCE_WIN32 1

/** 
 * Result native code is from posix `errno`.
 * 
 * @since 2.0
 */
#define PAL_RESULT_SOURCE_POSIX 2

/** 
 * Result native code is from egl `eglGetError()`.
 * 
 * @since 2.0
 */
#define PAL_RESULT_SOURCE_EGL 3

/** 
 * Result native code is from vulkan `VkResult`.
 * 
 * @since 2.0
 */
#define PAL_RESULT_SOURCE_VULKAN 4

/** 
 * Result native code is from d3d12 `HRESULT`.
 * 
 * @since 2.0
 */
#define PAL_RESULT_SOURCE_D3D12 5

/** 
 * Result native code is from metal `NSError`.
 * 
 * @since 2.0
 */
#define PAL_RESULT_SOURCE_METAL 6

/** 
 * A sentinel representing the count of result sources.
 * 
 * @since 2.0
 */
#define PAL_RESULT_SOURCE_COUNT 7

/**
 * A boolean type
 * 
 * @since 2.0
 */
typedef uint32_t PalBool;

/**
 * A value returned by most PAL functions.
 * 
 * This value constains the PAL result code, the result source and 
 * the native code itself. 
 * 
 * The result source and native code are optional.
 * 
 * @since 2.0
 */
typedef uint64_t PalResult;

/**
 * Result codes from a result value.
 * 
 * The result code of a `PAL_RESULT_SUCCESS` value will always be 
 * `PAL_RESULT_CODE_NONE.`
 * 
 * @since 2.0
 * @sa palGetResultCode
 */
typedef uint16_t PalResultCode;

/**
 * Result sources from a result value.
 * 
 * The result source of a `PAL_RESULT_SUCCESS` value will always be 
 * `PAL_RESULT_SOURCE_NONE`.
 * 
 * @since 2.0
 * @sa palGetResultSource
 */
typedef uint16_t PalResultSource;

/**
 * Type used for memory allocations.
 * 
 * The callback must allocate atleast `size` with the requested `alignment`.
 * If the requested alignment is `0`, the callback is required to allocate the
 * memory with a default alignment.
 * 
 * If `size` is `0`, the callback may define its own behavior. The callback may
 * define its own behavior of wether to initialize the allocated memory.
 *
 * @param[in] userData User data passed from `PalAllocator::userData`. 
 * Can be `nullptr`.
 * @param[in] size Number of bytes to allocate.
 * @param[in] alignment The alignment.
 *
 * @return The allocated memory on success or `nullptr` on failure. 
 *
 * @since 2.0
 * @sa PalFreeFn
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    uint64_t size,
    uint64_t alignment);

/**
 * Type used for memory deallocations.
 * 
 * The memory must have been allocated by the corresponding allocation
 * callback and must not have been freed.
 * 
 * If the `ptr` is `nulltr` the callback must do nothing.
 * 
 * @param[in] userData User data passed from `PalAllocator::userData`. 
 * Can be `nullptr`.
 * @param[in] ptr The memory to free.
 *
 * @since 2.0
 * @sa PalAllocateFn
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * Type used for log callbacks.
 * 
 * The message is only valid for the duration of the callback and must not be
 * modified or freed by the callback, the memory is owned by PAL.
 * 
 * The callback may be called concurrently from multiple threads. The callback
 * must be thread safe if the same callback is used by multiple threads or a
 * seperate callback must be provided for each thread.
 *
 * @param userData User data passed from `PalLogger::userData`. Can be `nullptr`.
 * @param msg Null-terminated UTF-8 log message.
 *
 * @since 2.0
 * @sa palLog
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);

/**
 * Contains information about the version of PAL.
 * 
 * The version consists of a major version, minor version and build number.
 * 
 * Uninitialized fields may result in undefined behavior.
 * 
 * @since 2.0
 */
typedef struct PalVersion 
{
    /** The major version. This is incremented for breaking changes.*/
    uint32_t major;

    /**
     * The minor version. This is incremented for backward-compatible
     * additions or features.
     */
    uint32_t minor;

    /** The build version. This is incremented for bug fixes.*/
    uint32_t build;
} PalVersion;

/**
 * Contains information about a memory allocator.
 * 
 * This struct provides a way to use a custom allocator with PAL. 
 * Some APIs are thread safe, therefore the custom allocator must be thread
 * safe if those APIs will be used. 
 * 
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 2.0
 */
typedef struct PalAllocator 
{
    /** Allocate function. Must not be `nullptr`.*/
    PalAllocateFn allocate;

    /** Free function. Must not be `nullptr`.*/
    PalFreeFn free;

    /** User data passed to allocate and free functions. Can be `nullptr`.*/
    void* userData;
} PalAllocator;

/**
 * Contains information about a logger.
 * 
 * This struct provides a way to use custom loggers with PAL. This allows
 * interception of log messages made through the log API. The logger may
 * be called concurrently from multiple threads, therefore it must
 * be thread safe or each thread having its own logger.
 *
 * Uninitialized fields may result in undefined behavior.
 * 
 * @since 2.0
 */
typedef struct PalLogger {
    /** Callback function. Must not be `nullptr`.*/
    PalLogCallback callback;

    /** User data passed to callback. Can be `nullptr`.*/
    void* userData;
} PalLogger;

/**
 * Convert a result value to a human-readable string.
 *
 * @param result The result value to format.
 * @param bufferSize The size of the buffer. Must not be `0`.
 * @param buffer The buffer to write to. The string will be truncated if
 * `bufferSize` is insufficient.
 *
 * Thread safety: `buffer` must be per thread.
 *
 * @since 2.0
 */
PAL_API void PAL_CALL palFormatResult(
    PalResult result,
    uint64_t bufferSize,
    char* buffer);

/**
 * Retrieve the PAL runtime version number.
 *
 * @param version The struct to recieve the runtime version. Must not be
 * `nullptr`.
 *
 * Thread safety: `version` must be per thread.
 *
 * @since 2.0
 * @sa palGetVersionString
 */
PAL_API void PAL_CALL palGetVersion(PalVersion* version);

/**
 * Retrieve the PAL runtime version as a string.
 *
 * @return Null-terminated string containing the PAL runtime version. The 
 * returned string is owned by PAL and must not be freed or modified.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palGetVersion
 */
PAL_API const char* PAL_CALL palGetVersionString(void);

/**
 * Allocate memory using a custom or default allocator.
 * 
 * The default allocator is thread safe. 
 *
 * @param allocator The allocator. `nullptr` to use the default allocator.
 * @param size Number of bytes to allocate. A size of `0` is 
 * implementation-defined.
 * @param alignment Must be power of two. An alignment of `0` uses the 
 * implementation-defined default.
 *
 * @return Allocated memory on success, or `nullptr` on failure.
 *
 * Thread safety: `allocator` implementation must be thread safe.
 *
 * @since 2.0
 * @sa palFree
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    uint64_t size,
    uint64_t alignment);

/**
 * Free memory allocated by palAllocate.
 * 
 * The default allocator is thread safe. Freeing an already freed memory 
 * is undefined behavior. The function does not set the freed memory to 
 * `nullptr`, its better to do that to prevent double frees.
 *
 * @param allocator The allocator used to allocate the memory. `nullptr` for
 * the default allocator.
 * @param ptr Memory to free.
 *
 * Thread safety: `allocator` implementation must be thread safe.
 *
 * @since 2.0
 * @sa palAllocate
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

/**
 * Log a formatted message to a custom or default logger.
 * 
 * The default logger is thread safe.
 *
 * @param logger Logger instance. `nullptr` to use the default logger.
 * @param fmt printf-style format string.
 * @param ... Arguments for the format string.
 *
 * Thread safety: Thread safe, but log output and
 * callbacks may be invoked concurrently. The user must ensure the callback
 * implementation is thread safe.
 *
 * @since 2.0
 * @sa palFormatResult
 */
PAL_API void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...);

/**
 * Retrieve the current high-resolution performance counter value.
 *
 * @return Current monotonically increasing performance counter value.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palGetPerformanceFrequency
 */
PAL_API uint64_t PAL_CALL palGetPerformanceCounter(void);

/**
 * Retrieve the frequency of the high-resolution performance counter.
 *
 * @return Performance counter frequency, in counts per second.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palGetPerformanceCounter
 */
PAL_API uint64_t PAL_CALL palGetPerformanceFrequency(void);

/**
 * Get the result code from the result value.
 *
 * @param result The result value.
 *
 * @return The result code from the result value.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
static inline PalResultCode PAL_CALL palGetResultCode(PalResult result)
{
    return (uint16_t)(result & 0xFFFFU);
}

/**
 * Get the result source from the result value.
 *
 * @param result The result value.
 *
 * @return The result source from the result value.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
static inline PalResultSource PAL_CALL palGetResultSource(PalResult result)
{
    return (uint16_t)((result >> 16) & 0xFFFFu);
}

/**
 * Get the result native code from the result value.
 *
 * @param result The result value.
 *
 * @return The result native code from the result value.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
static inline uint32_t PAL_CALL palGetResultNativeCode(PalResult result)
{
    return (uint32_t)(result >> 32);
}

/**
 * Create a result value.
 * 
 * If `nativeCode` is not `0` and the result source is `PAL_RESULT_SOURCE_NONE`,
 * it will be ignored when formatting the result value.
 * 
 * Creating the result value with `code`, `source` and `nativeCode` as 
 * `PAL_RESULT_CODE_NONE`, `PAL_RESULT_SOURCE_NONE` and `0` respectively is valid,
 * but that will creating a result value same as `PAL_RESULT_SUCCESS`.
 *
 * @param code The result code.
 * @param source The result source.
 * @param nativeCode The result native code.
 *
 * @return The created result value.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
static inline PalResult PAL_CALL palMakeResult(
    PalResultCode code,
    PalResultSource source,
    uint32_t nativeCode)
{
    return ((uint64_t)nativeCode << 32) | ((uint64_t)source << 16) | (uint64_t)code;
}

/**
 * @brief Combine two 32-bit unsigned integers into a single 64-bit unsigned integer.
 *
 * @return The combined 64-bit unsigned integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palUnpackUint32
 */
static inline uint64_t PAL_CALL palPackUint32(
    uint32_t low,
    uint32_t high)
{
    return (uint64_t)(((uint64_t)high << 32) | (uint64_t)low);
}

/**
 * @brief Combine two 32-bit signed integers into a single 64-bit unsigned integer.
 *
 * @return The combined 64-bit unsigned integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palUnpackInt32
 */
static inline uint64_t PAL_CALL palPackInt32(
    int32_t low,
    int32_t high)
{
    return ((uint64_t)(uint32_t)high << 32) | (uint32_t)low;
}

/**
 * @brief Pack a pointer into a 64-bit unsigned integer.
 *
 * @return The packed 64-bit unsigned integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palUnpackPointer
 */
static inline uint64_t PAL_CALL palPackPointer(void* ptr)
{
    return (uint64_t)(uintptr_t)ptr;
}

/**
 * @brief Combine two floats into a single 64-bit unsigned integer.
 *
 * @return The combined 64-bit unsigned integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palUnpackFloat
 */
static inline uint64_t PAL_CALL palPackFloat(
    float low,
    float high)
{
    uint64_t combined = 0;
#if PAL_BIG_ENDIAN
    memcpy(&((uint32_t*)&combined)[0], &high, sizeof(float));
    memcpy(&((uint32_t*)&combined)[1], &low, sizeof(float));
#else
    memcpy(&((uint32_t*)&combined)[0], &low, sizeof(float));
    memcpy(&((uint32_t*)&combined)[1], &high, sizeof(float));
#endif // PAL_BIG_ENDIAN

    return combined;
}

/**
 * @brief Retrieve two 32-bit unsigned integers from a 64-bit unsigned integer.
 *
 * @param[out] outLow Low value of the 64-bit unsigned integer.
 * @param[out] outHigh High value of the 64-bit unsigned integer.
 *
 * Thread safety: `outLow` and `outHigh` must be per thread.
 *
 * @since 2.0
 * @sa palPackUint32
 */
static inline void PAL_CALL palUnpackUint32(
    uint64_t data,
    uint32_t* outLow,
    uint32_t* outHigh)
{
    if (outLow) {
        *outLow = (uint32_t)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (uint32_t)(data >> 32);
    }
}

/**
 * @brief Retrieve two 32-bit signed integers from a 64-bit unsigned integer.
 *
 * @param[out] outLow Low value of the 64-bit unsigned integer.
 * @param[out] outHigh High value of the 64-bit unsigned integer.
 *
 * Thread safety: `outLow` and `outHigh` must be per thread.
 *
 * @since 2.0
 * @sa palPackInt32
 */
static inline void PAL_CALL palUnpackInt32(
    uint64_t data,
    int32_t* outLow,
    int32_t* outHigh)
{
    if (outLow) {
        *outLow = (int32_t)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (int32_t)(data >> 32);
    }
}

/**
 * @brief Unpack a pointer from a 64-bit unsigned integer.
 *
 * @return The pointer from the 64-bit unsigned integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 * @sa palPackPointer
 */
static inline void* PAL_CALL palUnpackPointer(uint64_t data)
{
    return (void*)(uintptr_t)data;
}

/**
 * @brief Retrieve two floats from a 64-bit unsigned integer.
 *
 * @param[out] outLow Low value of the 64-bit unsigned integer.
 * @param[out] outHigh High value of the 64-bit unsigned integer.
 *
 * Thread safety: `outLow` and `outHigh` must be per thread.
 *
 * @since 2.0
 * @sa palPackFloat
 */
static inline void PAL_CALL palUnpackFloat(
    uint64_t data,
    float* low,
    float* high)
{
#if PAL_BIG_ENDIAN
    if (low) {
        memcpy(low, &((uint32_t*)&data)[1], sizeof(float));
    }

    if (high) {
        memcpy(high, &((uint32_t*)&data)[0], sizeof(float));
    }
#else
    if (low) {
        memcpy(low, &((uint32_t*)&data)[0], sizeof(float));
    }

    if (high) {
        memcpy(high, &((uint32_t*)&data)[1], sizeof(float));
    }

#endif // PAL_BIG_ENDIAN
}

/** @} */

#endif // PAL_CORE_H