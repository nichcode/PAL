
#include "tests.h"
#include "pal/pal_input.h"

void mouseTest() {

    palLog("");
    palLog("===========================================");
    palLog("Mouse Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalInputDevice* mouse = nullptr;

    result = palInitInput(nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // enumerate connected mice 
    Int32 count = 0;
    if (palEnumerateInputDevices( 
        PAL_INPUT_MASK_MOUSE,
        &count,
        nullptr
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    palLog("Mouse Count: %d", count);
    if (count == 0) {
        return;
    }

    PalInputDevice* mice[count];
    if (palEnumerateInputDevices( 
        PAL_INPUT_MASK_MOUSE,
        &count,
        mice
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    PalInputDeviceInfo inputDeviceInfo;
    for (int i = 0; i < count; i++) {
        PalInputDevice* device = mice[i];

        result = palGetInputDeviceInfo(device, &inputDeviceInfo);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog(resultString);
            return;
        }

        // standard mouse Product id and vendor id are mostly 0
        // you can also use the name to check
        if (inputDeviceInfo.vendorID == 0) {
            palLog("Mouse Name: %s", inputDeviceInfo.name);
            palLog("Path: %s", inputDeviceInfo.path);
            palLog("Vender ID: %i", inputDeviceInfo.vendorID);
            palLog("Product ID: %i", inputDeviceInfo.productID);
            palLog("");
            
            mouse = device;
            break;
        }
    }

    // register the mouse device
    result = palRegisterInputDevice(mouse);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // get mouse state. If the mouse device is not registered, this will not be filled.
    // The state will be updated when palUpdateInput is called, so call this once before the loop.
    PalMouseState state;
    palGetMouseState(mouse, &state);

    bool running = true;
    while (running) {
        palUpdateInput();

        if (state.buttons[PAL_MOUSE_BUTTON_LEFT]) {
            palLog("Left mouse button pressed");
        }

        if (state.buttons[PAL_MOUSE_BUTTON_RIGHT]) {
            palLog("right mouse button pressed");
        }

        if (state.buttons[PAL_MOUSE_BUTTON_MIDDLE]) {
            palLog("middle mouse button pressed");
        }

        // get the mouse global position and relative movement
        int x, y;
        palGetMousePosition(&x, &y);
        //palLog("Mouse Pos - (%d, %d)", x, y);
    }

    palShutdownInput();
}