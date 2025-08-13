
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
 * @struct PalInputDevice
 * @brief Opaque handle to an input device.
 *
 * @ingroup input
 */
typedef struct PalInputDevice PalInputDevice;

/**
 * @enum PalInputMask
 * @brief bitmask used for input device enumeration.
 * This is a bitmask enum and multiple flags can be OR'ed together using bitwise OR operator (`|`).
 * 
 * @code
 * PalInputMask inputMask = PAL_INPUT_MASK_KEYBOARD | PAL_INPUT_MASK_GAMEPAD.
 * @endcode
 * @note All orientation follow the format `PAL_INPUT_MASK_**` for consistency and API use.
 *
 * @ingroup input
 */
typedef enum PalInputMask {
    PAL_INPUT_MASK_KEYBOARD = PAL_BIT(0),         /** < Enumerate keyboard devices.*/
    PAL_INPUT_MASK_MOUSE = PAL_BIT(1),            /** < Enumerate mouse devices.*/
    PAL_INPUT_MASK_GAMEPAD = PAL_BIT(2),          /** < Enumerate gamepad devices.*/

    /** < Enumerate all input devices.*/
    PAL_INPUT_MASK_ALL = PAL_INPUT_MASK_KEYBOARD | PAL_INPUT_MASK_MOUSE | PAL_INPUT_MASK_GAMEPAD
} PalInputMask;

/**
 * @enum PalInputDeviceType
 * @brief input device types. This is not a bitmask enum.
 *
 * @note All input device types follow the format `PAL_INPUT_DEVICE_**` for consistency and API use.
 *
 * @ingroup input
 */
typedef enum PalInputDeviceType {
    PAL_INPUT_DEVICE_KEYBOARD,             /** < Keyboard device.*/
    PAL_INPUT_DEVICE_MOUSE,                /** < Mouse device.*/
    PAL_INPUT_DEVICE_DUALSHOCK4,           /** < PS4 device.*/
    PAL_INPUT_DEVICE_DUALSENSE,            /** < Dualsense device.*/
    PAL_INPUT_DEVICE_XBOX_360,             /** < XBOX360 device.*/
    PAL_INPUT_DEVICE_XBOX_ONE,             /** < XBOX One device.*/
    PAL_INPUT_DEVICE_SWITCH_PRO,           /** < Nintento switch pro device.*/
    PAL_INPUT_DEVICE_GENERIC_GAMEPAD       /** < Generic USB gamepad device.*/
} PalInputDeviceType;

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
 * @struct PalInputDeviceInfo
 * @brief Contains information about an input device.
 *
 * @sa palGetInputDeviceInfo()
 *
 * @ingroup input
 */
typedef struct PalInputDeviceInfo {
    char name[64];                  /** < Name o the device.*/
    char path[128];                 /** < Path of the device.*/
    Uint32 vendorID;                /** < Vendor ID of the device.*/
    Uint32 productID;               /** < Product ID of the device.*/
    PalInputDeviceType type;        /** < The device type. see `PalInputDeviceType`.*/
} PalInputDeviceInfo;

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

/**
 * @brief Returns a list of active and connected input devices.
 *
 * User must `allocate` statically or dynamically and pass the maximum capacity of the
 * allocated array as `count` and pass the array itself as `inputDevices`.
 * The user is responsible for the life time of the array.
 *
 * The `count` should be the number, not the size in bytes. Example:
 * 
 * @code
 * PalInputDevice* inputDevices[2];
 * @endcode
 * 
 * @code
 * Int32 count = 2;
 * @endcode
 *
 * you can set the `inputDevices` to nullptr and PAL will set the count of the connected displays to `count`.
 * If the `count` is zero or less than zero, the function returns `PAL_RESULT_INSUFFICIENT_BUFFER`.
 * If `count` is less than the connected input devices, PAL will write up to `count`.
 *
 * @param[in] system Pointer to the input system instance.
 * @param[in] mask Bitmask of input devices to enumerate.
 * @param[in] count Capacity of the `inputDevices` array.
 * @param[out] inputDevices User allocated array of PalInputDevice.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note Users must not free the display handles, they are managed by the OS.
 * Users are required to cache this, and call this function again if input devices are added or removed.
 *
 * @note This function is thread-safe if `inputDevices` is thread local.
 * If not, the user is responsible for synchronization.
 *
 * @sa palCreateInputSystem(), PalInputDeviceInfo
 * @ingroup input
 */
_PAPI PalResult _PCALL palEnumerateInputDevices(
    PalInputSystem* system,
    PalInputMask mask,
    Int32* count,
    PalInputDevice** inputDevices);

/**
 * @brief Get information about an input device.
 *
 * This takes in a PalInputDeviceInfo struct and fills it.
 * Some of the fields are set to defaults if operation is not supported on the OS.
 * example: standard mouse and keyboard might not provide a vendor and product id,
 * so PAL will set it to `0`.
 *
 * @param[in] inputDevice Input device.
 * @param[out] info Pointer to the PalInputDeviceInfo struct to fill.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result code on failure.
 *
 * @note This function is thread-safe if `info` is thread local.
 * If not, the user is responsible for synchronization.
 *
 * @sa palEnumerateInputDevices(), PalInputDeviceInfo
 * @ingroup input
 */
_PAPI PalResult _PCALL palGetInputDeviceInfo(
    PalInputDevice* inputDevice, 
    PalInputDeviceInfo* info);

#endif // _PAL_INPUT_H