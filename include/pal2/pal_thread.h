/**
 * @brief This is the header file for PAL Thread Module API.
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
 * @{
 */
#define PAL_THREAD_FEATURE_STACK_SIZE (1U << 0) /**< set thread stack size */
#define PAL_THREAD_FEATURE_PRIORITY (1U << 1) /**< set and get thread priority */
#define PAL_THREAD_FEATURE_AFFINITY (1U << 2) /**< set and get thread affinity */
#define PAL_THREAD_FEATURE_NAME (1U << 3) /**< set and get thread name */
/** @} */

/**
 * @defgroup thread_priorities Thread Priorities
 * @{
 */
#define PAL_THREAD_PRIORITY_LOW 0
#define PAL_THREAD_PRIORITY_NORMAL 1
#define PAL_THREAD_PRIORITY_HIGH 2
#define PAL_THREAD_PRIORITY_COUNT 3 /**< number of thread priorities */
/** @} */

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
 * @typedef PalThreadFn
 * @brief Thread entry function.
 * 
 * This is an application-defined function where a thread starts executing from.
 * 
 * The function signature should look like this:
 * @code
 * void* PAL_CALL threadEntry(void* arg);
 * @endcode
 *
 * @param[in] arg User-defined data passed to the function or `nullptr`.
 * @return Return value of the thread as a pointer.
 *
 * @since Added in version 2.0
 */
typedef void* (PAL_CALL* PalThreadFn)(void* arg);

/**
 * @typedef PaTlsDestructorFn
 * @brief Thread local storage destruction function.
 * 
 * The function will be called when the TLS is freed and the value associated
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
 * This struct is used only during @ref palCreateThread() and may be
 * discarded after the function returns.
 * 
 * Setting `stackSize` to a value other than 0 requires @ref PAL_THREAD_FEATURE_STACK_SIZE
 * to be supported.
 *
 * Uninitialized fields may result in undefined behavior.
 * 
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef struct PalThreadCreateInfo {
    uint64_t stackSize; /**< thread stack size or `0` */
    const PalAllocator* allocator; /**< allocator to use or `nullptr` for default */
    PalThreadFn entry; /**< thread entry function */
    void* arg; /**< user-defined data passed to the function or `nullptr` */
} PalThreadCreateInfo;

/**
 * @brief Creates a new thread.
 * 
 * The created thread must be destroyed if not joined using @ref palDetachThread().
 * 
 * This function creates a thread using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy the its contents.
 * 
 * `info->allocator` is not copied. The allocator and any state referenced
 * by it must remain valid until the thread is done executing or its detached.
 * 
 * Created threads starts execution from `info->entry` function. The thread will
 * not be created if the entry function is `nullptr`. The created thread executes
 * until its detached or has finished. The thread is created with a priority of
 * `PAL_THREAD_PRIORITY_NORMAL`. Use @ref palSetThreadPriority() to change
 * the priority of the created thread.
 * 
 * Threads that are joined will automatically be detached after execution.
 * Free threads must be detached with @ref palDetachThread() when execution
 * has finished.
 *
 * @param[in] info Thread creation parameters.
 * @param[out] thread Output handle to recieve the created thread.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `thread` must be per thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
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
 * This function blocks the calling thread and waits for the specified thread
 * to finish executing before continuuing execution. After the provided thread
 * is done executing, it is freed and must not be used or detached.
 *
 * @param[in] thread Thread to join.
 * @param[out] retval Output to recieve the return value of the thread or `nullptr`.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `retval` must be per thread and `thread` must be externally synchhronized.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
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
 * This function releases the specified thread's resources and destroy it.
 * This function must not be use with a thread that was joined with @ref palJoinThread().
 * Calling this function on a thread that is not done executing is undefined behavior.
 * After this call, the thread must not be used.
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
 * This function yields the remainder of the calling threads time sliced, allowing other
 * threads of equal priority to run. The next thread is selected by the platform.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
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
 * @ingroup pal_thread
 */
PAL_API PalThread* PAL_CALL palGetCurrentThread();

/**
 * @brief Gets the supported features of the thread system.
 * 
 * This function gets the supported features of the thread system. This function may
 * be used if any thread has not been created.
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
 * @return Thread priority on success or `0` on failure.
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
 * @return Thread affinity on success or `0` on failure.
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
 * of the thread name in bytes. If the size of the specified buffer is smaller
 * than the size of the thread name, the buffer will be truncated.
 *
 * @param[in] thread Thread to get the name from.
 * @param[in] bufferSize Size of the buffer in bytes.
 * @param[out] size Output to recieve the size of the thread name in bytes.
 * @param[out] buffer Output buffer to write the thread name to.
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
 * @param[in] thread Thread to set its priority.
 * @param[in] priority Thread priority.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `thread` must be externally synchronized.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
 *               @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
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
 * `mask` is the set of CPUs the thread is permitted
 * to run on. The first CPU is `(1ULL << 0)`.
 *
 * @param[in] thread Thread to set its affinity.
 * @param[in] mask CPU core bitmask.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `thread` must be externally synchronized.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
 *               @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
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
 * The thread name will be visible in debuggers and the Task Manager (Windows).
 *
 * @param[in] thread Thread to set its name.
 * @param[in] name UTF-8 null terminated string.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `thread` must be externally synchronized.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
 *               @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 *
 * @note On `Linux`: thread names are limited to 16 characters including
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
 * The TLS handle can be used by multiple threads to associate thread local
 * vaules. The destructor will be called if @ref palDestroyTLS() is called and
 * the TLS has a `non-nullptr` value associated to it.
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
 * @param[in] Tls TLS to get its value.
 * @return Pointer to the value on success or `nullptr` on failure.
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
 * @param[in] Tls TLS to set its value.
 * @param[in] data Pointer to the value to set.
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
 * @param[out] mutex Output handle to recieve the created mutex.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `mutex` must be per thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY
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
 * Blocks if the mutex is already locked by another thread.
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
 * @param[out] condVar Output handle to recieve the created condition variable.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety `condVar` must be per thread.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 *               @ref PAL_RESULT_CODE_OUT_OF_MEMORY
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
 * Destroying a condition variable when threads are waiting on it results
 * in undefined behavior. The threads may be in waiting or blocking state
 * with no way to signal them.
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
 * @brief Unlocks the mutex and wait on the condition variable.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur.
 * This function waits until the condition variable is signaled.
 *
 * Example Code:
 * @code
 * while (!ready) { palWaitCondVar(condition, mutex); }
 * @endcode
 *
 * @param[in] condVar Condition variable to wait on.
 * @param[in] mutex Mutex to unlock.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety Thread safe.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
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
 * @brief Unlocks the mutex and wait on the condition variable for
 * the specified duration.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur.
 * This function waits for the specified `milliseconds`, the condition variable
 * will not be signaled if there is a timeout.
 * 
 * Example Code:
 * @code
 * while (!ready) { palWaitCondVar(condition, mutex); }
 * @endcode
 *
 * @param[in] condVar Condition variable to wait on.
 * @param[in] mutex Mutex to unlock.
 * @param[in] milliseconds Timeout in milliseconds.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure. 
 *         Call @ref palFormatResult for more information.
 *
 * @Thread-safety Thread safe.
 * 
 * @Result-codes Possible result codes include @ref PAL_RESULT_CODE_INVALID_HANDLE
 *               @ref PAL_RESULT_CODE_TIMEOUT
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
 * If multiple threads are waiting on the condition variable, the
 * thread that is awakened is implementation-defined.
 *
 * @param[in] condVar Condition variable.
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
 * @param[in] condVar Condition variable.
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
