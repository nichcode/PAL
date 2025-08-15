
#include "tests.h"
#include "pal/pal_input.h"

static const char* inputDeviceTypeToString(PalInputDeviceType type) {

    switch (type) {
        case PAL_INPUT_DEVICE_KEYBOARD: 
        return "Keyboard";

        case PAL_INPUT_DEVICE_MOUSE: 
        return "Mouse";

        case PAL_INPUT_DEVICE_DUALSHOCK4: 
        return "Dualshock 4(PS4)";

        case PAL_INPUT_DEVICE_DUALSENSE: 
        return "Dual Sense";

        case PAL_INPUT_DEVICE_XBOX_360: 
        return "Xbox 360";

        case PAL_INPUT_DEVICE_XBOX_ONE: 
        return "Xbox One";

        case PAL_INPUT_DEVICE_SWITCH_PRO:
        return "Switch Pro";

        case PAL_INPUT_DEVICE_GENERIC_GAMEPAD:
        return "Generic Gamepad";
    }
    return nullptr;
}

void inputTest() {

    palLog("");
    palLog("===========================================");
    palLog("Input Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    result = palInitInput(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // enumerate input devices
    Int32 keyboardCount = 0;
    Int32 mouseCount = 0;
    Int32 gamepadCount = 0;
    Int32 totalInputDevices = 0;

    // enumerate connected keyboards
    if (palEnumerateInputDevices( 
        PAL_INPUT_MASK_KEYBOARD,
        &keyboardCount,
        nullptr
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // enumerate connected mice
    if (palEnumerateInputDevices( 
        PAL_INPUT_MASK_MOUSE,
        &mouseCount,
        nullptr
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // enumerate connected gamepads
    if (palEnumerateInputDevices( 
        PAL_INPUT_MASK_GAMEPAD,
        &gamepadCount,
        nullptr
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    palLog("Keyboard Count: %d", keyboardCount);
    palLog("Mouse Count: %d", mouseCount);
    palLog("Gamepad Count: %d", gamepadCount);

    totalInputDevices = keyboardCount + mouseCount + gamepadCount;
    palLog("Total Input Devices Count: %d", totalInputDevices);
    palLog("");

    // enumerate all connected input devices
    PalInputDevice* inputDevices[totalInputDevices];
    if (palEnumerateInputDevices(
        PAL_INPUT_MASK_ALL,
        &totalInputDevices,
        inputDevices
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // Get input device info
    PalInputDeviceInfo inputDeviceInfo;
    for (int i = 0; i < totalInputDevices; i++) {
        PalInputDevice* device = inputDevices[i];

        result = palGetInputDeviceInfo(device, &inputDeviceInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        // log device info
        palLog("Input Device Name: %s", inputDeviceInfo.name);
        palLog(" Path: %s", inputDeviceInfo.path);
        palLog(" Type: %s", inputDeviceTypeToString(inputDeviceInfo.type));
        palLog(" Vender ID: %i", inputDeviceInfo.vendorID);
        palLog(" Product ID: %i", inputDeviceInfo.productID);
        palLog("");

    }

    palShutdownInput();
}