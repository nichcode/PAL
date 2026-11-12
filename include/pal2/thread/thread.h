
/**
 * @file thread.h
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

#ifndef PAL_THREAD_THREAD_H
#define PAL_THREAD_THREAD_H

#include "pal2/core/base.h"
#include "pal2/core/result.h"
#include "pal2/core/memory.h"

/**
 * @typedef PalThreadFeatures
 * @brief Thread system features.
 *
 * This is a bitmask of the features supported by the thread system.
 *
 * All values of this type follow the format `PAL_THREAD_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef uint32_t PalThreadFeatures;

/**
 * @typedef PalThreadPriority
 * @brief Thread priority.
 *
 * Identifies the scheduling priority of a thread.
 *
 * All values of this type follow the format `PAL_THREAD_PRIORITY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef uint32_t PalThreadPriority;

/**
 * @struct PalThread
 * @brief Opaque handle to a thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef struct PalThread PalThread;

/**
 * @defgroup thread_features Thread Features
 * @ingroup pal_thread
 * @{
 */
#define PAL_THREAD_FEATURE_STACK_SIZE (1U << 0) /**< Set thread stack size. */
#define PAL_THREAD_FEATURE_PRIORITY (1U << 1) /**< Get and set thread priority. */
#define PAL_THREAD_FEATURE_AFFINITY (1U << 2) /**< Get and set thread affinity. */
#define PAL_THREAD_FEATURE_NAME (1U << 3) /**< Get and set thread name. */
/** @} */

/**
 * @defgroup thread_priorities Thread Priorities
 * @ingroup pal_thread
 * @{
 */
#define PAL_THREAD_PRIORITY_LOW 0 /**< Lower than normal priority. */
#define PAL_THREAD_PRIORITY_NORMAL 1 /**< Default priority of newly created threads. */
#define PAL_THREAD_PRIORITY_HIGH 2 /**< Higher than normal priority. */
#define PAL_THREAD_PRIORITY_COUNT 3 /**< Number of thread priorities. */
/** @} */

/**
 * @typedef PalThreadFn
 * @brief Thread entry function.
 *
 * This is the application-defined function a thread starts executing from.
 * The thread finishes executing when this function returns.
 *
 * `arg` is owned by the application and must remain valid until the function no longer uses it.
 *
 * The function signature should look like this:
 * @code
 * void* PAL_CALL threadEntry(void* arg);
 * @endcode
 *
 * @param[in] arg User-defined data passed to the function, or `nullptr`.
 * @return The return value of the thread. It can be retrieved with @ref palJoinThread().
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef void* (PAL_CALL* PalThreadFn)(void* arg);

/**
 * @struct PalThreadCreateInfo
 * @brief Thread creation parameters.
 *
 * This struct is used only during @ref palCreateThread() and may be discarded
 * after the function returns.
 *
 * Setting `stackSize` to a value other than `0` requires
 * @ref PAL_THREAD_FEATURE_STACK_SIZE to be supported. `entry` must not be `nullptr`.
 *
 * The data `arg` points to is not copied and must remain valid until `entry` no longer uses it.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef struct PalThreadCreateInfo {
    uint64_t stackSize; /**< Thread stack size in bytes, or `0` for the platform default. */
    const PalAllocator* allocator; /**< Allocator to use, or `nullptr` for the default. */
    PalThreadFn entry; /**< Thread entry function. */
    void* arg; /**< User-defined data passed to `entry`, or `nullptr`. */
} PalThreadCreateInfo;

/**
 * @brief Creates a new thread.
 *
 * The created thread must either be joined with @ref palJoinThread() or
 * released with @ref palDetachThread().
 *
 * This function creates a thread using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy its contents.
 *
 * `info->allocator` is not copied. The allocator and any state referenced
 * by it must remain valid until the thread is done executing and has been
 * joined or detached.
 *
 * The created thread starts execution from the `info->entry` function. Thread
 * creation fails if the entry function is `nullptr`. The thread is created with
 * a priority of @ref PAL_THREAD_PRIORITY_NORMAL. Use
 * @ref palSetThreadPriority() to change it.
 *
 * A thread that is joined with @ref palJoinThread() is freed automatically once
 * it finishes executing. A thread that is not joined must be released with
 * @ref palDetachThread() after it has finished executing.
 *
 * @param[in] info Thread creation parameters.
 * @param[out] thread Output handle to receive the created thread.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `thread` must be per thread.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palJoinThread
 * @sa palDetachThread
 */
PAL_API PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread);

/**
 * @brief Waits for a thread.
 *
 * This function blocks the calling thread until the specified thread has
 * finished executing. Once the thread has finished, it is freed and must not
 * be used again or passed to @ref palDetachThread().
 *
 * @param[in] thread Thread to join.
 * @param[out] retval Output to receive the return value of the thread, or
 *                    `nullptr` to ignore it.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `retval` must be per thread. `thread` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_HANDLE
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palCreateThread
 */
PAL_API PalResult PAL_CALL palJoinThread(
    PalThread* thread,
    void** retval);

/**
 * @brief Releases a thread.
 *
 * This function releases the specified thread's resources and destroys it.
 * It must not be used with a thread that was joined with @ref palJoinThread().
 * Calling this function on a thread that has not finished executing is
 * undefined behavior. After this call, the thread must not be used.
 *
 * @param[in] thread Thread to detach.
 *
 * @thread-safety `thread` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palCreateThread
 */
PAL_API void PAL_CALL palDetachThread(PalThread* thread);

/**
 * @brief Suspends the calling thread.
 *
 * This function suspends the calling thread for the duration of `milliseconds`.
 *
 * @param[in] milliseconds Number of milliseconds to sleep.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
PAL_API void PAL_CALL palSleep(uint64_t milliseconds);

/**
 * @brief Yields the calling thread.
 *
 * This function yields the remainder of the calling thread's time slice,
 * allowing other threads of equal priority to run. The next thread is selected
 * by the platform.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
PAL_API void PAL_CALL palYield();

/**
 * @brief Gets the currently executing thread.
 *
 * @return The current thread on success or `nullptr` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
PAL_API PalThread* PAL_CALL palGetCurrentThread();

/**
 * @brief Gets the supported features of the thread system.
 *
 * This function may be called before any thread has been created.
 *
 * @return The thread features on success or `0` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
PAL_API PalThreadFeatures PAL_CALL palGetThreadFeatures();

/**
 * @brief Gets the priority of a thread.
 *
 * The @ref PAL_THREAD_FEATURE_PRIORITY feature must be supported.
 *
 * @param[in] thread Thread to get the priority from.
 * @return The thread priority on success or @ref PAL_THREAD_PRIORITY_NORMAL on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palSetThreadPriority
 */
PAL_API PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread);

/**
 * @brief Gets the affinity of a thread.
 *
 * The @ref PAL_THREAD_FEATURE_AFFINITY feature must be supported.
 *
 * @param[in] thread Thread to get the affinity from.
 * @return The thread affinity bitmask on success or `0` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palSetThreadAffinity
 */
PAL_API uint64_t PAL_CALL palGetThreadAffinity(PalThread* thread);

/**
 * @brief Gets the name of a thread.
 *
 * The @ref PAL_THREAD_FEATURE_NAME feature must be supported or this function
 * results in undefined behavior. Set `buffer` to `nullptr` to get the size
 * of the thread name in bytes. If `bufferSize` is smaller than the size of the
 * thread name, the name is truncated to fit the buffer.
 *
 * @param[in] thread Thread to get the name from.
 * @param[in] bufferSize Size of `buffer` in bytes.
 * @param[out] size Output to receive the size of the thread name in bytes,
 *                  or `nullptr` to ignore it.
 * @param[out] buffer Output buffer to write the thread name to, or `nullptr`
 *                    to only query the size.
 *
 * @thread-safety `buffer` must be per thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palSetThreadName
 */
PAL_API void PAL_CALL palGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer);

/**
 * @brief Sets the priority of a thread.
 *
 * The @ref PAL_THREAD_FEATURE_PRIORITY feature must be supported.
 *
 * @param[in] thread Thread to set the priority of.
 * @param[in] priority New thread priority (`PAL_THREAD_PRIORITY_*`).
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `thread` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_HANDLE
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palGetThreadPriority
 */
PAL_API PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority);

/**
 * @brief Sets the affinity of a thread.
 *
 * The @ref PAL_THREAD_FEATURE_AFFINITY feature must be supported.
 * `mask` is the set of logical processors (hardware threads) the thread is permitted to run on.
 * The first logical processor is `(1ULL << 0)`.
 *
 * @param[in] thread Thread to set the affinity of.
 * @param[in] mask Logical processor bitmask. Bit `n` set means the thread may run on logical processor `n`.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `thread` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_HANDLE
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palGetThreadAffinity
 */
PAL_API PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    uint64_t mask);

/**
 * @brief Sets the name of a thread.
 *
 * The @ref PAL_THREAD_FEATURE_NAME feature must be supported.
 * The thread name is visible in debugger.
 *
 * @param[in] thread Thread to set the name of.
 * @param[in] name New thread name (NULL-terminated UTF-8 string).
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @thread-safety `thread` must be externally synchronized.
 *
 * @result-code @ref PAL_RESULT_CODE_INVALID_HANDLE
 * @result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @note On Linux, thread names are limited to 16 characters including the NULL terminator.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palGetThreadName
 */
PAL_API PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name);

#endif // PAL_THREAD_THREAD_H