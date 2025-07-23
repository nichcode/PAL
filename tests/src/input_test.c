
#include "tests.h"
#include "pal/pal_input.h"

const char* inputDeviceTypeToString(PalInputDeviceType type) {

    switch (type) {
        case PAL_INPUT_DEVICE_GAMEPAD: 
        return "Gamepad";

        case PAL_INPUT_DEVICE_KEYBOARD: 
        return "Keyboard";

        case PAL_INPUT_DEVICE_MOUSE: 
        return "Mouse";

        case PAL_INPUT_DEVICE_HID: 
        return "HID";

        case PAL_INPUT_DEVICE_UNKNOWN: 
        return "Unknown";
    }
    return PAL_NULL;
}

void inputTest() {

    palLogInfo(PAL_NULL, "");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "Input Test");
    palLogInfo(PAL_NULL, "===========================================");
    palLogInfo(PAL_NULL, "");

    PalResult result;
    PalInput input;
    Uint32 inputDeviceCount = 0;
    PalInputDeviceInfo inputDeviceInfo;

    PalInputConfig inputConfig;
    inputConfig.allocator = PAL_NULL;
    inputConfig.eventDriver = PAL_NULL;

    result = palCreateInput(&inputConfig, &input);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    // enumerate input devices
    result = palEnumerateInputDevices(input, &inputDeviceCount, PAL_NULL);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    palLogInfo(PAL_NULL, "Input Device Count: %i", inputDeviceCount);

    PalInputDevice inputDevices[inputDeviceCount];
    result = palEnumerateInputDevices(input, &inputDeviceCount, inputDevices);
    if (result != PAL_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLogError(PAL_NULL, resultString);
        return;
    }

    // get input device info
    for (int i = 0; i < inputDeviceCount; i++) {
        PalInputDevice device = inputDevices[i];

        result = palGetInputDeviceInfo(device, &inputDeviceInfo);
        if (result != PAL_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLogError(PAL_NULL, resultString);
            return;
        }

        // log device info
        palLogInfo(PAL_NULL, "Input Device Name: %s", inputDeviceInfo.name);
        palLogInfo(PAL_NULL, " Type: %s", inputDeviceTypeToString(inputDeviceInfo.type));
        palLogInfo(PAL_NULL, " Vender ID: %i", inputDeviceInfo.vendorID);
        palLogInfo(PAL_NULL, " Product ID: %i", inputDeviceInfo.productID);
    }

    palDestroyInput(input);
}