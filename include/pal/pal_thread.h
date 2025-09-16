
/**

Copyright (C) 2025 Nicholas Agbo

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

/**
 * @file pal_thread.h
 *
 * Header file for thread functions, macros, enum and structs
 *
 * @defgroup thread
 *
 */

#ifndef _PAL_THREAD_H
#define _PAL_THREAD_H

#include "pal_core.h"

/**
 * @typedef PalTLSId
 * @brief Opaque handle to a Thread Local Storage.
 *
 * @ingroup thread
 */
typedef Uint32 PalTLSId;

/**
 * @struct PalThread
 * @brief Opaque handle to a thread.
 *
 * @ingroup thread
 */
typedef struct PalThread PalThread;

/**
 * @struct PalMutex
 * @brief Opaque handle to a mutex.
 *
 * @ingroup thread
 */
typedef struct PalMutex PalMutex;

/**
 * @struct PalCondVar
 * @brief Opaque handle to a condition variable.
 *
 * @ingroup thread
 */
typedef struct PalCondVar PalCondVar;

/**
 * @typedef PalThreadFn
 * @brief Function pointer type used for threads.
 *
 * @param[in] arg Optional pointer to user data passed into the function. Can be
 * `nullptr`.
 *
 * @return A pointer
 *
 * @sa palCreateThread
 * @ingroup thread
 */
typedef void* (*PalThreadFn)(void* arg);

/**
 * @typedef PaTlsDestructorFn
 * @brief Function pointer type used for TLS.
 *
 * This is called when the TLS is destroyed and its value is not `nullptr`
 *
 * @param[in] userData Optional pointer to user data passed into the function.
 * Can be `nullptr`.
 *
 * @sa palCreateThread
 * @ingroup thread
 */
typedef void (*PaTlsDestructorFn)(void* userData);

typedef enum {
    PAL_THREAD_FEATURE_STACK_SIZE = PAL_BIT(0), /** < Setting thread stack size.*/
    PAL_THREAD_FEATURE_PRIORITY   = PAL_BIT(1), /** < Setting and getting thread priority.*/
    PAL_THREAD_FEATURE_AFFINITY   = PAL_BIT(2), /** < Setting and getting thread affinity.*/
    PAL_THREAD_FEATURE_NAME       = PAL_BIT(3)  /** < Setting and getting thread name.*/
} PalThreadFeatures;

typedef enum {
    PAL_THREAD_PRIORITY_LOW,
    PAL_THREAD_PRIORITY_NORMAL,
    PAL_THREAD_PRIORITY_HIGH
} PalThreadPriority;

typedef struct {
    Uint64      stackSize; /** Thread stack size. Set to `0` to use default*/
    PalThreadFn entry;     /** Thread entry function*/
    void*       arg;       /** Optional pointer to user data for thread function. Can be
                              `nullptr`*/
} PalThreadCreateInfo;

/**
 * @brief Get the allocator for all thread allocations.
 *
 * The allocator will be used for creating threads, mutexes, and condition
 * variables.
 *
 * @return The allocator
 *
 * @note This function is thread-safe.
 *
 * @sa PalAllocator
 * @ingroup thread
 */
PAL_API const PalAllocator* PAL_CALL palGetThreadAllocator();

/**
 * @brief Set the allocator for all thread allocations.
 *
 * The allocator will be used for creating threads, mutexes, and condition
 * variables.
 *
 * @note This function is not thread-safe.
 *
 * @sa PalAllocator
 * @ingroup thread
 */
PAL_API void PAL_CALL palSetThreadAllocator(const PalAllocator* allocator);

/**
 * @brief Create a new thread
 *
 * The thread begins executing the entry function provided by the user in
 * `info`. The thread runs until the entry function has finished executing or
 * its detached with `palDetachThread()`. The `info` pointer must be valid,
 * explicitly initialized by the user. see `PalThreadCreateInfo` struct.
 *
 * @param[in] info Pointer to the PalThreadCreateInfo struct with creation
 * specifications.
 * @param[out] outThread Pointer to recieve the created thread.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure.
 *
 * @note This function is not thread safe.
 *
 * @sa PalThreadCreateInfo, palJoinThread() palDetachThread()
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palCreateThread(const PalThreadCreateInfo* info, PalThread** outThread);

/**
 * @brief Wait for the provided thread to finish executing.
 *
 * @param[in] thread Pointer to the thread.
 * @param[out] retval Optionally pointer to get the thread's exit value.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure.
 *
 * @note This function is thread safe.
 * @note Blocks the calling thread unti the provided thread has finished or
 * destryoyed
 *
 * @sa palDetachThread()
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palJoinThread(PalThread* thread, void* retval);

/**
 * @brief Release the thread's resources and destroy it.
 *
 * This function must be called when the thread is done executing.
 * After this call, the thread cannot be attached or used anymore.
 * If `thread` is invalid, this function returns silently.
 *
 * @param[in] thread Pointer to the thread.
 *
 * @note This function is thread safe.
 *
 * @sa palJoinThread(),
 * @ingroup thread
 */
PAL_API void PAL_CALL palDetachThread(PalThread* thread);

/**
 * @brief Suspend the calling thread for the provided duration of milliseconds.
 *
 * @param[in] milliseconds Number of milliseconds to sleep.
 *
 * @note This function is thread safe.
 *
 * @ingroup thread
 */
PAL_API void PAL_CALL palSleep(Uint64 milliseconds);

/**
 * @brief Yield the remainder of the calling thread's time sliced,
 * allowing other threads of equal priority to run.
 *
 * @note This function is thread safe.
 *
 * @ingroup thread
 */
PAL_API void PAL_CALL palYield();

/**
 * @brief Get the current executing thread.
 *
 * @return The current thread on success or `nullptr` on failure.
 *
 * @note This function is thread safe.
 *
 * @ingroup thread
 */
PAL_API PalThread* PAL_CALL palGetCurrentThread();

/**
 * @brief Get the supported features of PAL threading system.
 *
 * This is based on the platform (OS) features not a single created thread.
 *
 * @return The thread features on success or `0` on failure.
 *
 * @note This function is thread safe.
 *
 * @ingroup thread
 */
PAL_API PalThreadFeatures PAL_CALL palGetThreadFeatures();

/**
 * @brief Get the priority of the provided thread.
 *
 * `PAL_THREAD_FEATURE_PRIORITY` must be supported otherwise, this function will
 * fail and `PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED` will be returned.
 *
 * @param[in] thread The thread
 *
 * @return The thread priority on success or `0` on failure.
 *
 * @note This function is not thread safe.
 *
 * @ingroup thread
 */
PAL_API PalThreadPriority PAL_CALL palGetThreadPriority(PalThread* thread);

/**
 * @brief Get the affinity of the provided thread.
 *
 * `PAL_THREAD_FEATURE_AFFINITY` must be supported otherwise, this function will
 * fail and `PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED` will be returned. Thread
 * affinity is the number of CPU cores the thread is allowed to be executed on.
 *
 * @param[in] thread The thread
 *
 * @return The thread affinity on success or `0` on failure.
 *
 * @note This function is thread safe.
 *
 * @ingroup thread
 */
PAL_API Uint64 PAL_CALL palGetThreadAffinity(PalThread* thread);

/**
 * @brief Get the name of the provided thread.
 *
 * This function returns an allocated UTF-8 encoded string which must be freed
 * when no longer needed with `palFree()` The allocator used to allocate the
 * string is the same passed in `palSetThreadAllocator()`.
 *
 * `PAL_THREAD_FEATURE_NAME` must be supported otherwise, this function will
 * fail and `PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED` will be returned.
 *
 * @param[in] thread The thread
 *
 * @return Pointer to buffer to recieve the thread name.
 *
 * @note This function is not thread safe.
 * @note The returned string should not be freed with `palFree()`
 *
 * @ingroup thread
 */
PAL_API char* PAL_CALL palGetThreadName(PalThread* thread);

/**
 * @brief Set the priority of the provided thread.
 *
 * `PAL_THREAD_FEATURE_PRIORITY` must be supported otherwise, this function will
 * fail and `PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED` will be returned.
 *
 * @param[in] thread The thread
 * @param[in] priority The new thread priority
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure.
 *
 * @note This function is not thread safe.
 *
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palSetThreadPriority(PalThread* thread, PalThreadPriority priority);

/**
 * @brief Set the affinity of the provided thread.
 *
 * `PAL_THREAD_FEATURE_AFFINITY` must be supported otherwise, this function will
 * fail and `PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED` will be returned. Thread
 * affinity is the number of CPU cores the thread is allowed to be executed on.
 *
 * To be safe, get the number of CPU cores and use that to build the CPU mask.
 * Example: we set a thread to the first and second CPU core.
 *
 * @code
 * Uint64 cpuMask = PAL_BIT(0) | PAL_BIT(1).
 * @endcode
 *
 * @param[in] thread The thread
 * @param[in] mask The CPU core mask
 *
 * @return @return `PAL_RESULT_SUCCESS` on success or an appropriate result code
 * on failure.
 *
 * @note This function is not thread safe.
 *
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palSetThreadAffinity(PalThread* thread, Uint64 mask);

/**
 * @brief Set the name of the provided thread.
 *
 * `PAL_THREAD_FEATURE_NAME` must be supported otherwise, this function will
 * fail and `PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED` will be returned.
 *
 * The `name` must be a UTF-8 encoding null terminated string.
 *
 * @param[in] thread The thread
 * @param[in] name UTF-8 encoding null terminated string.
 *
 * @return @return `PAL_RESULT_SUCCESS` on success or an appropriate result code
 * on failure.
 *
 * @note This function is not thread safe.
 *
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palSetThreadName(PalThread* thread, const char* name);

/**
 * @brief Create a new TLS
 *
 * The TLS handle can be used by multiple threads to associate thread local
 * vaules. The `destructor` will be called if `palDestroyTLS()` is called and
 * the TLS has a valid value.
 *
 * @param[in] destructor Pointer to the TLS destructor function. Can be
 * `nullptr`
 *
 * @return The TLS or `0` on failure
 *
 * @note This function is thread safe.
 *
 * @sa palSetTLS(), palGetTLS(), palDestroyTLS()
 * @ingroup thread
 */
PAL_API PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor);

/**
 * @brief Destroy the provided TLS.
 *
 * If the value of the TLS is valid and not `nullptr`, the destructor will be
 * called.
 *
 * @param[in] id The TLS
 *
 * @note This function is thread safe.
 *
 * @sa palSetTLS(), palGetTLS(), palCreateTLS()
 * @ingroup thread
 */
PAL_API void PAL_CALL palDestroyTLS(PalTLSId id);

/**
 * @brief Get the value associated with the provided TLS.
 *
 * The value is per thread.
 *
 * @param[in] id The TLS
 *
 * @return the value on success or `nullptr` on failure
 *
 * @note This function is thread safe.
 *
 * @sa palSetTLS(), palCreateTLS()
 * @ingroup thread
 */
PAL_API void* PAL_CALL palGetTLS(PalTLSId id);

/**
 * @brief Set the value associated with the provided TLS.
 *
 * The value is per thread. This function fails silently if the TLS is not
 * valid.
 *
 * @param[in] id The TLS
 * @param[in] data The value to set for the calling thread
 *
 * @note This function is thread safe.
 *
 * @sa palGetTLS(), palCreateTLS()
 * @ingroup thread
 */
PAL_API void PAL_CALL palSetTLS(PalTLSId id, void* data);

/**
 * @brief Create a new mutex
 *
 * @param[out] outThread Pointer to recieve the created mutex.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure.
 *
 * @note This function is thread safe if `outMutex` is thread local.
 *
 * @sa palDestroyMutex(), palLockMutex(), palUnlockMutex()
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palCreateMutex(PalMutex** outMutex);

/**
 * @brief Destroy a mutex
 *
 * If `mutex` is invalid, this function returns silently.
 * The mutex must be unlocked before destroying if it was locked.
 *
 * @param[in] mutex Pointer to the mutex.
 *
 * @note This function is not thread safe.
 *
 * @sa palCreateMutex(), palLockMutex(), palUnlockMutex()
 * @ingroup thread
 */
PAL_API void PAL_CALL palDestroyMutex(PalMutex* mutex);

/**
 * @brief Lock a mutex. Blocks if the mutex is alrrady locked by another thread
 *
 * @param[in] mutex Pointer to the mutex.
 *
 * @note This function is thread safe.
 *
 * @sa palCreateMutex(), palUnlockMutex()
 * @ingroup thread
 */
PAL_API void PAL_CALL palLockMutex(PalMutex* mutex);

/**
 * @brief Unlock a mutex.
 *
 * The function must be called by the thread that first locked it.
 *
 * @param[in] mutex Pointer to the mutex.
 *
 * @note This function is thread safe.
 *
 * @sa palCreateMutex(), palUnlockMutex()
 * @ingroup thread
 */
PAL_API void PAL_CALL palUnlockMutex(PalMutex* mutex);

/**
 * @brief Create a new condition variable
 *
 * @param[out] outCondition Pointer to recieve the created condition variable.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on
 * failure.
 *
 * @note This function is thread safe if `outCondition` is thread local.
 *
 * @sa palDestroyCondVar(), palWaitCondVar()
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palCreateCondVar(PalCondVar** outCondition);

/**
 * @brief Destroy a condition variable
 *
 * If `condition` is invalid, this function returns silently.
 * Threads must not wait on the condition variable otherwise undefined
 * behaviour.
 *
 * @param[in] condition Pointer to the condition.
 *
 * @note This function is not thread safe.
 *
 * @sa palCreateCondVar(), palWaitCondVar()
 * @ingroup thread
 */
PAL_API void PAL_CALL palDestroyCondVar(PalCondVar* condition);

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
 * @param[in] condition Pointer to the condition.
 * @param[in] mutex Pointer to the mutex.
 *
 * @note This function is thread safe.
 *
 * @sa palCreateMutex(), palCreateCondVar()
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palWaitCondVar(PalCondVar* condition, PalMutex* mutex);

/**
 * @brief Unlock the provided mutex and wait on the condition variable.
 *
 * The mutex must be locked before this call. Spurious wakeups may occur, its
 * best to use a loop. If the condition variable is not signaled but the time to
 * wait is up `PAL_RESULT_TIMEOUT` is returned.
 *
 * @param[in] condition Pointer to the condition.
 * @param[in] mutex Pointer to the mutex.
 * @param[in] milliseconds Timeout in milliseconds
 *
 * @note This function is thread safe.
 *
 * @sa palCreateMutex(), palCreateCondVar()
 * @ingroup thread
 */
PAL_API PalResult PAL_CALL palWaitCondVarTimeout(PalCondVar* condition, PalMutex* mutex,
                                                 Uint64 milliseconds);

/**
 * @brief Wake a single thread waiting on the condition variable.
 *
 * @param[in] condition Pointer to the condition.
 *
 * @note This function is thread safe.
 *
 * @sa palCreateCondVar(), palWaitCondVar()
 * @ingroup thread
 */
PAL_API void PAL_CALL palSignalCondVar(PalCondVar* condition);

/**
 * @brief Wake all threads waiting on the condition variable.
 *
 * @param[in] condition Pointer to the condition.
 *
 * @note This function is thread safe.
 *
 * @sa palCreateCondVar(), palSignalCondVar()
 * @ingroup thread
 */
PAL_API void PAL_CALL palBroadcastCondVar(PalCondVar* condition);

#endif // _PAL_THREAD_H