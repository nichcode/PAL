
/**
 * @file pal_core.h
 * @brief This is the header file for PAL Core API.
 *
 * It defines all the types and functions of the core system.
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
 */

/** @{ */

#ifndef PAL_CORE_H
#define PAL_CORE_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
/**
 * @brief This is used to specify the C linkage for the API declarations.
 *
 * @since Added in version 2.0
 */
#define PAL_EXTERN_C extern "C"
#else
/**
 * @brief This is used to specify the C linkage for the API declarations.
 *
 * @since Added in version 2.0
 */
#define PAL_EXTERN_C

/**
 * @brief Represents `NULL`.
 *
 * @since Added in version 2.0
 */
#define nullptr ((void*)0)
#endif // __cplusplus

#ifdef _WIN32
/**
 * @brief This is used to specify PAL calling convention.
 *
 * @since Added in version 2.0
 */
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
/**
 * @brief This is used to specify PAL API symbol.
 *
 * @since Added in version 2.0
 */
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllexport)
#else
/**
 * @brief This is used to specify PAL API symbol.
 *
 * @since Added in version 2.0
 */
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllimport)
#endif // PAL_EXPORT
#else
/**
 * @brief This is used to specify PAL calling convention.
 *
 * @since Added in version 2.0
 */
#define PAL_CALL
#ifdef _PAL_EXPORT
/**
 * @brief This is used to specify PAL API symbol.
 *
 * @since Added in version 2.0
 */
#define PAL_DECLSPEC PAL_EXTERN_C __attribute__((visibility("default")))
#else
/**
 * @brief This is used to specify PAL API symbol.
 *
 * @since Added in version 2.0
 */
#define PAL_DECLSPEC PAL_EXTERN_C
#endif // PAL_EXPORT
#endif // _WIN32

#ifdef _PAL_BUILD_DLL
/**
 * @brief This is used to specify PAL API functions attributes.
 *
 * @since Added in version 2.0
 */
#define PAL_API PAL_EXTERN_C PAL_DECLSPEC
#else
/**
 * @brief This is used to specify PAL API functions attributes.
 *
 * @since Added in version 2.0
 */
#define PAL_API PAL_EXTERN_C
#endif // _PAL_BUILD_DLL

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
/**
 * @brief Indicates whether a system uses big-endian byte order.
 *
 * @since Added in version 2.0
 */
#define PAL_BIG_ENDIAN 1
#else
/**
 * @brief Indicates whether a system uses big-endian byte order.
 *
 * @since Added in version 2.0
 */
#define PAL_BIG_ENDIAN 0
#endif // __ORDER_BIG_ENDIAN__

/**
 * @brief Represents an infinite time period.
 *
 * @since Added in version 2.0
 */
#define PAL_INFINITE UINT32_MAX

/**
 * @brief Represents `true` or `1`.
 *
 * @since Added in version 2.0
 */
#define PAL_TRUE 1

/**
 * @brief Represents `false` or `0`.
 *
 * @since Added in version 2.0
 */
#define PAL_FALSE 0

/**
 * @brief Operation completed successfully.
 *
 * This is returned by a function if it was successful without any errors.
 *
 * This is the only value that can be checked directly with standard checks.
 * (eg. result == `PAL_RESULT_SUCCESS`).
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SUCCESS 0

/**
 * @brief No result code.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_NONE 0

/**
 * @brief The suupplied argument is invalid.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_INVALID_ARGUMENT 1

/**
 * @brief Memory allocation failed.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_OUT_OF_MEMORY 2

/**
 * @brief The operation failed due to a platform specific error.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_PLATFORM_FAILURE 3

/**
 * @brief The operation did not complete within the specified time.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_TIMEOUT 4

/**
 * @brief The supplied handle is invalid.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_INVALID_HANDLE 5

/**
 * @brief The requested feature or feature used is not supported.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED 6

/**
 * @brief The operation performed is invalid for the context.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_INVALID_OPERATION 7

/**
 * @brief The device has been lost.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_DEVICE_LOST 8

/**
 * @brief The supplied handle out of date.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_OUT_OF_DATE 9

/**
 * @brief A value representing the count of result codes.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_CODE_COUNT 10

/**
 * @brief No result source.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SOURCE_NONE 0

/**
 * @brief Result native code is from win32 `GetLastError()`.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SOURCE_WIN32 1

/**
 * @brief Result native code is from posix `errno`.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SOURCE_POSIX 2

/**
 * @brief Result native code is from egl `eglGetError()`.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SOURCE_EGL 3

/**
 * @brief Result native code is from vulkan `VkResult`.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SOURCE_VULKAN 4

/**
 * @brief Result native code is from d3d12 `HRESULT`.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SOURCE_D3D12 5

/**
 * @brief Result native code is from metal `NSError`.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SOURCE_METAL 6

/**
 * @brief A value representing the count of result sources.
 *
 * @since Added in version 2.0
 */
#define PAL_RESULT_SOURCE_COUNT 7

/**
 * @brief A boolean type.
 *
 * Must be `PAL_TRUE` or `PAL_FALSE`.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalBool;

/**
 * @brief Value returned by most PAL functions.
 *
 * This value constains the PAL result code, the result source and
 * the native code itself. If a function completed successfully, it returns
 * `PAL_RESULT_SUCCESS`.
 *
 * All values of this type follow the format `PAL_RESULT_*` for API consistency
 * and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint64_t PalResult;

/**
 * @brief Result codes from a result value.
 *
 * The result code of a `PAL_RESULT_SUCCESS` value will always be
 * `PAL_RESULT_CODE_NONE.`
 *
 * All values of this type follow the format `PAL_RESULT_CODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @sa palGetResultCode
 */
typedef uint16_t PalResultCode;

/**
 * @brief Result sources from a result value.
 *
 * The result source of a `PAL_RESULT_SUCCESS` value will always be
 * `PAL_RESULT_SOURCE_NONE`.
 *
 * All values of this type follow the format `PAL_RESULT_SOURCE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @sa palGetResultSource
 */
typedef uint16_t PalResultSource;

/**
 * @brief Function pointer type used for memory allocations.
 *
 * The callback must allocate atleast `size` parameter with the requested
 * `alignment` parameter or return `nullptr` if the allocation failed.
 * If the requested alignment is `0`, the callback must determines the
 * alignment to use for the allocation.
 *
 * If the requested size is `0`, the callback must determines the behavior. The
 * callback must decide whether to initialize the allocated memory or not.
 * PAL does not filter any requested size or alignment.
 *
 * PAL uses this callback to allocate all of its internal memory, this means
 * if a PAL function is thread safe and it will be called from multiple
 * threads, the callback implementation must also be thread safe.
 *
 * If a PAL function which uses the callback fails, it will deallocate the
 * memory it allocated using the same alloator's free callback.
 *
 * @param[in] userData User data passed from `PalAllocator::userData`.
 * Can be `nullptr`.
 * @param[in] size Number of bytes to allocate.
 * @param[in] alignment The alignment.
 *
 * @return The allocated memory on success or `nullptr` on failure.
 *
 * @since Added in version 2.0
 * @sa PalFreeFn
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    uint64_t size,
    uint64_t alignment);

/**
 * @brief Function pointer type used for memory deallocations.
 *
 * The memory must have been allocated by the corresponding allocation
 * callback and must not have been deallocated. The callback will not
 * be called if the `ptr` parameter is `nullptr`.
 *
 * @param[in] userData User data passed from `PalAllocator::userData`.
 * Can be `nullptr`.
 * @param[in] ptr The memory to free.
 *
 * @since Added in version 2.0
 * @sa PalAllocateFn
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * @brief Function pointer type used for log callbacks.
 *
 * The message is only valid for the duration of the callback and must not be
 * modified or freed by the callback, the memory is owned by PAL.
 *
 * The callback may be called concurrently from multiple threads. The callback
 * must be thread safe if the same callback is used by multiple threads or a
 * seperate callback must be provided for each thread.
 *
 * @param userData User data passed from `PalLogger::userData`. Can be
 * `nullptr`.
 * @param msg Null-terminated UTF-8 log message.
 *
 * @since Added in version 2.0
 * @sa palLog
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);

/**
 * @brief Contains information about the version of PAL.
 *
 * The version consists of a major version, minor version and build number.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 */
typedef struct PalVersion {
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
 * @brief Contains information about a memory allocator.
 *
 * This struct provides a way to use a custom allocator with PAL.
 * Some APIs are thread safe, therefore the custom allocator must be thread
 * safe if those APIs will be used.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 */
typedef struct PalAllocator {
    /** Allocate function. Must not be `nullptr`.*/
    PalAllocateFn allocate;

    /** Free function. Must not be `nullptr`.*/
    PalFreeFn free;

    /** User data passed to allocate and free functions. Can be `nullptr`.*/
    void* userData;
} PalAllocator;

/**
 * @brief Contains information about a logger.
 *
 * This struct provides a way to use custom loggers with PAL. This allows
 * interception of log messages made through the log API. The logger may
 * be called concurrently from multiple threads, therefore it must
 * be thread safe or each thread having its own logger.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 */
typedef struct PalLogger {
    /** Callback function. Must not be `nullptr`.*/
    PalLogCallback callback;

    /** User data passed to callback. Can be `nullptr`.*/
    void* userData;
} PalLogger;

/**
 * @brief Converts a result value to a human-readable string.
 *
 * @param[in] result The result value to format.
 * @param[in] bufferSize The size of the buffer. Must not be `0`.
 * @param[out] buffer The buffer to write to. The string will be truncated if
 * `bufferSize` is insufficient.
 *
 * @Thread-safety `buffer` parameter must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palFormatResult(
    PalResult result,
    uint64_t bufferSize,
    char* buffer);

/**
 * @brief Retrieves the PAL runtime version number.
 *
 * @param[out] version The struct to recieve the runtime version. Must not be
 * `nullptr`.
 *
 * @Thread-safety `version` must be per thread.
 *
 * @since Added in version 2.0
 * @sa palGetVersionString
 */
PAL_API void PAL_CALL palGetVersion(PalVersion* version);

/**
 * @brief Retrieves the PAL runtime version as a string.
 *
 * @return Null-terminated string containing the PAL runtime version. The
 * returned string is owned by PAL and must not be freed or modified.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palGetVersion
 */
PAL_API const char* PAL_CALL palGetVersionString(void);

/**
 * @brief Allocates memory using a custom or default allocator.
 *
 * This function allocate atleast `size` parameter of memory with the requested
 * `alignment` parameter. If the requested alignment is `0`, an
 * implementation default will be used.
 *
 * If the requested size is `0`, the behavior is implementation defined.
 * Therefore its recommended the caller avoids `0` size allocations.
 *
 * If allocations will be made from multiple threads, the `allocator`
 * parameter must be thread safe. The default allocator is thread safe.
 *
 * @param[in] allocator The allocator. `nullptr` to use the default allocator.
 * @param[in] size Number of bytes to allocate. A size of `0` is
 * implementation-defined.
 * @param[in] alignment Must be power of two. An alignment of `0` uses the
 * implementation-defined default.
 *
 * @return Allocated memory on success, or `nullptr` on failure.
 *
 * @Thread-safety `allocator` implementation must be thread safe.
 *
 * @since Added in version 2.0
 * @sa palFree
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    uint64_t size,
    uint64_t alignment);

/**
 * @brief Deallocates memory allocated by palAllocate.
 *
 * Deallocating memory that has been deallocated will cause undefined behavior.
 * This function does not set the s`ptr` parameter to `nullptr` after
 * deallocation. It is recommended the caller does that after this call to
 * prevent any double deallocations.
 *
 * If deallocations will be made from multiple threads, the `allocator`
 * parameter must be thread safe. The default allocator is thread safe.
 *
 * @param[in] allocator The allocator used to allocate the memory. `nullptr` for
 * the default allocator. The default allocator is thread safe.
 * @param[in] ptr Memory to free. Must be valid.
 *
 * @Thread-safety `allocator` implementation must be thread safe.
 *
 * @since Added in version 2.0
 * @sa palAllocate
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

/**
 * @brief Logs a formatted message to a custom or default logger.
 *
 * @param[in] logger Logger instance. `nullptr` to use the default logger. The
 * default logger is thread safe.
 * @param[in] fmt printf-style format string.
 * @param[in] ... Arguments for the format string.
 *
 * @Thread-safety Thread safe, but log output and
 * callbacks may be invoked concurrently. The user must ensure the callback
 * implementation is thread safe.
 *
 * @since Added in version 2.0
 * @sa palFormatResult
 */
PAL_API void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...);

/**
 * @brief Retrieves the current high-resolution performance counter value.
 *
 * @return Current monotonically increasing performance counter value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palGetPerformanceFrequency
 */
PAL_API uint64_t PAL_CALL palGetPerformanceCounter(void);

/**
 * @brief Retrieves the frequency of the high-resolution performance counter.
 *
 * @return Performance counter frequency, in counts per second.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palGetPerformanceCounter
 */
PAL_API uint64_t PAL_CALL palGetPerformanceFrequency(void);

/**
 * @brief Gets the result code from the result value.
 *
 * @param[in] result The result value.
 *
 * @return The result code from the result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
static inline PalResultCode PAL_CALL palGetResultCode(PalResult result)
{
    return (uint16_t)(result & 0xFFFFU);
}

/**
 * @brief Gets the result source from the result value.
 *
 * @param[in] result The result value.
 *
 * @return The result source from the result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
static inline PalResultSource PAL_CALL palGetResultSource(PalResult result)
{
    return (uint16_t)((result >> 16) & 0xFFFFu);
}

/**
 * @brief Gets the result native code from the result value.
 *
 * @param[in] result The result value.
 *
 * @return The result native code from the result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
static inline uint32_t PAL_CALL palGetResultNativeCode(PalResult result)
{
    return (uint32_t)(result >> 32);
}

/**
 * @brief Creates a result value.
 *
 * If `nativeCode` is not `0` and the result source is `PAL_RESULT_SOURCE_NONE`,
 * it will be ignored when formatting the result value.
 *
 * Creating the result value with `code`, `source` and `nativeCode` as
 * `PAL_RESULT_CODE_NONE`, `PAL_RESULT_SOURCE_NONE` and `0` respectively is
 * valid, but that will creating a result value same as `PAL_RESULT_SUCCESS`.
 *
 * @param[in] code The result code.
 * @param[in] source The result source.
 * @param[in] nativeCode The result native code.
 *
 * @return The created result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
static inline PalResult PAL_CALL palMakeResult(
    PalResultCode code,
    PalResultSource source,
    uint32_t nativeCode)
{
    return ((uint64_t)nativeCode << 32) | ((uint64_t)source << 16) |
           (uint64_t)code;
}

/**
 * @brief Combines two 32-bit unsigned integers into a single 64-bit unsigned
 * integer.
 * 
 * @param[in] low The low 32-bit unsigned integer.
 * @param[in] high The high 32-bit unsigned integer.
 *
 * @return The combined 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palUnpackUint32
 */
static inline uint64_t PAL_CALL palPackUint32(
    uint32_t low,
    uint32_t high)
{
    return (uint64_t)(((uint64_t)high << 32) | (uint64_t)low);
}

/**
 * @brief Combines two 32-bit signed integers into a single 64-bit unsigned
 * integer.
 * 
 * @param[in] low The low 32-bit signed integer.
 * @param[in] high The high 32-bit signed integer.
 *
 * @return The combined 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palUnpackInt32
 */
static inline uint64_t PAL_CALL palPackInt32(
    int32_t low,
    int32_t high)
{
    return ((uint64_t)(uint32_t)high << 32) | (uint32_t)low;
}

/**
 * @brief Packs a pointer into a 64-bit unsigned integer.
 * 
 * @param[in] ptr The pointer to pack.
 *
 * @return The packed 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palUnpackPointer
 */
static inline uint64_t PAL_CALL palPackPointer(void* ptr)
{
    return (uint64_t)(uintptr_t)ptr;
}

/**
 * @brief Combines two floats into a single 64-bit unsigned integer.
 * 
 * @param[in] low The low float value.
 * @param[in] high The high float value.
 *
 * @return The combined 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
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
 * @brief Retrieves two 32-bit unsigned integers from a 64-bit unsigned integer.
 *
 * @param[in] data The 64-bit unsigned integer.
 * @param[out] low Low value of the 64-bit unsigned integer.
 * @param[out] high High value of the 64-bit unsigned integer.
 *
 * @Thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
 * @sa palPackUint32
 */
static inline void PAL_CALL palUnpackUint32(
    uint64_t data,
    uint32_t* low,
    uint32_t* high)
{
    if (low) {
        *low = (uint32_t)(data & 0xFFFFFFFF);
    }

    if (high) {
        *high = (uint32_t)(data >> 32);
    }
}

/**
 * @brief Retrieves two 32-bit signed integers from a 64-bit unsigned integer.
 *
 * @param[in] data The 64-bit unsigned integer.
 * @param[out] low Low value of the 64-bit unsigned integer.
 * @param[out] high High value of the 64-bit unsigned integer.
 *
 * @Thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
 * @sa palPackInt32
 */
static inline void PAL_CALL palUnpackInt32(
    uint64_t data,
    int32_t* low,
    int32_t* high)
{
    if (low) {
        *low = (int32_t)(data & 0xFFFFFFFF);
    }

    if (high) {
        *high = (int32_t)(data >> 32);
    }
}

/**
 * @brief Unpacks a pointer from a 64-bit unsigned integer.
 *
 * @param[in] data The 64-bit unsigned integer.
 * @return The pointer from the 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palPackPointer
 */
static inline void* PAL_CALL palUnpackPointer(uint64_t data)
{
    return (void*)(uintptr_t)data;
}

/**
 * @brief Retrieves two floats from a 64-bit unsigned integer.
 *
 * @param[in] data The 64-bit unsigned integer.
 * @param[out] low Low value of the 64-bit unsigned integer.
 * @param[out] high High value of the 64-bit unsigned integer.
 *
 * @Thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
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