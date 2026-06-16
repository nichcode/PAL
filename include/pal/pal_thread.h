
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup pal_thread Thread
 * @ingroup pal_thread
 * @{
 */

#ifndef _PAL_THREAD_H
#define _PAL_THREAD_H

#include "pal_core.h"

#define PAL_THREAD_FEATURE_STACK_SIZE (1ULL << 0)
#define PAL_THREAD_FEATURE_PRIORITY (1ULL << 1)
#define PAL_THREAD_FEATURE_AFFINITY (1ULL << 2)
#define PAL_THREAD_FEATURE_NAME (1ULL << 3)

#define PAL_THREAD_PRIORITY_LOW 0
#define PAL_THREAD_PRIORITY_NORMAL 1
#define PAL_THREAD_PRIORITY_HIGH 2

/**
 * @struct PalThread
 * @brief Opaque handle to a thread.
 *
 * @since 1.0
 */
typedef struct PalThread PalThread;

/**
 * @typedef PalTLSId
 * @brief Opaque handle to a Thread Local Storage.
 *
 * @since 1.0
 */
typedef uint32_t PalTLSId;

/**
 * @struct PalMutex
 * @brief Opaque handle to a mutex.
 *
 * @since 1.0
 */
typedef struct PalMutex PalMutex;

/**
 * @struct PalCondVar
 * @brief Opaque handle to a condition variable.
 *
 * @since 1.0
 */
typedef struct PalCondVar PalCondVar;

/**
 * @typedef PalThreadFeatures
 * @brief Thread system features.
 *
 * All thread features follow the format `PAL_THREAD_FEATURE_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint64_t PalThreadFeatures;

/**
 * @typedef PalThreadPriority
 * @brief Thread priority types. This is not a bitmask.
 *
 * All thread priority types follow the format `PAL_THREAD_PRIORITY_**` for
 * consistency and API use.
 *
 * @since 2.0
 */
typedef uint32_t PalThreadPriority;

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
 * @typedef PaTlsDestructorFn
 * @brief Function pointer type used for TLS.
 *
 * This is called when the TLS is destroyed and its value is not nullptr.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 *
 * @since 1.0
 */
typedef void (*PaTlsDestructorFn)(void* userData);

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
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if `retval` is per thread.
 *
 * @since 2.0
 */
PAL_API PalResult PAL_CALL palJoinThread(
    PalThread* thread,
    void** retval);

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

/**
 * @brief Create a new TLS.
 *
 * The TLS handle can be used by multiple threads to associate thread local
 * vaules. The destructor will be called if palDestroyTLS() is called and
 * the TLS has a valid value.
 *
 * @param[in] destructor Pointer to the TLS destructor function. Can be
 * nullptr.
 *
 * @return The TLS on success or 0 on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palDestroyTLS
 */
PAL_API PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor);

/**
 * @brief Destroy the provided TLS.
 *
 * @param[in] id The TLS to destroy.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palCreateTL
 */
PAL_API void PAL_CALL palDestroyTLS(PalTLSId id);

/**
 * @brief Get the value associated with the provided TLS on the calling thread.
 *
 * @param[in] id The TLS to query value.
 *
 * @return the value on success or nullptr on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palSetTLS
 */
PAL_API void* PAL_CALL palGetTLS(PalTLSId id);

/**
 * @brief Set a value associated with the provided TLS on the calling thread.
 *
 * @param[in] id The TLS to set value to.
 * @param[in] data The value to set for the calling thread
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetTLS
 */
PAL_API void PAL_CALL palSetTLS(
    PalTLSId id,
    void* data);

/**
 * @brief Create a mutex.
 *
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use
 * default.
 * @param[out] outMutex Pointer to a PalMutex to recieve the created mutex.
 * Must not be nullptr.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if the provided allocator is
 * thread safe and `outMutex` is per thread.
 *
 * @since 1.0
 * @sa palDestroyMutex
 */
PAL_API PalResult PAL_CALL palCreateMutex(
    const PalAllocator* allocator,
    PalMutex** outMutex);

/**
 * @brief Destroy a mutex
 *
 * If `mutex` is invalid, this function returns silently.
 * The mutex must be unlocked before destroying if it was locked.
 *
 * @param[in] mutex Pointer to the mutex.
 *
 * Thread safety: Thread safe if the allocator used to create
 * the mutex is thread safe and `mutex` is per thread.
 *
 * @since 1.0
 * @sa palCreateMutex
 */
PAL_API void PAL_CALL palDestroyMutex(PalMutex* mutex);

/**
 * @brief Lock a mutex. Blocks if the mutex is already locked by another thread.
 *
 * @param[in] mutex Pointer to the mutex to lock.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palUnlockMutex
 */
PAL_API void PAL_CALL palLockMutex(PalMutex* mutex);

/**
 * @brief Unlock a mutex.
 *
 * The function must be called by the thread that first locked the mutex.
 *
 * @param[in] mutex Pointer to the mutex to unlock.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palLockMutex
 */
PAL_API void PAL_CALL palUnlockMutex(PalMutex* mutex);

/**
 * @brief Create a condition variable.
 *
 * @param[in] allocator Optional user-provided allocator. Set to nullptr to use
 * default.
 * @param[out] outCondVar Pointer to a PalCondVar to recieve the created
 * condition variable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or a result code on
 * failure. Call palFormatResult() for more information.
 *
 * Thread safety: Thread safe if the provided allocator is
 * thread safe and `outCondVar` is per thread.
 *
 * @since 1.0
 * @sa palDestroyCondVar
 */
PAL_API PalResult PAL_CALL palCreateCondVar(
    const PalAllocator* allocator,
    PalCondVar** outCondVar);

/**
 * @brief Destroy a condition variable.
 *
 * If the condition variable is invalid, this function returns silently.
 * Threads must not wait on the condition variable or undefined
 * behaviour.
 *
 * @param[in] condVar Pointer to the condition to destroy
 *
 * Thread safety: Thread safe if the allocator used to create
 * the condition varibale is thread safe and `condVar` is per thread.
 *
 * @since 1.0
 * @sa palCreateCondVar
 */
PAL_API void PAL_CALL palDestroyCondVar(PalCondVar* condVar);

/**
 * @brief Unlock the provided mutex and wait on the condition variable.
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
 * @param[in] condVar Pointer to the condition variable.
 * @param[in] mutex Pointer to the mutex.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palWaitCondVarTimeout
 */
PAL_API PalResult PAL_CALL palWaitCondVar(
    PalCondVar* condVar,
    PalMutex* mutex);

/**
 * @brief Unlock the provided mutex and wait on the condition variable.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur, its
 * best to use a loop. If the condition variable is not signaled but the time to
 * wait is up `PAL_RESULT_TIMEOUT` is returned.
 *
 * @param[in] condVar Pointer to the condition variable.
 * @param[in] mutex Pointer to the mutex.
 * @param[in] milliseconds Timeout in milliseconds.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palWaitCondVar
 */
PAL_API PalResult PAL_CALL palWaitCondVarTimeout(
    PalCondVar* condVar,
    PalMutex* mutex,
    uint64_t milliseconds);

/**
 * @brief Wake a single thread waiting on the condition variable.
 *
 * @param[in] condVar Pointer to the condition variable.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palBroadcastCondVar
 */
PAL_API void PAL_CALL palSignalCondVar(PalCondVar* condVar);

/**
 * @brief Wake all threads waiting on the condition variable.
 *
 * @param[in] condVar Pointer to the condition variable.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palSignalCondVar
 */
PAL_API void PAL_CALL palBroadcastCondVar(PalCondVar* condVar);

/** @} */

#endif // _PAL_THREAD_H
