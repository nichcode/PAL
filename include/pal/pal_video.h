
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
 * @file pal_video.h
 * 
 * Header file for video functions, macros, enum and structs
 * 
 * @defgroup video 
 */

#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

/**
 * @struct PalVideoSystem
 * @brief Opaque handle to a video system.
 *
 * @ingroup video
 */
typedef struct PalVideoSystem PalVideoSystem;

/**
 * @struct PalDisplay
 * @brief Opaque handle to a video display (monitor).
 *
 * @ingroup video
 */
typedef struct PalDisplay PalDisplay;

/**
 * @struct PalVideoSystemCreateInfo
 * @brief Specifies options for creating an instance of the video system.
 * 
 * This struct must be initialized and passed to palCreateVideoSystem().
 * 
 * All fields must be explicitly set by the user.
 * 
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup video
 */
typedef struct {
    PalAllocator* allocator;         /** < User allocator or nullptr for default.*/
} PalVideoSystemCreateInfo;

/**
 * @struct PalDisplayInfo
 * @brief Contains information about a display.
 * All values are in physical pixels.
 * 
 * @sa palGetDisplayInfo()
 *
 * @ingroup video
 */
typedef struct {
    char name[32];          /** < Name of the display.*/
    int x;                  /** < X position of yhe display.*/
    int y;                  /** < Y position of the display.*/
    Uint32 width;           /** < Width of the display in pixels.*/
    Uint32 height;          /** < Height of the display in pixels.*/
    Uint32 dpi;             /** < Display dpi.*/
    Uint32 refreshRate;     /** < Refresh rate in HZ.*/
    bool primary;           /** < True if this is the primary display.*/
} PalDisplayInfo;

/**
 * @brief Create an instance of the video system.
 * 
 * A video system is neeed to enumerate connected displays (monitors), 
 * get supported video features, get supported window styles 
 * and window creation.
 * 
 * The user must call this function before any other video related functionality,
 * to obtain the video system handle which will be passed in other video related functions.
 *
 * @param[in] info A pointer to a PalVideoSystemCreateInfo struct with creation options.
 * @param[out] outSystem Pointer to the created video system handle.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * then the user is responsible for synchronization.
 * 
 * @note The created video system instance must be destroyed with `PalDestroyVideoSystem()`
 * when no longer needed.
 * 
 * @sa PalVideoSystemCreateInfo, palDestroyVideoSystem()
 * @ingroup video
 */
_PAPI PalResult _PCALL palCreateVideoSystem(
    const PalVideoSystemCreateInfo* info,
    PalVideoSystem** outSystem);

/**
 * @brief Destroy the instance of the video system.
 * 
 * After this call, the pointer will be invalid and should not be used anymore.
 * 
 * This function can be called multiple times without any undefined behavior.
 * If the video system handle is invalid or a nullptr, the function returns silently.
 * 
 * @param[in] system A pointer to the video system handle to destroy.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * then the user is responsible for synchronization.
 * All resources created through the video system must be destroyed before this call.
 * 
 * @sa palCreateVideoSystem()
 * @ingroup video
 */
_PAPI void _PCALL palDestroyVideoSystem(
    PalVideoSystem* system);

/**
 * @brief Returns a list of active and connected displays (monitors).
 * 
 * User must `allocate` statically and dynamically and pass the maximum capacity of the
 * allocated array as `count` and pass the array itself as `displays`.
 * The user is responsible for the life time of the array.
 * 
 * The `count` should be the number, not the size in bytes. Example:
 * @code
 * PalDisplay** displays[2];
 * int count = 2;
 * @endcode
 * 
 * you can set the `displays` to nullptr and PAL will set the count of the connected displays
 * to `count`. 
 * If the `count` is zero or less than zero, the function returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 * If `count` is less than the connected displays, PAL will write up to count.
 * 
 * @param[in] system A pointer to the video system handle.
 * @param[in] count The capacity of the `displays` array.
 * @param[in] displays User allocated array of PalDisplay.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 * 
 * @note Users must not free the display handles, they are managed by the OS.
 * Users are required to cache this, and call this again of displays changed.
 * 
 * @note This function is thread-safe if `displays` is thread local.
 * If not, then the user is responsible for synchronization.
 * 
 * @sa palCreateVideoSystem(), palGetPrimaryDisplay(), PalDisplayInfo
 * @ingroup video
 */
_PAPI PalResult _PCALL palEnumerateDisplays(
    PalVideoSystem* system,
    int* count,
    PalDisplay** displays);

/**
 * @brief Get the primary active display (monitors).
 * 
 * The primary display can changed based on OS settings.
 * 
 * @param[in] system A pointer to the video system handle.
 * @param[out] outDisplay User allocated PalDisplay handle to recieve the primary display.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 * 
 * @note Users must not free the display handle, it is managed by the OS.
 * 
 * @note This function is thread-safe if `outDisplay` is thread local.
 * If not, then the user is responsible for synchronization.
 * 
 * @sa palCreateVideoSystem(), palEnumerateDisplays(), PalDisplayInfo
 * @ingroup video
 */
_PAPI PalResult _PCALL palGetPrimaryDisplay(
    PalVideoSystem* system,
    PalDisplay** outDisplay);

/**
 * @brief Get information about a display (monitor).
 * 
 * This takes in a PalDisplayInfo struct and fills it.
 * Some of the fields are set to defaults if operation is not supported on the OS.
 * example: On Windows 7, DPI will always be 96.
 * 
 * @param[in] display A valid display handle.
 * @param[in] info Pointer to the PalDisplayInfo struct to fill.
 * 
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 * 
 * @note This function is thread-safe if `info` is thread local.
 * If not, then the user is responsible for synchronization.
 * 
 * @sa palEnumerateDisplays(), palGetPrimaryDisplay(), PalDisplayInfo
 * @ingroup video
 */
_PAPI PalResult _PCALL palGetDisplayInfo(
    PalDisplay* display, 
    PalDisplayInfo* info);

#endif // _PAL_VIDEO_H