
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup thread Thread section
 * @ingroup pal_thread
 * @{
 */

#ifndef _THREAD_THREAD_H
#define _THREAD_THREAD_H

#include "pal/core/defines.h"
#include "pal/core/memory.h"
#include "pal/core/result.h"

/**
 * @struct PalThread
 * @brief Opaque handle to a thread.
 *
 * @since 1.0
 */
typedef struct PalThread PalThread;

/**
 * @typedef PalThreadFn
 * @brief Function pointer type used for thread entry function.
 *
 * @param[in] arg Optional pointer to user data. Can be nullptr.
 *
 * @return The return value of the thread as a pointer.
 *
 * @since 1.0
 */
typedef void* (*PalThreadFn)(void* arg);

/**
 * @typedef PalThreadFeatures
 * @brief Thread system features.
 *
 * All thread features follow the format `PAL_THREAD_FEATURE_**` for
 * consistency and API use.
 *
 * @since 1.0
 */
typedef enum {
    PAL_THREAD_FEATURE_STACK_SIZE = (1ULL << 0),
    PAL_THREAD_FEATURE_PRIORITY = (1ULL << 1),
    PAL_THREAD_FEATURE_AFFINITY = (1ULL << 2),
    PAL_THREAD_FEATURE_NAME = (1ULL << 3)
} PalThreadFeatures;

/**
 * @typedef PalThreadPriority
 * @brief Thread priority types. This is not a bitmask enum.
 *
 * All thread priority types follow the format `PAL_THREAD_PRIORITY_**` for
 * consistency and API use.
 *
 * @since 1.0
 */
typedef enum {
    PAL_THREAD_PRIORITY_LOW,
    PAL_THREAD_PRIORITY_NORMAL,
    PAL_THREAD_PRIORITY_HIGH
} PalThreadPriority;

/**
 * @struct PalThreadCreateInfo
 * @brief Creation parameters for a thread.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since 1.0
 */
typedef struct {
    uint64_t stackSize;              /**< Set to 0 to use default*/
    const PalAllocator* allocator; /**< Set to nullptr to use default.*/
    PalThreadFn entry;             /**< Thread entry function*/
    void* arg;                     /**< Optional user-provided data. Can be nullptr.*/
} PalThreadCreateInfo;

/**
 * @brief Create a new thread.
 *
 * The allocator field in the provided PalThreadCreateInfo struct will not
 * be copied, therefore the pointer must remain valid until the win is
 * detached. Detach the thread with palDetachThread() when no
 * longer needed.
 *
 * The created thread starts executing from the entry function. The thread runs
 * until the entry function has finished executing or its detached.
 *
 * @param[in] info Pointer to a PalThreadCreateInfo struct that specifies
 * parameters. Must not be nullptr.
 * @param[out] outThread Pointer to a PalThread to recieve the created
 * thread.  Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if the provided allocator is
 * thread safe and `outThread` is per thread. The default allocator is
 * thread safe.
 *
 * @since 1.0
 * @sa palDetachThread
 */
PAL_API PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** outThread);

/**
 * @brief Wait for the provided thread to finish executing.
 *
 * After the thread is done executing, it is freed automatically and
 * must not be used anymore nor detached.
 *
 * @param[in] thread Pointer to the thread.
 * @param[out] retval Optionally pointer to get the threads exit value.
 * Pass the address of the pointer. Internally it will be reinterpreted into a
 * pointer-to-pointer. This is for ABI stability.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `retval` is per thread.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palJoinThread(
    PalThread* thread,
    void* retval);

/**
 * @brief Release the thread's resources and destroy it.
 *
 * Must be called when the thread is done executing.
 * After this call, the thread cannot be attached or used anymore.
 * If the thread is invalid or nullptr, this function returns silently.
 *
 * This must not be called on a thread that has been attached.
 *
 * @param[in] thread Pointer to the thread to detach.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API void PAL_CALL palDetachThread(PalThread* thread);

/**
 * @brief Suspend the calling thread for the provided duration of milliseconds.
 *
 * @param[in] milliseconds Number of milliseconds to sleep.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API void PAL_CALL palSleep(uint64_t milliseconds);

/**
 * @brief Yield the remainder of the calling threads time sliced,
 * allowing other threads of equal priority to run.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API void PAL_CALL palYield();

/**
 * @brief Get the current executing thread.
 *
 * @return The current thread on success or nullptr on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API PalThread* PAL_CALL palGetCurrentThread();

/**
 * @brief Get the supported features of PAL thread system.
 *
 * This is based on the platform (OS) features not a single created thread.
 *
 * @return The thread features on success or 0 on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API PalThreadFeatures PAL_CALL palGetThreadFeatures();

/**
 * @brief Get the priority of the provided thread.
 *
 * `PAL_THREAD_FEATURE_PRIORITY` must be supported.
 *
 * @param[in] thread The thread to query priority for.
 *
 * @return The thread priority on success or 0 on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread);

/**
 * @brief Get the affinity of the provided thread.
 *
 * `PAL_THREAD_FEATURE_AFFINITY` must be supported. Thread affinity is the
 * number of CPU cores the thread is allowed to be executed on.
 *
 * @param[in] thread The thread to query affinity for.
 *
 * @return The thread affinity on success or 0 on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API uint64_t PAL_CALL palGetThreadAffinity(PalThread* thread);

/**
 * @brief Get the name of the provided thread.
 *
 * `PAL_THREAD_FEATURE_NAME` must be supported.
 * fails. Set the buffer to nullptr to get the size of the thread name in bytes.
 *
 * If the size of the provided buffer is less than the actual size of thread
 * name, PAL will write upto that limit.
 *
 *
 * @param[in] thread The thread to query its name.
 * @param[in] bufferSize Size of the provided buffer in bytes.
 * @param[out] outSize The actual size of the thread name in bytes.
 * @param[out] outBuffer Pointer to a user provided buffer to recieve the name.
 * Can be nullptr.
 *
 * Thread safety: Thread safe.
 *
 * @note On Linux: Thread names are limited to 16 characters including the null
 * terminator.
 *
 * @note On MacOS: Thread names are limited to 64 characters including the null
 * terminator.
 *
 * @since 1.0
 * @sa palSetThreadName
 */
PAL_API PalResult PAL_CALL palGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer);

/**
 * @brief Set the priority of the provided thread.
 *
 * `PAL_THREAD_FEATURE_PRIORITY` must be supported.
 *
 * @param[in] thread The thread to set priority for.
 * @param[in] priority The new thread priority.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * @return
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority);

/**
 * @brief Set the affinity of the provided thread.
 *
 * `PAL_THREAD_FEATURE_AFFINITY` must be supported.
 * Thread affinity is the number of CPU cores the thread is allowed to be
 * executed on.
 *
 * To be safe, get the number of CPU cores and use that to build the CPU mask.
 * Example: we set a thread to the first and second CPU core.
 *
 * @code
 * uint64_t cpuMask = (1ULL << 0) | (1ULL << 1).
 * @endcode
 *
 * @param[in] thread The thread to set affinity for.
 * @param[in] mask The CPU core mask.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 */
PAL_API PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    uint64_t mask);

/**
 * @brief Set the name of the provided thread.
 *
 * `PAL_THREAD_FEATURE_NAME` must be supported.
 * The thread name will be visible in debuggers and the Task Manager (Windows).
 *
 * @param[in] thread The thread
 * @param[in] name UTF-8 null terminated string.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe.
 *
 * @note On Linux: Thread names are limited to 16 characters including the null
 * terminator.
 *
 * @note On MacOS: Thread names are limited to 64 characters including the null
 * terminator.
 *
 * @since 1.0
 * @sa palGetThreadName
 */
PAL_API PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name);

#endif // _THREAD_THREAD_H

/** @} */