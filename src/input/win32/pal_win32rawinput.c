
#include "pal_pch.h"
#include "pal_win32input.h"

bool palGetRawDeviceInfo(
    PalInputDevice device, 
    PalInputDeviceInfo* info) {

    HANDLE rawDevice = (HANDLE)device;
    RID_DEVICE_INFO deviceInfo = {};
    deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
    Uint32 size = sizeof(RID_DEVICE_INFO);
    if (GetRawInputDeviceInfoA(
        device,
        RIDI_DEVICEINFO,
        &deviceInfo,
        &size) == (UINT)-1) {
        return PAL_FALSE;
    }

    switch (deviceInfo.dwType) {
        case RIM_TYPEKEYBOARD: {
            strcpy(info->name, "Standard Keyboard");
            info->productID = 0;
            info->vendorID = 0;
            info->type = PAL_INPUT_DEVICE_KEYBOARD;
            break;
        }
        case RIM_TYPEMOUSE: {
            strcpy(info->name, "Standard Mouse");
            info->productID = 0;
            info->vendorID = 0;
            info->type = PAL_INPUT_DEVICE_MOUSE;
            break;
        }

        case RIM_TYPEHID: {
            int usagePage = deviceInfo.hid.usUsagePage;
            int usage = usage;

            if (usagePage == 0x01 && usage == 0x06) {
                info->type = PAL_INPUT_DEVICE_KEYBOARD;
                break;

            } else if (usagePage == 0x01 && usage == 0x02) {
                info->type = PAL_INPUT_DEVICE_MOUSE;
                break;

            } else if (usagePage == 0x01 && usage == 0x05) {
                info->type = PAL_INPUT_DEVICE_GAMEPAD;
                break;
                
            } else {
                info->type = PAL_INPUT_DEVICE_HID;
                break;
            }

            // TODO: HIDAPI
            break;
        }
    }

    return PAL_TRUE;
}