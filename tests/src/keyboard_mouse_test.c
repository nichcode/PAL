
#include "tests.h"
#include "pal/pal_input.h"

void keyboardAndMouseTest() {

    palLog("");
    palLog("===========================================");
    palLog("Keyboard And Mouse Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalInputDevice* keyboard = nullptr;
    PalInputDevice* mouse = nullptr;

    result = palInitInput(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // enumerate connected keyboards and mice 
    Int32 count = 0;
    if (palEnumerateInputDevices( 
        PAL_INPUT_MASK_KEYBOARD | PAL_INPUT_MASK_MOUSE,
        &count,
        nullptr
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    palLog("Input Device Count: %d", count);
    if (count == 0) {
        return;
    }

    PalInputDevice* inputDevices[count];
    if (palEnumerateInputDevices(
        PAL_INPUT_MASK_KEYBOARD | PAL_INPUT_MASK_MOUSE,
        &count,
        inputDevices
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // loop through the list to find the keyboard and mouse
    PalInputDeviceInfo inputDeviceInfo;
    for (int i = 0; i < count; i++) {
        PalInputDevice* device = inputDevices[i];

        result = palGetInputDeviceInfo(device, &inputDeviceInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        if (inputDeviceInfo.type == PAL_INPUT_DEVICE_KEYBOARD) {
            keyboard = device;
        } else {
            // since its just two device types, the other device type will be a mouse.
            mouse = device;
        }
    }

    if (!keyboard && !mouse) {
        // could not find keyboard and mouse
        return;
    }

    // register keyboard
    result = palRegisterInputDevice(keyboard);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // register mouse
    result = palRegisterInputDevice(mouse);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // get keyboard and mouse state
    PalMouseState mouseState;
    PalKeyboardState keyboardState;
    palGetKeyboardState(&keyboardState);
    palGetMouseState(&mouseState);

    bool running = true;
    bool unRegistered = false;

    while (running) {
        palUpdateInput();

        if (keyboardState.keys[PAL_KEY_ESCAPE]) {
            // terminate
            running = false;
        }

        if (!unRegistered) {
            if (keyboardState.keys[PAL_KEY_SPACE]) {
                palUnregisterInputDevice(mouse);
                if (result != PAL_RESULT_SUCCESS) {
                    const char* resultString = palResultToString(result);
                    palLog("PAL error - %s", resultString);
                    return;
                }
                unRegistered = true;
            }
        }

        if (unRegistered) {
            if (keyboardState.keys[PAL_KEY_ENTER]) {
                palRegisterInputDevice(mouse);
                if (result != PAL_RESULT_SUCCESS) {
                    const char* resultString = palResultToString(result);
                    palLog("PAL error - %s", resultString);
                    return;
                }
                unRegistered = false;
            }
        }

        // this will be updated only if a mouse device is registered
        // but since we are logging every frame, it will log the old position
        // FIXME: use events rather 
        palLog("Mouse Pos - (%d, %d)", mouseState.motion->x, mouseState.motion->y);
    }

}