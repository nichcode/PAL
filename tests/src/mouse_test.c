
#include "tests.h"
#include "pal/pal_input.h"

void mouseTest() {

    palLog("");
    palLog("===========================================");
    palLog("Mouse Test");
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
        PAL_INPUT_MASK_KEYBOARD,
        &count,
        mice
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // register the mouse device
    // we register the first mouse device in the array,
    // but you should query info and check which you want to register.
    result = palRegisterInputDevice(mice[0]);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // get mouse state. If the mouse device is not registered, this will not be filled.
    // The state will be updated when palUpdateInput is called, so call this once before the loop.
    PalMouseState state;
    palGetMouseState(&state);

    bool running = true;
    while (running) {
        palUpdateInput();

        // we can check mouse button here because there won't be anywhere to click.
        palLog("Mouse Global Pos - (%d, %d)", state.motion->x, state.motion->y);
    }

    palShutdownInput();
}