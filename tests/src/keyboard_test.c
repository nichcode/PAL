
#include "tests.h"
#include "pal/pal_input.h"


void keyboardTest() {

    palLog("");
    palLog("===========================================");
    palLog("Keyboard Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalInputDevice* keyboard = nullptr;

    result = palInitInput(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // enumerate connected keyboards 
    Int32 count = 0;
    if (palEnumerateInputDevices(
        PAL_INPUT_MASK_KEYBOARD,
        &count,
        nullptr
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    palLog("Keyboard Count: %d", count);
    if (count == 0) {
        return;
    }

    PalInputDevice* keyboards[count];
    if (palEnumerateInputDevices(
        PAL_INPUT_MASK_KEYBOARD,
        &count,
        keyboards
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    PalInputDeviceInfo inputDeviceInfo;
    for (int i = 0; i < count; i++) {
        PalInputDevice* device = keyboards[i];

        result = palGetInputDeviceInfo(device, &inputDeviceInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        // standard keyboard Product id and vendor id are mostly 0
        if (inputDeviceInfo.vendorID == 0) {
            palLog("Mouse Name: %s", inputDeviceInfo.name);
            palLog("Path: %s", inputDeviceInfo.path);
            palLog("Vender ID: %i", inputDeviceInfo.vendorID);
            palLog("Product ID: %i", inputDeviceInfo.productID);
            palLog("");

            keyboard = device;
            break;
        }
    }

    // register the keyboard device
    result = palRegisterInputDevice(keyboard);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // get keyboard state. If the keyboard device is not registered, the state struct will not be filled.
    // The state will be updated when palUpdateInput is called, so call this once before the loop.
    PalKeyboardState state;
    palGetKeyboardState(keyboard, &state);

    bool running = true;
    while (running) {
        palUpdateInput();

        if (state.scancodes[PAL_SCANCODE_RIGHT]) {
            palLog("Right Scancode (Right key on QWERTY layout) was pressed");
        }

        if (state.scancodes[PAL_SCANCODE_LEFT]) {
            palLog("Left Scancode (Left key on QWERTY layout) was pressed");
        }
        
        if (state.keys[PAL_KEY_ESCAPE]) {
            // terminate
            running = false;
        }
    }

    palShutdownInput();
}