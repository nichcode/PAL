
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
 * @file pal_input.h
 *
 * Header file for input functions, macros, enum and structs
 *
 * @defgroup input
 */

#ifndef _PAL_INPUT_H
#define _PAL_INPUT_H

#include "pal_core.h"

/**
 * @struct PalInputSystem
 * @brief Opaque handle to a input system.
 *
 * @ingroup input
 */
typedef struct PalInputSystem PalInputSystem;

/**
 * @struct PalInputSystemCreateInfo
 * @brief Specifies options for creating an instance of the input system.
 *
 * This struct must be initialized and passed to `palCreateInputSystem()`.
 *
 * All fields must be explicitly set by the user.
 *
 * @note Uninitialized fields may result in undefined behavior.
 *
 * @ingroup input
 */
typedef struct PalInputSystemCreateInfo {
    PalAllocator *allocator;             /** < User allocator or nullptr for default.*/
    PalEventDriver *eventDriver;         /** < Event driver to push input events.*/
} PalInputSystemCreateInfo;

/**
 * @brief Create an instance of the input system.
 *
 * An input system is neeed to enumerate connected input devices,
 * register input devices and poll state for the registered devices.
 *
 * The user must call this function before any other input related functions.
 * The `eventDriver` field in `info` must be valid to recieve input related events.
 *
 * @param[in] info Pointer to a PalInputSystemCreateInfo struct with creation options.
 * @param[out] outSystem Pointer to recieve the created input system instance.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * the user is responsible for synchronization.
 *
 * @note The created input system instance must be destroyed with `PalDestroyInputSystem()` when no longer needed.
 *
 * @sa PalInputSystemCreateInfo, palDestroyInputSystem()
 * @ingroup input
 */
_PAPI PalResult _PCALL palCreateInputSystem(
    const PalInputSystemCreateInfo *info,
    PalInputSystem **outSystem);

/**
 * @brief Destroys an instance of the input system.
 *
 * After this call, the pointer will be invalid and should not be used anymore.
 *
 * This function can be called multiple times without any undefined behavior.
 * If the input system instance is invalid or `nullptr`, this function returns silently.
 *
 * @param[in] system Pointer to the input system instance to destroy.
 *
 * @note This function is not thread-safe. If multiple threads will call this function,
 * the user is responsible for synchronization.
 * All resources created through the input system must be destroyed before this call.
 *
 * @sa palCreateInputSystem()
 * @ingroup input
 */
_PAPI void _PCALL palDestroyInputSystem(PalInputSystem *system);

#endif // _PAL_INPUT_H