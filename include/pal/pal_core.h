
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
 * @file pal_core.h
 *
 * Header file for core functions, macros, enum and structs
 *
 * @defgroup core
 *
 */

#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Set up shared library dependencies
#ifdef _WIN32
#define _PCALL __stdcall
#ifdef _PAL_EXPORT
#define _PAPI __declspec(dllexport)
#else 
#define _PAPI __declspec(dllimport)
#endif // PAL_EXPORT
#else
// other platforms
#define _PCALL
#ifdef PAL_EXPORT
#define _PAPI extern "C" __attribute__((visibility("default")))
#else 
#define _PAPI
#endif // PAL_EXPORT
#endif // _WIN32

#ifdef __cplusplus
}
#endif // __cplusplus

// systems reflection
#ifdef _PAL_BUILD_VIDEO
#define PAL_HAS_VIDEO
#endif // _PAL_BUILD_VIDEO

// Set up typedefs for C
#ifndef __cplusplus
#define nullptr ((void *)0)
#define true 1
#define false 0

/**
 * @brief A bool
 * @warning This works from C99
 */
typedef _Bool bool;
#endif // __cplusplus

#define PAL_BIT(x) 1 << x

/**
 * @brief An unsigned 8-bit integer
 */
typedef uint8_t Uint8;

/**
 * @brief An signed 8-bit integer
 */
typedef int8_t Int8;

/**
 * @brief An unsigned 16-bit integer
 */
typedef uint16_t Uint16;

/**
 * @brief An signed 16-bit integer
 */
typedef int16_t Int16;

/**
 * @brief An unsigned 32-bit integer
 */
typedef uint32_t Uint32;

/**
 * @brief An signed 32-bit integer
 */
typedef int32_t Int32;

/**
 * @brief An unsigned 64-bit integer
 */
typedef uint64_t Uint64;

/**
 * @brief An unsigned 64-bit integer
 */
typedef int64_t Int64;

/**
 * @struct PalEventDriver
 * @brief Opaque handle to an event driver.
 *
 * @ingroup video
 */
typedef struct PalEventDriver PalEventDriver;

/**
 * @struct PalEvent
 * @brief Opaque handle to an event. This describes a single event.
 *
 * @ingroup video
 */
typedef struct PalEvent PalEvent;

/**
 * @typedef PalEventCallback
 * @brief Function pointer type used for callback events.
 * 
 * This is the callback which will be called if user enables an event with the callback mode.
 * See `PalDispatchMode`.
 * 
 * @param[in] userData Pointer passed in the function. This is from `PalEventDriver`.
 * @param[in] event The event triggered.
 * 
 * @sa PalEventDriverCreateInfo
 * @ingroup core
 */
typedef void (*PalEventCallback)(
    void* userData, 
    const PalEvent* event);

/**
 * @typedef PalPushFn
 * @brief Function pointer type used for pushing events onto an event queue.
 * 
 * The `event` must be initialized and explicitly set by the user and then passed in the function.
 * The `userData` is the same pointer passed into the allocator struct.
 * 
 * @param[in] userData Pointer provided by the user.
 * @param[in] event The event to push onto the queue.
 * 
 * @sa PalEventQeue, PalPollFn, palPushEvent(), palPushEvent()
 * @ingroup core
 */
typedef void (*PalPushFn)(
    void* userData, 
    PalEvent* event);

/**
 * @typedef PalPollFn
 * @brief Function pointer type used for polling events from an event queue.
 *
 * The `userData` is the same pointer passed into the allocator struct.
 * If the event queue is empty or the `outEvent` is nullptr, the returns `false`, 
 * but `true` if the `outEvent` was filled.
 * 
 * @return True on success or false on failure or queue empty.
 * 
 * @param[in] userData Pointer provided by the user.
 * @param[out] outEvent The event to push onto the queue.
 * 
 * @sa PalEventQeue, PalPushFn, palPushEvent(), palPushEvent()
 * @ingroup core
 */
typedef bool (*PalPollFn)(
    void* userData, 
    PalEvent* outEvent);

/**
 * @typedef PalAllocateFn
 * @brief Function pointer type used for memory allocations.
 * 
 * Allocates `size` bytes with `alignment` and return a pointer or nullptr on failure.
 * The `userData` is the same pointer passed into the allocator struct.
 * 
 * @param[in] userData Pointer provided by the user.
 * @param[in] size Size in bytes to allocate.
 * @param[in] alignment Must be power of two and at least `8` or `16`. If zero `16` will be used.
 * 
 * @return Pointer to the allocated memory or nullptr if allocation failed.
 * 
 * @sa PalAllocator, PalFreeFn, palAllocate
 * @ingroup core
 */
typedef void* (*PalAllocateFn)(
    void* userData, 
    Uint64 size, 
    Uint64 alignment);

/**
 * @typedef PalFreeFn
 * @brief Function pointer type used for memory deallocations.
 * 
 * Deallocate memory allocated by PalAllocateFn. 
 * Passing nullptr or an invalid pointer for `ptr` must be safe.
 *
 * @param[in] userData Optional pointer provided by the user. May be a nullptr.
 * @param[in] ptr Pointer to the memory to free. May be a nullptr.
 * 
 * @sa PalAllocateFn, PalAllocator
 * @ingroup core
 */
typedef void (*PalFreeFn)(void* userData, void* ptr);

/**
 * @enum PalEventType
 * @brief Types for an event. This is not a bitmask enum.
 *
 * @note All event types follow the format `PAL_EVENT_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum PalEventType {
    PAL_EVENT_WINDOW_CLOSE,              /** < The window close button was clicked.*/
    PAL_EVENT_WINDOW_RESIZE,             /** < The window was resized.*/
    PAL_EVENT_WINDOW_MOVE,               /** < The window was moved.*/
    PAL_EVENT_DPI_CHANGED,               /** < The display dpi has changed.*/
    PAL_EVENT_DISPLAYS_CHANGED,          /** < The display list has changed. Enumerate (`palEnumerateDisplays()`) again to get valid handles..*/
    PAL_EVENT_USER,                      /** < User event. Differentiate between them with userID field in PalEvent.*/
    PAL_EVENT_MAX
} PalEventType;

/**
 * @enum PalDispatchMode
 * @brief Dispatch mode for an event. This is not a bitmask enum.
 *
 * @note All dispatch modes follow the format `PAL_DISPATCH_**` for consistency and API use.
 *
 * @ingroup video
 */
typedef enum PalDispatchMode {
    PAL_DISPATCH_NONE,                       /** < Dont't dispatch event.*/
    PAL_DISPATCH_POLL,                       /** < Dispatch to the event queue.*/
    PAL_DISPATCH_CALLBACK                    /** < Dispatch to event callback.*/
} PalDispatchMode;

/**
 * @enum PalResult
 * @brief codes returned by PAL functions.
 * 
 * Aside from the core system, PAL functions return a PalResult.
 * 
 * `PAL_RESULT_SUCCESS` code means the operation completed successfully. 
 * Any other value indicates an error.
 * 
 * @note For clarity, always use the named constants like 
 * `PAL_RESULT_OUT_OF_MEMORY` not their numeric values.
 * 
 * @note All result follow the format `PAL_RESULT_**` for consistency and API use.
 */
typedef enum PalResult {
    PAL_RESULT_SUCCESS,                           /** < Operation was successful.*/
    PAL_RESULT_NULL_POINTER,                      /** < A nullptr was used where it is not allowed.*/
    PAL_RESULT_INVALID_ARGUMENT,                  /** < One or more arguments were invalid.*/
    PAL_RESULT_OUT_OF_MEMORY,                     /** < Out of Memory or memory allocation failed.*/
    PAL_RESULT_INVALID_ALLOCATOR,                 /** < A partially defined custom allocator.*/
    PAL_RESULT_ACCESS_DENIED,                     /** < OS denied PAL access.*/
    PAL_RESULT_INVALID_DISPLAY,                   /** < An invalid display.*/
    PAL_RESULT_INSUFFICIENT_BUFFER,               /** < Buffer too small.*/
    PAL_RESULT_INVALID_DISPLAY_MODE,              /** < An invalid display mode.*/
    PAL_RESULT_INVALID_ORIENTATION,               /** < An invalid display orientation.*/
    PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED,       /** < A video feature used was not supported.*/
    PAL_RESULT_VIDEO_DEVICE_NOT_FOUND,            /** < OS video driver could not be found (busy).*/
    PAL_RESULT_INVALID_WINDOW,                    /** < An invalid window.*/
    PAL_RESULT_INVALID_OPERATION,                 /** < An invalid operation.*/
    PAL_RESULT_INPUT_DEVICE_NOT_FOUND,            /** < OS input driver could not be found (busy).*/
    PAL_RESULT_INVALID_INPUT_DEVICE,              /** < An invalid input device.*/
} PalResult;

/**
 * @struct PalVersion
 * @brief Describes the version of the PAL library.
 *
 * @ingroup core
 */
typedef struct PalVersion {
    Int32 major;         /** < Major version (breaking changes).*/
    Int32 minor;         /** < Minor version (features additions).*/
    Int32 patch;         /** < Patch version (bug fixes only). */
} PalVersion;

/**
 * @struct PalAllocator
 * @brief Describes a user provided memory allocator.
 * 
 * Allows the user to override the default allocator for PAL.
 *
 * @ingroup core
 */
typedef struct PalAllocator {
    PalAllocateFn allocate;    /** < Allocation function pointer.*/
    PalFreeFn free;            /** < Free function pointer.*/
    void* userData;            /** < Optional user data passed to allocation functions.*/
} PalAllocator;

/**
 * @struct PalTimer
 * @brief Describes a high resolution timer
 *
 * @ingroup core
 */
typedef struct PalTimer {
    Uint64 startTime;        /** < Start time.*/
    Uint64 duration;         /** < Duration in seconds.*/
} PalTimer;

/**
 * @struct PalTimer
 * @brief Describes a clock for time related calculations.
 *
 * @ingroup core
 */
typedef struct PalClock {
    Uint64 frequency;        /** < Ticks per second.*/
    Uint64 startTime;        /** < Start time.*/
} PalClock;

/**
 * @struct PalEvent
 * @brief Opaque handle to an event. This describes a single event.
 *
 * @ingroup video
 */
struct PalEvent {
    PalEventType type;           /** < The type of the event. See `PalEventType`.*/
    Int64 data;                  /** < First data payload.*/
    Int64 data2;                 /** < Second data payload.*/
    Uint64 userID;               /** < This is for user events.*/
    Uint64 sourceID;             /** < ID of what generated the event. Example: a window, etc.*/
};

/**
 * @struct PalEventQueue
 * @brief Describes a user provided event queue.
 * 
 * Allows the user to override the default event queue when creating an event driver.
 *
 * @ingroup core
 */
typedef struct PalEventQueue {
    PalPushFn push;             /** < Event push function pointer.*/
    PalPollFn poll;             /** < Event poll function pointer.*/
    void* userData;             /** < Optional user data passed to queue functions.*/
} PalEventQueue;

/**
 * @struct PalEventDriverCreateInfo
 * @brief Specifies options for creating an event driver.
 *
 * This struct must be initialized and passed to `palCreateEventDriver()`.
 *
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct PalEventDriverCreateInfo {
    PalAllocator* allocator;           /** < User allocator or nullptr for default.*/
    PalEventQueue* queue;              /** < User privided event queue. Set to nullptr to use default.*/
    PalEventCallback callback;         /** < Event callback to call for callback type events. Can be nullptr.*/
    void* userData;                    /** < Optional user data passed to event callback function.*/
} PalEventDriverCreateInfo;

/**
 * @brief Return a human readable string for a specified result code.
 * 
 * This returns a constant, null-terminated string describing the result.
 * 
 * @param[in] result The result code to describe
 * @return A pointer to the null-terminated result string.
 * 
 * @note This function is thread-safe and the pointer should not be freed.
 * @sa PalResult
 */
_PAPI const char* _PCALL palResultToString(PalResult result);

/**
 * @brief Get the runtime version of PAL.
 * 
 * This returns a copy of PALVersion struct, 
 * which contains the major, minor and patch of the PAL runtime version.
 * 
 * This version reflects the compiled PAL library and it can be used to validate compatibility.
 *
 * @return A copy of PAL runtime version.
 * @note This function is thread-safe.
 *
 * @sa palGetVersionString(), PalVersion
 * @ingroup core
 */
_PAPI PalVersion _PCALL palGetVersion();

/**
 * @brief Get the human readable string representing the runtime version PAL.
 *
 * This returns a constant, null-terminated string describing the PAL runtime version
 * in the format: `1.2.3`, where `1` is the major, `2` is the minor and `3` is the patch versions respectively.
 *
 * @return A pointer to the null-terminated version string.
 * @note This function is thread-safe and the pointer should not be freed.
 *
 * @sa palGetVersion(), PalVersion
 * @ingroup core
 */
_PAPI const char* _PCALL palGetVersionString();

/**
 * @brief Allocate memory with a custom or default allocator.
 * 
 * If a valid allocator is provided, 
 * its `allocate` function is called with the provided size and user data.
 * Otherwise, the default allocator is used.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator will be used if it is a `nullptr`.
 * @param[in] size Size in bytes to allocate.
 * @param[in] alignment Must be power of two and at least `8` or `16`. If zero `16` will be used.
 * 
 * @return Pointer to the allocated memory or a nullptr if allocation failed.
 *
 * @note This does not initialize the allocated memory.
 * This function is thread safe if the provided allocator is thread safe.
 * @sa palFree(), PalAllocator
 * @ingroup core
 */
_PAPI void* _PCALL palAllocate(
    PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment);
    
/**
 * @brief Free memory with a custom or default allocator.
 * 
 * If a valid allocator is provided, 
 * its `free` function is called with the provided ptr and user data.
 * Otherwise, the default allocator is used.
 *
 * @param[in] allocator Optional pointer to an allocator. The default allocator will be used if it is a `nullptr`.
 * @param[in] ptr The pointer to the memory block to free.
 *
 * @note This function is thread safe if the provided allocator is thread safe.
 * @sa PalAllocator, palAllocate
 * @ingroup core
 */
_PAPI void _PCALL palFree(
    PalAllocator* allocator,
    void* ptr);

/**
 * @brief Log a formatted message.
 *
 * This supports unicode characters.
 *
 * @param[in] fmt UTF-8 encoding printf-style format string.
 *
 * Example:
 * @code
 * palLog("%s - %f", string, float);
 * @endcode
 *
 * @note This function is thread-safe. 
 * This will do nothing if there's no console on the OS.
 * @ingroup core
 */
_PAPI void _PCALL palLog(const char* fmt, ...);

/**
 * @brief Get the system clock.
 * 
 * This is used to measure elasped time and for time related calculations.
 * 
 * See `tests/src/time_test` for an example.
 * 
 * @return The system clock.
 * 
 * @sa palGetTime(), PalClock, palGetPerformanceCounter(), palGetPerformanceFrequency()
 *
 * @note This function is thread-safe.
 * @ingroup core
 */
_PAPI PalClock _PCALL palGetSystemClock();

/**
 * @brief Get the current time in seconds using a clock.
 * 
 * This returns the current absolute time in seconds.
 *
 * @param[in] clock The clock to use. see palGetSystemClock().
 * @return Current high resolution time in seconds or `0.0` if `clock` is nullptr.
 * 
 * @note This function is thread-safe. This should not be used to measure calender time.
 * @sa palGetPerformanceCounter(), palGetPerformanceFrequency()
 * @ingroup core
 */
_PAPI double _PCALL palGetTime(PalClock* clock);

/**
 * @brief Get the system performance counter value in ticks
 * 
 * This can be used to measure elasped time when divided by the performance frequency.
 * This is done by `palGetTime()`
 * 
 * @return Current tick count from the system
 * @sa palGetTime(), palGetPerformanceFrequency
 * @note This function is thread-safe.
 * @ingroup core
 */
_PAPI Uint64 _PCALL palGetPerformanceCounter();

/**
 * @brief Get the frequency of the system performance counter.
 * 
 * This function returns the number of ticks per second of the system performance counter.
 * @return Frequency in ticks per second.
 * 
 * @sa palGetTime(), palGetPerformanceCounter
 * @note This function is thread-safe. 
 * 
 * @note The returned frequency is constant for the duration of the application.
 * @ingroup core
 */
_PAPI Uint64 _PCALL palGetPerformanceFrequency();

/**
 * @brief Creates an event driver used to signal events between multiple systems.
 * 
 * The event driver can be shared across multiple systems. 
 * Example: the same event driver for both video and input systems.
 * 
 * The user is responsible for destroying the event driver when no longer needed.
 * If its reference by other systems, those systems must be destroyed before destroying the event driver.
 *
 * @param[in] info Pointer to a PalEventDriverCreateInfo struct with creation options.
 * @param[out] outEventDriver Pointer to recieve the created event driver.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * the user is responsible for synchronization.
 * @note The created event driver must be destroyed with `palDestroyEventDriver()` when no longer needed.
 *
 * @sa PalEventDriverCreateInfo, palDestroyEventDriver()
 * @ingroup core
 */
_PAPI PalResult _PCALL palCreateEventDriver(
    PalEventDriverCreateInfo* info, 
    PalEventDriver** outEventDriver);

/**
 * @brief Destroy the event driver.
 *
 * This function can be called multiple times without any undefined behavior.
 * If the event driver is invalid or a nullptr, the function returns silently.
 *
 * @param[in] eventDriver Pointer to the event driver to destroy.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * the user is responsible for synchronization.
 *
 * @sa palCreateEventDriver()
 * @ingroup core
 */
_PAPI void _PCALL palDestroyEventDriver(PalEventDriver* eventDriver);

/**
 * @brief Sets the dispatch mode for a single event type for the given event driver.
 * 
 * If dispatch mode is `PAL_DISPATCH_POLL`, the event will be dispatched to the event queue.
 * If dispatch mode is `PAL_DISPATCH_CALLBACK`, the event will be dispatched to the event callback function. 
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type Event type to set dispatch for.
 * @param[in] mode Dispatch mode to use. See `PalDispatchMode`.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * the user is responsible for synchronization.
 * @note This function is guaranteed not to fail if `eventDriver` is valid.
 *
 * @sa palCreateEventDriver(), PalDispatchMode, PalEventType
 * @ingroup core
 */
_PAPI void _PCALL palSetEventDispatchMode(
    PalEventDriver* eventDriver, 
    PalEventType type, 
    PalDispatchMode mode);

/**
 * @brief Sets the dispatch mode for all event types for the given event driver.
 * 
 * If dispatch mode is `PAL_DISPATCH_POLL`, all events will be dispatched to the event queue.
 * If dispatch mode is `PAL_DISPATCH_CALLBACK`, all events will be dispatched to the event callback function. 
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] mode Dispatch mode to use. See `PalDispatchMode`.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * the user is responsible for synchronization.
 * @note This function is guaranteed not to fail if `eventDriver` is valid.
 *
 * @sa palCreateEventDriver(), PalDispatchMode, PalEventType
 * @ingroup core
 */
_PAPI void _PCALL palSetAllEventDispatchMode(
    PalEventDriver* eventDriver, 
    PalDispatchMode mode);

/**
 * @brief Get the dispatch mode for the event type.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] type Event type to get its dispatch mode.
 * 
 * @return The dispatch mode on success or `PAL_DISPATCH_NONE` on failure.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if `eventDriver` is valid.
 *
 * @sa palCreateEventDriver(), PalDispatchMode, PalEventType
 * @ingroup core
 */
_PAPI PalDispatchMode _PCALL palGetEventDispatchMode(
    PalEventDriver* eventDriver, 
    PalEventType type);

/**
 * @brief Push an event onto the event queue of an event driver.
 * 
 * If the dispatch mode for `event` is `PAL_DISPATCH_POLL`, 
 * the event will be pushed to the event queue. 
 * Otherwise if dispatch mode is `PAL_DISPATCH_CALLBACK`,
 * the event callback function will be called. 
 * 
 * This function fails silently if the dispatch mode is `PAL_DISPATCH_CALLBACK`
 * and the user did not set a valid event callback function.
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[in] event Pointer to an event. This should be initialized and explicitly set by the user.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if `eventDriver` and `event` are valid.
 *
 * @sa palCreateEventDriver(), PalDispatchMode, PalEvent
 * @ingroup core
 */
_PAPI void _PCALL palPushEvent(
    PalEventDriver* eventDriver,
    PalEvent* event);

/**
 * @brief Retrieve the next available event from the event queue of an event driver.
 * 
 * This functions retrieves the next pending event from the event queue of an event driver without blocking.
 * If no events are available, it returns `false`.
 *
 * If `eventDriver` is not valid, this function returns `false`
 *
 * @param[in] eventDriver Pointer to the event driver.
 * @param[out] outEvent Pointer to recieve the event from the event queue. Must be valid.
 *
 * @note This function is thread-safe.
 * @note This function is guaranteed not to fail if `eventDriver` and `outEvent` are valid.
 *
 * @sa palCreateEventDriver(), PalEvent
 * @ingroup core
 */
_PAPI bool _PCALL palPollEvent(
    PalEventDriver* eventDriver,
    PalEvent* outEvent);

/**
 * @brief Combine two 32-bit unsigned integers into a 64-bit signed integer.
 * 
 * @return The combined 64-bit signed integer.
 * 
 * @sa palPackInt32(), palUnpackUint32(), palUnpackInt32()
 * @note This function is thread-safe. 
 * 
 * @ingroup core
 */
static inline Int64 palPackUint32(
    Uint32 low,
    Uint32 high) {

    return (Int64) (((Uint64)high << 32) | (Uint64)low);
}

/**
 * @brief Combine two 32-bit signed integers into a 64-bit signed integer.
 * 
 * @return The combined 64-bit signed integer.
 * 
 * @sa palPackUint32(), palUnpackUint32(), palUnpackInt32()
 * @note This function is thread-safe. 
 * 
 * @ingroup core
 */
static inline Int64 palPackInt32(
    Int32 low,
    Int32 high) {

    return ((Int64) (Uint32)high << 32) | (Uint32)low;
}

/**
 * @brief Unpack a 64-bit signed integer into two 32-bit unsigned integers.
 * 
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 * 
 * @sa palPackUint32(), palUnpackInt32()
 * @note This function is thread-safe. 
 * 
 * @ingroup core
 */
static inline void palUnpackUint32(
    Int64 data,
    Uint32* outLow,
    Uint32* outHigh) {

    if (outLow) {
        *outLow = (Uint32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Uint32)((Uint64)data >> 32);
    }
}

/**
 * @brief Unpack a 64-bit signed integer into two 32-bit signed integers.
 * 
 * @param[out] outLow Low value of the 64-bit signed integer.
 * @param[out] outHigh High value of the 64-bit signed integer.
 * 
 * @sa palPackUint32(), palUnpackUint32()
 * @note This function is thread-safe. 
 * 
 * @ingroup core
 */
static inline void palUnpackInt32(
    Int64 data,
    Int32* outLow,
    Int32* outHigh) {

    if (outLow) {
        *outLow = (Int32)(data & 0xFFFFFFFF);
    }

    if (outHigh) {
        *outHigh = (Int32)((Uint64)data >> 32);
    }
}

#endif // _PAL_CORE_H