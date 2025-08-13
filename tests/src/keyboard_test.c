
#include "tests.h"
#include "pal/pal_input.h"


void keyboardTest() {

    palLog("");
    palLog("===========================================");
    palLog("Keyboard Test");
    palLog("===========================================");
    palLog("");

    PalResult result;
    PalInputSystem* input = nullptr;

    PalInputSystemCreateInfo createInfo;
    createInfo.allocator = nullptr; // for default.
    createInfo.eventDriver = nullptr; // for default.
    result = palCreateInputSystem(&createInfo, &input);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // enumerate connected keyboards 
    Int32 count = 0;
    if (palEnumerateInputDevices(
        input, 
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
        input, 
        PAL_INPUT_MASK_KEYBOARD,
        &count,
        keyboards
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // register the keyboard device
    // we register the first keyboard device
    // you can check the product and vendor to know if its standard or HID
    result = palRegisterInputDevice(input, keyboards[0]);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // get keyboard state
    // the pointers the PalKeyboardState fields keyState and scancodeState will be update with palUpdateInput.
    // So this function should be called once after keyboard device registration for efficiency.
    PalKeyboardState state;
    palGetKeyboardState(input, &state);

    bool running = true;
    while (running) {
        palUpdateInput(input);

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

    palDestroyInputSystem(input);
}