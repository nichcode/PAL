
#ifndef _PAL_INPUT_H
#define _PAL_INPUT_H

#include "pal_core.h"

PAL_DECLARE_HANDLE(PalInput);
PAL_DECLARE_HANDLE(PalInputDevice);
PAL_DECLARE_HANDLE(PalEventDriver);

typedef enum PalInputDeviceType {
    PAL_INPUT_DEVICE_UNKNOWN = 0,
    PAL_INPUT_DEVICE_KEYBOARD,
    PAL_INPUT_DEVICE_MOUSE,
    PAL_INPUT_DEVICE_DUALSHOCK4,
    PAL_INPUT_DEVICE_DUALSENSE,
    PAL_INPUT_DEVICE_XBOX_360,
    PAL_INPUT_DEVICE_XBOX_ONE,
    PAL_INPUT_DEVICE_SWITCH_PRO,
    PAL_INPUT_DEVICE_GAMEPAD,
    PAL_INPUT_DEVICE_HID
} PalInputDeviceType;

typedef struct PalInputDeviceInfo {
    char name[64];
    char path[128];
    Uint32 vendorID;
    Uint32 productID;
    PalInputDeviceType type;
} PalInputDeviceInfo;

typedef struct PalInputConfig {
    PalAllocator* allocator;
    PalEventDriver eventDriver;
} PalInputConfig;

_PAPI PalResult _PCALL palCreateInput(
    PalInputConfig* config,
    PalInput* outInput);

_PAPI void _PCALL palDestroyInput(
    PalInput input);

_PAPI PalResult _PCALL palUpdateInput(
    PalInput input);

_PAPI PalResult _PCALL palEnumerateInputDevices(
    PalInput input,
    int* count,
    PalInputDevice* inputDevices);

_PAPI PalResult _PCALL palGetInputDeviceInfo(
    PalInputDevice inputDevice, 
    PalInputDeviceInfo* info);

#endif // _PAL_INPUT_H