
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pal_core Core
 * @ingroup pal_core
 * @{
 */

#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
#define PAL_EXTERN_C extern "C"
#else
#define PAL_EXTERN_C
#define nullptr ((void*)0)
#endif // __cplusplus

// Set up shared library dependencies
#ifdef _WIN32
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllexport)
#else
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllimport)
#endif // PAL_EXPORT
#else
// other plafforms
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

#define PAL_TRUE 1
#define PAL_FALSE 0
#define PAL_INFINITE UINT32_MAX

#define PAL_RESULT_SUCCESS 0
#define PAL_RESULT_INVALID_ARGUMENT 1
#define PAL_RESULT_OUT_OF_MEMORY 2
#define PAL_RESULT_PLATFORM_FAILURE 3
#define PAL_RESULT_TIMEOUT 4
#define PAL_RESULT_INVALID_HANDLE 5
#define PAL_RESULT_FEATURE_NOT_SUPPORTED 6
#define PAL_RESULT_NOT_INITIALIZED 7
#define PAL_RESULT_INVALID_OPERATION 8
#define PAL_RESULT_DEVICE_LOST 9
#define PAL_RESULT_OUT_OF_DATE 10

/**
 * @typedef PalBool
 * @brief Must be `PAL_TRUE` or `PAL_FALSE`.
 * 
 * @since 2.0
 */
typedef uint32_t PalBool;

/**
 * @typedef PalResult
 * @brief Value returned by most PAL functions.
 * 
 * Non-success results (eg. `PAL_RESULT_INVALID_HANDLE`) may contain
 * additional information for debugging and logging purposes. For checking specific
 * result codes, call `palGetResultCode()` to get the code from the result value. 
 * 
 * Example: 
 * 
 * uint16_t resultCode = palGetResultCode(result);
 * 
 * if (resultCode == `PAL_RESULT_INVALID_DEVICE_LOST`) {}.
 *
 * All result codes follow the format `PAL_RESULT_**` for consistency and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalResult;

/**
 * @typedef PalAllocateFn
 * @brief Function pointer type used for memory allocations.
 *
 * @param[in] userData Optional pointer to user data passed from ::PalAllocator. Can be nullptr.
 * @param[in] size Number of bytes to allocate. Must not be 0.
 * @param[in] alignment Must be power of two. Set to 0 to use default (16).
 *
 * @return Pointer to the allocated memory on success or nullptr on failure.
 *
 * @since 1.0
 * @sa PalFreeFn
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    uint64_t size,
    uint64_t alignment);

/**
 * @typedef PalFreeFn
 * @brief Function pointer type used for memory deallocations.
 *
 * @param[in] userData Optional pointer to user data passed from ::PalAllocator. Can be nullptr.
 * @param[in] ptr Pointer to memory previously allocated by PalAllocateFn. Must return safely if
 * pointer is nullptr.
 *
 * @since 1.0
 * @sa PalAllocateFn
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * @typedef PalLogCallback
 * @brief Function pointer type used for log callbacks.
 *
 * @param userData Optional pointer to user data passed from ::PalLogger. Can be nullptr.
 * @param msg Null-terminated UTF-8 log message.
 *
 * @since 1.0
 * @sa palLog
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);

/**
 * @struct PalVersion
 * @brief Describes the version of PAL.
 *
 * @since 1.0
 */
typedef struct {
    uint32_t major; /**< Major version (breaking changes).*/
    uint32_t minor; /**< Minor version (adding features).*/
    uint32_t build; /**< Build version (bug fixes).*/
} PalVersion;

/**
 * @struct PalAllocator
 * @brief Custom memory allocator.
 *
 * Provides user-defined memory allocation and free functions.
 *
 * @since 1.0
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
 */
typedef struct {
    PalLogCallback callback;
    void* userData; /** Optional user-provided data. Can be nullptr.*/
} PalLogger;

/**
 * Get the result code from the result value.
 * 
 * `PAL_RESULT_SUCCESS` code can be compared with the result value without comparing the
 * result code. 
 * 
 * @param result The result value.
 *
 * @return The result code from the result value.
 *
 * Thread safety: Thread safe.
 *
 * @since 2.0
 */
PAL_API uint16_t PAL_CALL palGetResultCode(PalResult result);

/**
 * Convert a result value to a human-readable string.
 * 
 * This returns a null-terminated string. The string is truncated if `bufferSize` is insufficient.
 *
 * @param result The PalResult value to format.
 * @param bufferSize The size of the buffer.
 * @param buffer The buffer.
 *
 * Thread safety: Thread safe if buffer is per thread.
 *
 * @since 2.0
 */
PAL_API void PAL_CALL palFormatResult(
    PalResult result, 
    uint64_t bufferSize,
    char* buffer);

/**
 * Retrieve the PAL version number.
 * 
 * @param version Pointer to PalVersion struct to fill.
 *
 * Thread safety: Thread safe if version is per thread.
 *
 * @since 2.0
 * @sa palGetVersionString
 */
PAL_API void PAL_CALL palGetVersion(PalVersion* version);

/**
 * Retrieve the PAL version string.
 *
 * @return Null-terminated string containing the PAL version.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetVersion
 */
PAL_API const char* PAL_CALL palGetVersionString();

/**
 * Allocate memory using the provided allocator.
 *
 * @param allocator The allocator to use. Set to nullptr to use default.
 * @param size Number of bytes to allocate.
 * @param alignment Alignment in bytes. Must be a power of two.
 *
 * @return Pointer to allocated memory on success, or nullptr on failure.
 *
 * Thread safety: Thread safe only if the provided allocator is thread safe. The default allocator
 * is thread safe.
 *
 * @since 1.0
 * @sa palFree
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    uint64_t size,
    uint64_t alignment);

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
 * @sa palAllocate
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

/**
 * Log a formatted message.
 *
 * @param logger Logger instance. Set to nullptr to use default logger.
 * @param fmt printf-style format string.
 * @param ... Arguments for the format string.
 *
 * Thread safety: Thread safe, but log output and
 * callbacks may be invoked concurrently. The user must ensure the callback
 * implementation is thread safe.
 *
 * @since 1.0
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
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetPerformanceFrequency
 */
PAL_API uint64_t PAL_CALL palGetPerformanceCounter();

/**
 * Query the frequency of the high-resolution performance counter.
 *
 * @return Performance counter frequency, in counts per second.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetPerformanceCounter
 */
PAL_API uint64_t PAL_CALL palGetPerformanceFrequency();

/**
 * @brief Combine two 32-bit unsigned integers into a single 64-bit signed
 * integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palUnpackUint32
 */
static inline int64_t PAL_CALL palPackUint32(
    uint32_t low,
    uint32_t high)
{
    return (int64_t)(((uint64_t)high << 32) | (uint64_t)low);
}

/**
 * @brief Combine two 32-bit signed integers into a single 64-bit signed
 * integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palUnpackInt32
 */
static inline int64_t PAL_CALL palPackInt32(
    int32_t low,
    int32_t high)
{
    return ((int64_t)(uint32_t)high << 32) | (uint32_t)low;
}

/**
 * @brief Pack a pointer into a 64-bit signed integer.
 *
 * @return The packed 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palUnpackPointer
 */
static inline int64_t PAL_CALL palPackPointer(void* ptr)
{
    return (int64_t)(uintptr_t)ptr;
}

/**
 * @brief Combine two floats into a single 64-bit signed integer.
 *
 * @return The combined 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.3
 * @sa palUnpackFloat
 */
static inline int64_t PAL_CALL palPackFloat(
    float low,
    float high)
{
    int64_t combined = 0;
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
 * @brief Retrieve two 32-bit unsigned integers from a 64-bit signed integer.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palPackUint32
 */
static inline void PAL_CALL palUnpackUint32(
    int64_t data,
    uint32_t* outLow,
    uint32_t* outHigh)
{
    if (outLow) {
        *outLow = (uint32_t)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (uint32_t)((uint64_t)data >> 32);
    }
}

/**
 * @brief Retrieve two 32-bit signed integers from a 64-bit signed integer.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * Thread safety: Thread-safe if @c outLow and @c outHigh are
 * thread local.
 *
 * @since 1.0
 * @sa palPackInt32
 */
static inline void PAL_CALL palUnpackInt32(
    int64_t data,
    int32_t* outLow,
    int32_t* outHigh)
{
    if (outLow) {
        *outLow = (int32_t)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (int32_t)((uint64_t)data >> 32);
    }
}

/**
 * @brief Unpack a pointer from a 64-bit signed integer.
 *
 * @return The pointer from the 64-bit signed integer.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palPackPointer
 */
static inline void* PAL_CALL palUnpackPointer(int64_t data)
{
    return (void*)(uintptr_t)data;
}

/**
 * @brief Retrieve two floats from a 64-bit signed integer.
 *
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 *
 * Thread safety: Thread-safe if @c outLow and @c outHigh are
 * thread local.
 *
 * @since 1.3
 * @sa palPackFloat
 */
static inline void PAL_CALL palUnpackFloat(
    int64_t data,
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

#endif // _PAL_CORE_H
