/**
 * @file pal_thread.h
 * @brief This is the header file for PAL Thread API.
 *
 * It defines all the types and functions of the thread module.
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
 * @brief Thread Features.
 * 
 * @{
 */
#define PAL_THREAD_FEATURE_STACK_SIZE (1U << 0)
#define PAL_THREAD_FEATURE_PRIORITY (1U << 1)
#define PAL_THREAD_FEATURE_AFFINITY (1U << 2)
#define PAL_THREAD_FEATURE_NAME (1U << 3)
/** @} */

/**
 * @defgroup thread_priorities Thread Priorities
 * @brief Thread Priorities.
 * 
 * @{
 */
#define PAL_THREAD_PRIORITY_LOW 0
#define PAL_THREAD_PRIORITY_NORMAL 1
#define PAL_THREAD_PRIORITY_HIGH 2
#define PAL_THREAD_PRIORITY_COUNT 3
/** @} */

/**
 * @struct PalThread
 * @brief Opaque handle to a thread.
 *
 * @since Added in version 2.0
 */
typedef struct PalThread PalThread;

/**
 * @typedef PalTLSId
 * @brief Opaque handle to a Thread Local Storage.
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
 * @typedef PalThreadFeatures
 * @brief Thread system features.
 * 
 * This is a bitmask of all supported features of the thread system.
 * 
 * All values of this type follow the format `PAL_THREAD_FEATURE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalThreadFeatures;

/**
 * @typedef PalThreadPriority
 * @brief Thread priority types.
 * 
 * All values of this type follow the format `PAL_THREAD_PRIORITY_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalThreadPriority;

/**
 * @typedef PalThreadFn
 * @brief Function pointer type used for thread entry function.
 * 
 * The function signature should look like this:
 * @code
 * void* PAL_CALL threadEntry(void* arg);
 * @endcode
 *
 * @param[in] arg User data passed from `PalThreadCreateInfo::arg`.
 * Can be `nullptr`.
 *
 * @return The return value of the thread as a pointer.
 *
 * @since Added in version 2.0
 */
typedef void* (PAL_CALL* PalThreadFn)(void* arg);

/**
 * @typedef PaTlsDestructorFn
 * @brief Function pointer type used for TLS.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL tlsDestructor(void* userData);
 * @endcode
 *
 * This is called when the TLS is destroyed and its value is not `nullptr`.
 *
 * @param[in] userData User data passed from `palCreateTLS()`.
 * Can be `nullptr`.
 *
 * @since Added in version 2.0
 */
typedef void (PAL_CALL* PaTlsDestructorFn)(void* userData);

/**
 * @struct PalThreadCreateInfo
 * @brief Contains creation parameters of a thread.
 *
 * Uninitialized fields may result in undefined behavior.
 * 
 * @since Added in version 2.0
 * 
 * @var PalThreadCreateInfo::stackSize
 * The thread stack size. Explicitly setting the stack size of a thread
 * needs `PAL_THREAD_FEATURE_STACK_SIZE` to be supported. @nl
 * Set to `0` for default.
 * 
 * @var PalThreadCreateInfo::allocator
 * The allocator the thread should use. Set to `nullptr` to use the
 * thread-safe default.
 * 
 * @var PalThreadCreateInfo::entry
 * The thread entry function. Must not be `nullptr`.
 * 
 * @var PalThreadCreateInfo::arg
 * User data passed to entry function. Can be `nullptr`.
 */
typedef struct PalThreadCreateInfo
{
    uint64_t stackSize;
    const PalAllocator* allocator;
    PalThreadFn entry;
    void* arg;
} PalThreadCreateInfo;

/**
 * @brief Creates a new thread.
 * 
 * `PalThreadCreateInfo::allocator` will not be copied, therefore the pointer
 * must remain valid until the thread is detached or has finished executing.
 * 
 * Detach the thread with `palDetachThread()` when no longer needed.
 * Threads that were joined will automatically been detached after execution.
 * 
 * The created thread starts executing from the entry function. The thread runs
 * until the entry function has finished executing or its detached.
 *
 * @param[in] info Information about how to create the thread.
 * @param[out] thread The output handle to recieve the created thread.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `thread` must be per thread and 
 * `PalEventDriverCreateInfo::allocator` must be thread-safe.
 *
 * @since Added in version 2.0
 * @sa palDetachThread
 */
PAL_API PalResult PAL_CALL palCreateThread(
    const PalThreadCreateInfo* info,
    PalThread** thread);

/**
 * @brief Waits for the thread to finish executing.
 *
 * After the thread is done executing, it is freed automatically and
 * must not be used anymore or detached.
 *
 * @param[in] thread The thread.
 * @param[out] retval The output to recieve the return value of the thread. 
 * Can be `nullptr` to discard it.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `retval` must be per thread.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palJoinThread(
    PalThread* thread,
    void** retval);

/**
 * @brief Releases the threads resources and destroy it.
 *
 * Must be called when the thread is done executing.
 * After this call, the thread cannot be attached or used anymore.
 *
 * This must not be called on a thread that has been attached.
 *
 * @param[in] thread The thread.
 *
 * @Thread-safety `thread` must be externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palDetachThread(PalThread* thread);

/**
 * @brief Suspends the calling thread for the duration of milliseconds.
 *
 * @param[in] milliseconds Number of milliseconds to sleep.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSleep(uint64_t milliseconds);

/**
 * @brief Yields the remainder of the calling threads time sliced,
 * allowing other threads of equal priority to run.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palYield();

/**
 * @brief Gets the current executing thread.
 *
 * @return The current thread on success or `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API PalThread* PAL_CALL palGetCurrentThread();

/**
 * @brief Gets the supported features of thread system.
 *
 * This is based on the platform (OS) features not a single created thread.
 *
 * @return The thread features on success or 0 on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API PalThreadFeatures PAL_CALL palGetThreadFeatures();

/**
 * @brief Gets the priority of the thread.
 *
 * `PAL_THREAD_FEATURE_PRIORITY` must be supported otherwise 
 * undefined behavior.
 *
 * @param[in] thread The thread.
 *
 * @return The thread priority on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread);

/**
 * @brief Gets the affinity of the provided thread.
 *
 * `PAL_THREAD_FEATURE_AFFINITY` must be supported otherwise 
 * undefined behavior.
 *
 * @param[in] thread The thread.
 *
 * @return The thread affinity on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API uint64_t PAL_CALL palGetThreadAffinity(PalThread* thread);

/**
 * @brief Gets the name of the provided thread.
 *
 * `PAL_THREAD_FEATURE_NAME` must be supported otherwise undefined behavior.
 * Set the buffer to `nullptr` to get the size of the thread name in bytes.
 *
 * If the size of `buffer` is less than the actual size of the thread
 * name, PAL will write upto that limit.
 *
 * @param[in] thread The thread.
 * @param[in] bufferSize Size of `buffer` in bytes.
 * @param[out] size The output to recieve the actual size of the thread 
 * name in bytes.
 * @param[out] buffer The output buffer to write the thread name to.
 *
 * @Thread-safety `buffer` must be per thread.
 *
 * @since Added in version 2.0
 * @sa palSetThreadName
 */
PAL_API void PAL_CALL palGetThreadName(
    PalThread* thread,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer);

/**
 * @brief Sets the priority of the thread.
 *
 * `PAL_THREAD_FEATURE_PRIORITY` must be supported otherwise
 * undefined behavior.
 *
 * @param[in] thread The thread.
 * @param[in] priority The new thread priority.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `thread` must be externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palSetThreadPriority(
    PalThread* thread,
    PalThreadPriority priority);

/**
 * @brief Sets the affinity of the thread.
 *
 * `PAL_THREAD_FEATURE_AFFINITY` must be supported otherwise
 * undefined behavior.
 *
 * To be safe, get the number of CPU cores and use that to build the CPU mask.
 * Example: we set a thread to the first and second CPU core.
 *
 * @code
 * uint64_t cpuMask = (1ULL << 0) | (1ULL << 1).
 * @endcode
 *
 * @param[in] thread The thread.
 * @param[in] mask The CPU core mask.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `thread` must be externally synchronized.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palSetThreadAffinity(
    PalThread* thread,
    uint64_t mask);

/**
 * @brief Sets the name of the thread.
 *
 * `PAL_THREAD_FEATURE_NAME` must be supported otherwise undefined behavior.
 * The thread name will be visible in debuggers and the Task Manager (Windows).
 *
 * @param[in] thread The thread.
 * @param[in] name UTF-8 null terminated string.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `thread` must be externally synchronized.
 *
 * @note On `Linux`: Thread names are limited to 16 characters including the null
 * terminator.
 *
 * @since Added in version 2.0
 * @sa palGetThreadName
 */
PAL_API PalResult PAL_CALL palSetThreadName(
    PalThread* thread,
    const char* name);

/**
 * @brief Creates a new TLS.
 *
 * The TLS handle can be used by multiple threads to associate thread local
 * vaules. The destructor will be called if `palDestroyTLS()` is called and
 * the TLS has a valid value.
 *
 * @param[in] destructor The TLS destructor callback. Can be `nullptr`.
 *
 * @return The created TLS id on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palDestroyTLS
 */
PAL_API PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor);

/**
 * @brief Destroys the provided TLS.
 *
 * @param[in] Tls The TLS.
 *
 * @Thread-safety `Tls` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateTLS
 */
PAL_API void PAL_CALL palDestroyTLS(PalTLSId Tls);

/**
 * @brief Gets the value associated with the TLS on the calling thread.
 *
 * @param[in] Tls The TLS.
 *
 * @return the value on success or `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palSetTLS
 */
PAL_API void* PAL_CALL palGetTLS(PalTLSId Tls);

/**
 * @brief Sets a value associated with the TLS on the calling thread.
 *
 * @param[in] Tls The TLS.
 * @param[in] data The value to set for the calling thread.
 *
 * @Thread-safety `Tls` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palGetTLS
 */
PAL_API void PAL_CALL palSetTLS(
    PalTLSId Tls,
    void* data);

/**
 * @brief Creates a mutex.
 *
 * @param[in] allocator The allocator the thread should use. Set to `nullptr`
 * to use the thread-safe default.
 * @param[out] mutex The output handle to recieve the created mutex.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `mutex` must be per thread and `allocator` must be 
 * thread safe.
 *
 * @since Added in version 2.0
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
 * @param[in] mutex The mutex.
 *
 * @Thread-safety `mutex` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateMutex
 */
PAL_API void PAL_CALL palDestroyMutex(PalMutex* mutex);

/**
 * @brief Locks a mutex. 
 * 
 * Blocks if the mutex is already locked by another thread.
 *
 * @param[in] mutex The mutex.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palUnlockMutex
 */
PAL_API void PAL_CALL palLockMutex(PalMutex* mutex);

/**
 * @brief Unlocks a mutex.
 *
 * The function must be called by the thread that first locked the mutex.
 *
 * @param[in] mutex The mutex.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palLockMutex
 */
PAL_API void PAL_CALL palUnlockMutex(PalMutex* mutex);

/**
 * @brief Creates a condition variable.
 *
 * @param[in] allocator The allocator the thread should use. Set to `nullptr`
 * to use the thread-safe default.
 * @param[out] condVar The output handle to recieve the created
 * condition variable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety `condVar` must be per thread and `allocator` must be 
 * thread safe.
 *
 * @since Added in version 2.0
 * @sa palDestroyCondVar
 */
PAL_API PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** condVar);

/**
 * @brief Destroys a condition variable.
 *
 * Threads must not wait on the condition variable or undefined
 * behaviour.
 *
 * @param[in] condVar The condition variable.
 *
 * @Thread-safety `condVar` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateCondVar
 */
PAL_API void PAL_CALL palDestroyCondVar(PalCondVar* condVar);

/**
 * @brief Unlocks the mutex and wait on the condition variable.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur, its
 * best to use a loop. This waits until the condition varibale is signaled.
 *
 * Example:
 *
 * @code
 * while (!ready) { palWaitCondVar(condition, mutex); }
 * @endcode
 *
 * @param[in] condVar The condition variable.
 * @param[in] mutex The mutex.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palWaitCondVarTimeout
 */
PAL_API PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex);

/**
 * @brief Unlocks the mutex and wait on the condition variable for
 * the specified duration.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur, its
 * best to use a loop. If the condition variable is not signaled but the time to
 * wait is up `PAL_RESULT_TIMEOUT` is returned.
 *
 * @param[in] condVar The condition variable.
 * @param[in] mutex The mutex.
 * @param[in] milliseconds Timeout in milliseconds.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palWaitCondVar
 */
PAL_API PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds);

/**
 * @brief Wakes a thread waiting on the condition variable.
 *
 * @param[in] condVar The condition variable.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palBroadcastCondVar
 */
PAL_API void PAL_CALL palSignalCondVar(PalCondVar* condVar);

/**
 * @brief Wakes all threads waiting on the condition variable.
 *
 * @param[in] condVar The condition variable.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palSignalCondVar
 */
PAL_API void PAL_CALL palBroadcastCondVar(PalCondVar* condVar);

/** @} */

#endif // PAL_THREAD_H
