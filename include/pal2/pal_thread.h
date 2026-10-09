/**
 * @file pal_thread.h
 * @brief Header file for the PAL Thread Module API.
 * 
 * Defines all the types, constants and functions of the thread module.
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
 * @defgroup pal_thread Thread Module
 * @{
 */

#ifndef PAL_THREAD_H
#define PAL_THREAD_H

#include "pal_core.h"

/**
 * @defgroup thread_features Thread Features
 * @{
 */
#define PAL_THREAD_FEATURE_STACK_SIZE (1U << 0) /**< Set thread stack size. */
#define PAL_THREAD_FEATURE_PRIORITY (1U << 1) /**< Get and set thread priority. */
#define PAL_THREAD_FEATURE_AFFINITY (1U << 2) /**< Get and set thread affinity. */
#define PAL_THREAD_FEATURE_NAME (1U << 3) /**< Get and set thread name. */
/** @} */

/**
 * @defgroup thread_priorities Thread Priorities
 * @{
 */
#define PAL_THREAD_PRIORITY_LOW 0 /**< Lower than normal priority. */
#define PAL_THREAD_PRIORITY_NORMAL 1 /**< Default priority of newly created threads. */
#define PAL_THREAD_PRIORITY_HIGH 2 /**< Higher than normal priority. */
#define PAL_THREAD_PRIORITY_COUNT 3 /**< Number of thread priorities. */
/** @} */

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
 */
typedef uint32_t PalThreadFeatures;

/**
 * @typedef PalThreadPriority
 * @brief Thread priority.
 * 
 * All values of this type follow the format `PAL_THREAD_PRIORITY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalThreadPriority;

/**
 * @struct PalThread
 * @brief Opaque handle to a thread.
 *
 * @since Added in version 2.0
 */
typedef struct PalThread PalThread;

/**
 * @typedef PalTLSId
 * @brief Opaque handle to a thread-local storage (TLS) slot.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalTLSId;

/**
 * @struct PalMutex
 * @brief Opaque handle to a mutex.
 *
 * @since Added in version 2.0
 */
typedef struct PalMutex PalMutex;

/**
 * @struct PalCondVar
 * @brief Opaque handle to a condition variable.
 *
 * @since Added in version 2.0
 */
typedef struct PalCondVar PalCondVar;

/**
 * @typedef PalThreadFn
 * @brief Thread entry function.
 * 
 * This is the application-defined function a thread starts executing from.
 * 
 * The function signature should look like this:
 * @code
 * void* PAL_CALL threadEntry(void* arg);
 * @endcode
 *
 * @param[in] arg User-defined data passed to the function or `nullptr`.
 * @return The return value of the thread. It can be retrieved with @ref palJoinThread().
 *
 * @since Added in version 2.0
 */
typedef void* (PAL_CALL* PalThreadFn)(void* arg);

/**
 * @typedef PaTlsDestructorFn
 * @brief Thread-local storage destructor function.
 * 
 * The function is called when the TLS is destroyed and the value associated
 * with it is not `nullptr`.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL tlsDestructor(void* userData);
 * @endcode
 *
 * @param[in] userData User-defined data passed to the function or `nullptr`.
 *
 * @since Added in version 2.0
 */
typedef void (PAL_CALL* PaTlsDestructorFn)(void* userData);
/** @} */

/**
 * @struct PalThreadCreateInfo
 * @brief Thread creation parameters.
 * 
 * This struct is used only by @ref palCreateThread() and may be discarded
 * after the function returns.
 * 
 * Setting `stackSize` to a value other than `0` requires
 * @ref PAL_THREAD_FEATURE_STACK_SIZE to be supported.
 *
 * `entry` must not be `nullptr`. All fields must be initialized; uninitialized
 * fields result in undefined behavior.
 * 
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef struct PalThreadCreateInfo {
    uint64_t stackSize; /**< Thread stack size in bytes or `0` for the platform default. */
    const PalAllocator* allocator; /**< Allocator to use or `nullptr` for default. */
    PalThreadFn entry; /**< Thread entry function. */
    void* arg; /**< User-defined data passed to `entry` or `nullptr`. */
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
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `thread` must be per thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT,
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY and
 *               @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
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
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety `retval` must be per thread and `thread` must be externally synchronized.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
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
 * @Thread-safety `thread` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
PAL_API void PAL_CALL palDetachThread(PalThread* thread);

/**
 * @brief Suspends the calling thread.
 * 
 * This function suspends the calling thread for the duration of `milliseconds`.
 *
 * @param[in] milliseconds Number of milliseconds to sleep.
 *
 * @Thread-safety Thread safe.
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
 * @Thread-safety Thread safe.
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
 * @Thread-safety Thread safe.
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
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
PAL_API PalThreadFeatures PAL_CALL palGetThreadFeatures();

/**
 * @brief Gets the priority of a thread.
 *
 * @ref PAL_THREAD_FEATURE_PRIORITY feature must be supported.
 *
 * @param[in] thread Thread to get the priority from.
 * @return The thread priority on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
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
 * @ref PAL_THREAD_FEATURE_AFFINITY feature must be supported.
 *
 * @param[in] thread Thread to get the affinity from.
 * @return The thread affinity bitmask on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
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
 * @ref PAL_THREAD_FEATURE_NAME feature must be supported or this function
 * results in undefined behavior. Set `buffer` to `nullptr` to get the size
 * of the thread name in bytes. If `bufferSize` is smaller than the size of the
 * thread name, the name is truncated to fit the buffer.
 *
 * @param[in] thread Thread to get the name from.
 * @param[in] bufferSize Size of `buffer` in bytes.
 * @param[out] size Output to receive the size of the thread name in bytes,
 *                  or `nullptr` if not needed.
 * @param[out] buffer Output buffer to write the thread name to, or `nullptr`
 *                    to only query the size.
 *
 * @Thread-safety `buffer` must be per thread.
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
 * @ref PAL_THREAD_FEATURE_PRIORITY feature must be supported.
 * 
 * @param[in] thread Thread to set the priority of.
 * @param[in] priority New thread priority (`PAL_THREAD_PRIORITY_*`).
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety `thread` must be externally synchronized.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
 *               and @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
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
 * @ref PAL_THREAD_FEATURE_AFFINITY feature must be supported.
 * `mask` is the set of CPUs the thread is permitted to run on. The first CPU
 * is `(1ULL << 0)`.
 *
 * @param[in] thread Thread to set the affinity of.
 * @param[in] mask CPU core bitmask. Bit `n` set means the thread may run on CPU `n`.
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety `thread` must be externally synchronized.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
 *               and @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
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
 * @ref PAL_THREAD_FEATURE_NAME feature must be supported.
 * The thread name is visible in debuggers and, on Windows, in the Task Manager.
 *
 * @param[in] thread Thread to set the name of.
 * @param[in] name Null-terminated UTF-8 string.
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety `thread` must be externally synchronized.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
 *               and @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
 *
 * @note On Linux, thread names are limited to 16 characters including
 *       the null terminator.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palGetThreadName
 */
PAL_API PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name);

/**
 * @brief Creates a TLS.
 * 
 * The created TLS must be destroyed using @ref palDestroyTLS().
 *
 * The TLS handle can be shared by multiple threads, each of which has its own
 * value associated with it. The destructor is called when @ref palDestroyTLS()
 * is called and the TLS has a non-`nullptr` value associated with it.
 *
 * @param[in] destructor TLS destructor function or `nullptr`.
 * @return The created TLS on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palDestroyTLS
 */
PAL_API PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor);

/**
 * @brief Destroys the TLS.
 *
 * @param[in] Tls TLS to destroy.
 *
 * @Thread-safety `Tls` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palCreateTLS
 */
PAL_API void PAL_CALL palDestroyTLS(PalTLSId Tls);

/**
 * @brief Gets the value associated with the TLS on the calling thread.
 *
 * @param[in] Tls TLS to get the value of.
 * @return Pointer to the value on success or `nullptr` if no value was set or
 *         on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palSetTLS
 */
PAL_API void* PAL_CALL palGetTLS(PalTLSId Tls);

/**
 * @brief Sets the TLS value on the calling thread.
 *
 * @param[in] Tls TLS to set the value of.
 * @param[in] data Pointer to the value to set or `nullptr`.
 *
 * @Thread-safety `Tls` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palGetTLS
 */
PAL_API void PAL_CALL palSetTLS(
    PalTLSId Tls,
    void* data);

/**
 * @brief Creates a mutex.
 * 
 * The created mutex must be destroyed using @ref palDestroyMutex().
 *
 * @param[in] allocator Allocator to use or `nullptr` for default.
 * @param[out] mutex Output handle to receive the created mutex.
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety `mutex` must be per thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               and @ref PAL_RESULT_CODE_OUT_OF_MEMORY.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palDestroyMutex
 */
PAL_API PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** mutex);

/**
 * @brief Destroys a mutex.
 * 
 * The mutex must be unlocked before destroying if it was locked.
 *
 * @param[in] mutex Mutex to destroy.
 *
 * @Thread-safety `mutex` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palCreateMutex
 */
PAL_API void PAL_CALL palDestroyMutex(PalMutex* mutex);

/**
 * @brief Locks a mutex.
 *
 * Blocks until the mutex is available if it is already locked by another thread.
 *
 * @param[in] mutex Mutex to lock.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palUnlockMutex
 */
PAL_API void PAL_CALL palLockMutex(PalMutex* mutex);

/**
 * @brief Unlocks a mutex.
 *
 * The function must be called by the thread that first locked the mutex.
 *
 * @param[in] mutex Mutex to unlock.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palLockMutex
 */
PAL_API void PAL_CALL palUnlockMutex(PalMutex* mutex);

/**
 * @brief Creates a condition variable.
 * 
 * The created condition variable must be destroyed using @ref palDestroyCondVar().
 *
 * @param[in] allocator Allocator to use or `nullptr` for default.
 * @param[out] condVar Output handle to receive the created condition variable.
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety `condVar` must be per thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               and @ref PAL_RESULT_CODE_OUT_OF_MEMORY.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palDestroyCondVar
 */
PAL_API PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar);

/**
 * @brief Destroys a condition variable.
 * 
 * Destroying a condition variable while threads are waiting on it is
 * undefined behavior, since those threads may be blocked with no way to
 * signal them.
 *
 * @param[in] condVar Condition variable to destroy.
 *
 * @Thread-safety `condVar` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palCreateCondVar
 */
PAL_API void PAL_CALL palDestroyCondVar(PalCondVar* condVar);

/**
 * @brief Unlocks the mutex and waits on the condition variable.
 *
 * The mutex must be locked by the calling thread before this call. It is
 * unlocked while waiting and locked again before the function returns.
 * Spurious wakeups may occur, so always wait inside a loop that checks the
 * condition. This function waits until the condition variable is signaled.
 *
 * Example Code:
 * @code
 * while (!ready) { palWaitCondVar(condition, mutex); }
 * @endcode
 *
 * @param[in] condVar Condition variable to wait on.
 * @param[in] mutex Locked mutex to unlock while waiting.
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Thread safe.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palWaitCondVarTimeout
 */
PAL_API PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex);

/**
 * @brief Unlocks the mutex and waits on the condition variable for
 * the specified duration.
 *
 * The mutex must be locked by the calling thread before this call. It is
 * unlocked while waiting and locked again before the function returns.
 * Spurious wakeups may occur, so always wait inside a loop that checks the
 * condition. This function waits for at most `milliseconds` and fails with
 * @ref PAL_RESULT_CODE_TIMEOUT if the condition variable was not signaled in
 * that time.
 * 
 * Example Code:
 * @code
 * while (!ready) {
 *     if (palWaitCondVarTimeout(condition, mutex, 100) != PAL_RESULT_SUCCESS) {
 *         break; // timed out or failed
 *     }
 * }
 * @endcode
 *
 * @param[in] condVar Condition variable to wait on.
 * @param[in] mutex Locked mutex to unlock while waiting.
 * @param[in] milliseconds Timeout in milliseconds.
 * @return @ref PAL_RESULT_SUCCESS on success or a result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Thread safe.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
 *               and @ref PAL_RESULT_CODE_TIMEOUT.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palWaitCondVar
 */
PAL_API PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds);

/**
 * @brief Wakes a thread waiting on the condition variable.
 * 
 * If multiple threads are waiting on the condition variable, which thread is
 * awakened is implementation-defined. Does nothing if no thread is waiting.
 *
 * @param[in] condVar Condition variable to signal.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palBroadcastCondVar
 */
PAL_API void PAL_CALL palSignalCondVar(PalCondVar* condVar);

/**
 * @brief Wakes all threads waiting on the condition variable.
 *
 * @param[in] condVar Condition variable to broadcast to.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 * 
 * @sa palSignalCondVar
 */
PAL_API void PAL_CALL palBroadcastCondVar(PalCondVar* condVar);

#endif // PAL_THREAD_H
