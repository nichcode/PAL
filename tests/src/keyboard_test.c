
#include "tests.h"
#include "pal/pal_input.h"


void keyboardTest() {

    palLog("");
    palLog("===========================================");
    palLog("Keyboard Test");
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

    // register the keyboard device
    // we register the first mouse device in the array,
    // but you should query info and check which you want to register.
    result = palRegisterInputDevice(keyboards[0]);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // get keyboard state. If the keyboard device is not registered, this will not be filled.
    // The state will be updated when palUpdateInput is called, so call this once before the loop.
    PalKeyboardState state;
    palGetKeyboardState(&state);

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