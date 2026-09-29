
/**
 * @file pal_core.h
 * @brief This is the header file for PAL Core API.
 *
 * It defines all the types and functions of the core module.
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
 * @defgroup pal_core Core Module
 * @{
 */

#ifndef PAL_CORE_H
#define PAL_CORE_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
#define PAL_EXTERN_C extern "C"
#else
#define PAL_EXTERN_C
#define nullptr ((void*)0) /**< `NULL` */
#endif // __cplusplus

#ifdef _WIN32
#define PAL_CALL __stdcall
#ifdef _PAL_EXPORT
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllexport)
#else
#define PAL_DECLSPEC PAL_EXTERN_C __declspec(dllimport)
#endif // PAL_EXPORT
#else
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
#define PAL_API PAL_EXTERN_C
#endif // _PAL_BUILD_DLL

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define PAL_BIG_ENDIAN 1
#else
#define PAL_BIG_ENDIAN 0
#endif // __ORDER_BIG_ENDIAN__

#define PAL_INFINITE UINT32_MAX /**< infinite time or number */
#define PAL_LOG_MSG_SIZE 4096 /**< maximum log buffer size */

#define PAL_TRUE 1 /**< `true` or `1` */
#define PAL_FALSE 0 /**< `false` or `0` */
#define PAL_RESULT_SUCCESS 0 /**< function completed successfully */

/**
 * @defgroup result_codes Result Codes
 * @{
 */
#define PAL_RESULT_CODE_NONE 0 /**< no result code */
#define PAL_RESULT_CODE_INVALID_ARGUMENT 1 /**< invalid argument was passed */
#define PAL_RESULT_CODE_OUT_OF_MEMORY 2 /**< memory allocation failed */
#define PAL_RESULT_CODE_PLATFORM_FAILURE 3 /**< platform-specific error not known to PAL */
#define PAL_RESULT_CODE_TIMEOUT 4 /**< timeout occured */
#define PAL_RESULT_CODE_INVALID_HANDLE 5 /**< invalid handle was passed */
#define PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED 6 /**< unsupported feature was used */
#define PAL_RESULT_CODE_INVALID_OPERATION 7 /**< invalid operation was performed */
#define PAL_RESULT_CODE_DEVICE_LOST 8 /**< device was lost */
#define PAL_RESULT_CODE_OUT_OF_DATE 9 /**< handle out of date */
#define PAL_RESULT_CODE_COUNT 10 /**< number of result codes */
/** @} */

/**
 * @defgroup result_sources Result Sources
 * @{
 */
#define PAL_RESULT_SOURCE_NONE 0 /**< no result source */
#define PAL_RESULT_SOURCE_WIN32 1 /**< win32 `GetLastError()` code */
#define PAL_RESULT_SOURCE_POSIX 2 /**< posix `errno` code */
#define PAL_RESULT_SOURCE_EGL 3 /**< egl `eglGetError()` code*/
#define PAL_RESULT_SOURCE_VULKAN 4 /**< vulkan `VkResult` code */
#define PAL_RESULT_SOURCE_D3D12 5 /**< d3d12 `HRESULT` code */
#define PAL_RESULT_SOURCE_METAL 6 /**< metal `NSError` code */
#define PAL_RESULT_SOURCE_COUNT 7 /**< number of result sources */
/** @} */

/**
 * @typedef PalBool
 * @brief Boolean type.
 * 
 * @since Added in version 2.0
 */
typedef uint32_t PalBool;

/**
 * @typedef PalResult
 * @brief Value returned by most PAL functions.
 * 
 * value returned by PAL functions which contains the result code, and optionally the
 * result source and the native code. The result source shows the origin of the native code.
 * 
 * @since Added in version 2.0
 */
typedef uint64_t PalResult;

/**
 * @typedef PalResultCode
 * @brief PAL result code.
 *
 * All values of this type follow the format `PAL_RESULT_CODE_*` for API consistency
 * and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint16_t PalResultCode;

/**
 * @typedef PalResultSource
 * @brief PAL result source.
 * 
 * All values of this type follow the format `PAL_RESULT_SOURCE_*` for API consistency
 * and ease of use.
 * 
 * @since Added in version 2.0
 */
typedef uint16_t PalResultSource;

/**
 * @typedef PalLibrarySymbol
 * @brief Generic library symbol.
 * 
 * @since Added in version 2.2
 */
typedef void (PAL_CALL *PalLibrarySymbol)(void);

/**
 * @typedef PalLibrary
 * @brief Opaque handle to a shared library.
 * 
 * @since Added in version 2.2
 */
typedef struct PalLibrary PalLibrary;

/**
 * @brief Memory allocation function.
 * 
 * The callback must allocate atleast `size` with the requested `alignment`
 * and return a pointer to the allocated memory. If allocation fails, the
 * callback must return `nullptr`. If the requested alignment is `0`,
 * the callback must use an implementation-defined default.
 * 
 * The callback may initialize the allocated memory. The behavior of a `0-size`
 * allocation is implementation defined. The requested alignment must be power
 * of two.
 * 
 * The function signature should look like this:
 * @code
 * void* PAL_CALL alloc(void* userData, uint64_t size, uint64_t alignment);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the callback or `nullptr`.
 * @param[in] size Number of bytes to allocate.
 * @param[in] alignment Alignment of the allocated memory.
 * @return Pointer to the allocated memory on success or `nullptr` on failure.
 *
 * @since Added in version 2.0
 * 
 * @sa PalFreeFn
 */
typedef void*(PAL_CALL* PalAllocateFn)(
    void* userData,
    uint64_t size,
    uint64_t alignment);

/**
 * @brief Memory deallocation function.
 * 
 * The callback must deallocate memory previously allocate by the corresponding
 * memory allocation function.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL free(void* userData, void* ptr);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the callback or `nullptr`.
 * @param[in] ptr Memory to free.
 *
 * @since Added in version 2.0
 * 
 * @sa PalAllocateFn
 */
typedef void(PAL_CALL* PalFreeFn)(
    void* userData,
    void* ptr);

/**
 * @brief Log callback function.
 * 
 * `msg` is only valid for the duration of the callback and must not be modified or freed
 * by the callback, the memory is owned by PAL.
 * 
 * The callback may be called concurrently from multiple threads, the implementation
 * must be thread safe if it will be used by multiple threads.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL logCallback(void* userData, const char* msg);
 * @endcode
 *
 * @param userData User-defined data passed to the callback or `nullptr`.
 * @param msg Null-terminated UTF-8 string containing the log message.
 *
 * @since Added in version 2.0
 * 
 * @sa palLog
 */
typedef void(PAL_CALL* PalLogCallback)(
    void* userData,
    const char* msg);
/** @} */

/**
 * @struct PalVersion
 * @brief PAL runtime version.
 * 
 * `major` is incremented when breaking changes are made.
 * `minor` is incremented when when backward-compatible features are added.
 * `build` is incremented when bugs are fixed without new additions or breaking changes made.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef struct PalVersion {
    uint32_t major; /**< major version */
    uint32_t minor; /**< minor version */
    uint32_t build; /**< patch version */
} PalVersion;

/**
 * @struct PalAllocator
 * @brief Memory allocator.
 * 
 * The allocator will be used by PAL to allocate its internal memory.
 * The allocator must be thread safe if it is used by PAL functions that
 * are called concurrently from multiple threads.
 * 
 * The allocator will not be copied, so it must remain valid for as long
 * as PAL may use it. The default allocator is thread safe and own by PAL.
 *
 * Uninitialized fields may result in undefined behavior.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef struct PalAllocator {
    PalAllocateFn allocate; /**< memory allocation function */
    PalFreeFn free; /**< memory deallocation function */
    void* userData; /**< user-defined data passed to callbacks or `nullptr` */
} PalAllocator;

/**
 * @struct PalLogger
 * @brief Logger.
 * 
 * The logger provides a way to intercept log messages made through the PAL log API.
 * The logger will not be copied, so it must remain valid for as long
 * as PAL may use it. The default logger is thread safe and own by PAL.
 * 
 * Uninitialized fields may result in undefined behavior.
 * 
 * @since Added in version 2.0
 * @ingroup pal_core
 */
typedef struct PalLogger {
    PalLogCallback callback; /**< log callback */
    void* userData; /**< user-defined data passed to callback or `nullptr` */
} PalLogger;

/**
 * @brief Converts a result value to a human-readable string.
 * 
 * This function converts a result value into a null-terminated UTF-8 encoded string.
 * The converted string will be truncated if `buffer` is insufficient.
 *
 * @param[in] result Result value.
 * @param[in] bufferSize Size of the buffer.
 * @param[out] buffer Output buffer to write to.
 *
 * @Thread-safety `buffer` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 */
PAL_API void PAL_CALL palFormatResult(
    PalResult result,
    uint64_t bufferSize,
    char* buffer);

/**
 * @brief Retrieves the PAL runtime version.
 * 
 * @param[out] version Output struct to recieve the PAL runtime version.
 *
 * @Thread-safety `version` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetVersionString
 */
PAL_API void PAL_CALL palGetVersion(PalVersion* version);

/**
 * @brief Retrieves the PAL runtime version as a string.
 * 
 * This function converts the PAL runtime version into a null-terminated UTF-8
 * encoded string. The returned string is owned by PAL and must not be modified
 * or freed. The returned string is formatted as `major.minor.patch`.
 *
 * @return Null-terminated string containing the PAL runtime version.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetVersion
 */
PAL_API const char* PAL_CALL palGetVersionString(void);

/**
 * @brief Allocates memory.
 * 
 * This function allocates memory with a custom or default allocator.
 * The allocator must remain valid for as long as memory allocated with it
 * are not freed. The allocated memory must be freed using the same allocator.
 * PAL does not validate this requirement, using a seperate allocator results
 * in undefined behavior.
 * 
 * This function allocates atleast `size` memory with the requested `alignment`.
 * The requested alignment must be power of two. If the requested alignment is `0`,
 * a default will be used. `0-size` allocations is implementation-defined.
 *
 * @param[in] allocator Allocator to use or `nullptr` for the default.
 * @param[in] size Number of bytes to allocate.
 * @param[in] alignment Alignment of the allocated memory.
 * @return Pointer to the allocated memory on success, or `nullptr` if allocation fails.
 *
 * @Thread-safety `allocator` implementation must be thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palFree
 */
PAL_API void* PAL_CALL palAllocate(
    const PalAllocator* allocator,
    uint64_t size,
    uint64_t alignment);

/**
 * @brief Deallocates memory.
 * 
 * This function deallocates memory with a custom or default allocator.
 * The memory must be freed using the same allocator used to allocate it.
 * PAL does not validate this requirement, using a seperate allocator results
 * in undefined behavior.
 * 
 * This function does not set `ptr` to `nullptr` after the memory is deallocated.
 * Passing a deallocated memory will results in undefined behavior. If the 
 * memory is `nullptr`, this function return silently.
 *
 * @param[in] allocator Allocator to use or `nullptr` for the default.
 * @param[in] ptr Memory to free.
 *
 * @Thread-safety `allocator` implementation must be thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palAllocate
 */
PAL_API void PAL_CALL palFree(
    const PalAllocator* allocator,
    void* ptr);

/**
 * @brief Logs a formatted message.
 * 
 * This function logs a formatted message to a custom or default logger.
 * Log messages are limited to `PAL_LOG_MSG_SIZE`, messages exceeding this
 * limit are truncated and the remaining characters are discarded.
 * 
 * Logging in a log callback with the default logger is valid but logging
 * in a log callback with a custom logger triggers recursive logging. PAL
 * guards against this and the whole log message will be discarded.
 *
 * @param[in] logger Logger to use or `nullptr` for the default.
 * @param[in] fmt printf-style format string.
 * @param[in] ... Arguments for the format string.
 *
 * @Thread-safety Thread safe, but log output and callbacks may be invoked concurrently.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palFormatResult
 */
PAL_API void PAL_CALL palLog(
    const PalLogger* logger,
    const char* fmt,
    ...);

/**
 * @brief Retrieves the performance counter.
 * 
 * This function retrieves the current high-resolution monotonically increasing
 * performance counter value.
 *
 * @return Current performance counter value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetPerformanceFrequency
 */
PAL_API uint64_t PAL_CALL palGetPerformanceCounter(void);

/**
 * @brief Retrieves the performance counter frequency.
 * 
 * This function retrieves the high-resolution performance counter frequency in
 * counts per second.
 *
 * @return Performance counter frequency.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetPerformanceCounter
 */
PAL_API uint64_t PAL_CALL palGetPerformanceFrequency(void);

/**
 * @brief Loads the shared library.
 * 
 * This function loads the specified shared library module dynamically into address
 * space. using the platforms search rules.`path` must have the library extension
 * appended to it and can be absolute or relative.
 * 
 * The specified module will load other modules if there is a dependency between them.
 * The library does not resolve its symbols after being loaded, a symbol is
 * resolved when @ref palGetSymbol() is called.
 * 
 * Calling the function with `path` set to `nullptr` is implementation-defined
 * behavior. The returned library must be freed with @ref palFreeLibrary().
 * 
 * @param[in] path The path to the library.
 * @return The loaded library on success or `nullptr` if the library failed to load.
 *
 * @Thread-safety The entry function must be thread-safe.
 *
 * @since Added in version 2.2
 * @ingroup pal_core
 * 
 * @sa palGetSymbol
 * @sa palFreeLibrary
 */
PAL_API PalLibrary* PAL_CALL palLoadLibrary(const char* path);

/**
 * @brief Retrieves a symbol from a library.
 * 
 * This function retrieves a symbol from the specified library. `library` must be
 * loaded into address space before this call.
 * 
 * Exported symbols are returned as `PalLibrarySymbol`, a cast is required to use
 * the symbol as its declared type.
 *
 * @param[in] library Library to retrieve the symbol from.
 * @param[in] name Null-terminated UTF-8 encoded name of the symbol.
 * 
 * @return the symbol on success or `nullptr` if the symbol was not found.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.2
 * @ingroup pal_core
 * 
 * @sa palLoadLibrary
 * @sa palFreeLibrary
 */
PAL_API PalLibrarySymbol PAL_CALL palGetSymbol(
    PalLibrary* library, 
    const char* name);

/**
 * @brief Unloads the specified library from address space.
 * 
 * This function unloads the specified library from address space and
 * invalidates all it symbols after this call.
 * 
 * @param[in] library Library to free.
 *
 * @Thread-safety `library` must be externally synchronized.
 *
 * @since Added in version 2.2
 * @ingroup pal_core
 * 
 * @sa palLoadLibrary
 * @sa palGetSymbol
 */
PAL_API void PAL_CALL palFreeLibrary(PalLibrary* library);

/**
 * @brief Gets the result code from the specified result value.
 *
 * @param[in] result Result value.
 * @return Result code.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetResultSource
 * @sa palGetResultNativeCode
 */
static inline PalResultCode PAL_CALL palGetResultCode(PalResult result)
{
    return (uint16_t)(result & 0xFFFFU);
}

/**
 * @brief Gets the result source from the specified result value.
 *
 * @param[in] result Result value.
 * @return Result source.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetResultCode
 * @sa palGetResultNativeCode
 */
static inline PalResultSource PAL_CALL palGetResultSource(PalResult result)
{
    return (uint16_t)((result >> 16) & 0xFFFFu);
}

/**
 * @brief Gets the result native code from the specified result value.
 *
 * @param[in] result Result value.
 * @return Result native code.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetResultCode
 * @sa palGetResultSource
 */
static inline uint32_t PAL_CALL palGetResultNativeCode(PalResult result)
{
    return (uint32_t)(result >> 32);
}

/**
 * @brief Creates a result value.
 * 
 * This function creates a result value with the specified result code, result
 * source and the native code. Creating a result value with the
 * result code, result source and native code set to @ref PAL_RESULT_CODE_NONE,
 * @ref PAL_RESULT_SOURCE_NONE and `0` respectively creates a result value
 * which is equal to @ref PAL_RESULT_SUCCESS.
 *
 * @param[in] code Result code.
 * @param[in] source Result source.
 * @param[in] nativeCode Native code.
 * @return Created result value.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palGetResultCode
 * @sa palGetResultSource
 * @sa palGetResultNativeCode
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
 * @brief Combines two 32-bit unsigned integers into a single 64-bit unsigned integer.
 * 
 * @param[in] low Low 32-bit unsigned integer.
 * @param[in] high High 32-bit unsigned integer.
 * @return Combined 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palUnpackUint32
 */
static inline uint64_t PAL_CALL palPackUint32(
    uint32_t low,
    uint32_t high)
{
    return (uint64_t)(((uint64_t)high << 32) | (uint64_t)low);
}

/**
 * @brief Combines two 32-bit signed integers into a single 64-bit unsigned integer.
 * 
 * @param[in] low Low 32-bit signed integer.
 * @param[in] high High 32-bit signed integer.
 * @return Combined 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
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
 * @param[in] ptr Pointer to pack.
 * @return Packed 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palUnpackPointer
 */
static inline uint64_t PAL_CALL palPackPointer(void* ptr)
{
    return (uint64_t)(uintptr_t)ptr;
}

/**
 * @brief Combines two floats into a single 64-bit unsigned integer.
 * 
 * @param[in] low Low float value.
 * @param[in] high High float value.
 * @return Combined 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palUnpackFloat
 */
static inline uint64_t PAL_CALL palPackFloat(
    float low,
    float high)
{
    uint64_t combined = 0;
#if PAL_BIG_ENDIAN
    memcpy(&combined, &high, sizeof(float));
    memcpy((char*)&combined + sizeof(float), &low, sizeof(float));
#else
    memcpy(&combined, &low, sizeof(float));
    memcpy((char*)&combined + sizeof(float), &high, sizeof(float));
#endif // PAL_BIG_ENDIAN

    return combined;
}

/**
 * @brief Retrieves two 32-bit unsigned integers from a 64-bit unsigned integer.
 *
 * @param[in] data 64-bit unsigned integer.
 * @param[out] low Output to recieve the low value.
 * @param[out] high Output to recieve the high value.
 *
 * @Thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
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
 * @param[in] data 64-bit unsigned integer.
 * @param[out] low Output to recieve the low value.
 * @param[out] high Output to recieve the high value.
 *
 * @Thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
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
 * @param[in] data 64-bit unsigned integer.
 * @return Pointer from the 64-bit unsigned integer.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palPackPointer
 */
static inline void* PAL_CALL palUnpackPointer(uint64_t data)
{
    return (void*)(uintptr_t)data;
}

/**
 * @brief Retrieves two floats from a 64-bit unsigned integer.
 * 
 * @param[in] data 64-bit unsigned integer.
 * @param[out] low Output to recieve the low value.
 * @param[out] high Output to recieve the high value.
 *
 * @Thread-safety `low` and `high` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_core
 * 
 * @sa palPackFloat
 */
static inline void PAL_CALL palUnpackFloat(
    uint64_t data,
    float* low,
    float* high)
{
#if PAL_BIG_ENDIAN
    if (low) {
        memcpy(low, (char*)&data + sizeof(float), sizeof(float));
    }

    if (high) {
        memcpy(high, &data, sizeof(float));
    }
#else
    if (low) {
        memcpy(low, &data, sizeof(float));
    }

    if (high) {
        memcpy(high, (char*)&data + sizeof(float), sizeof(float));
    }

#endif // PAL_BIG_ENDIAN
}

#endif // PAL_CORE_H