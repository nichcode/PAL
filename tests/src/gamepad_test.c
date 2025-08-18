
#include "tests.h"
#include "pal/pal_input.h"

void gamepadTest() {

    palLog("");
    palLog("===========================================");
    palLog("Gamepad Test");
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

    // enumerate connected gamepads 
    Int32 count = 0;
    if (palEnumerateInputDevices(
        PAL_INPUT_MASK_GAMEPAD,
        &count,
        nullptr
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    palLog("Gamepad Count: %d", count);
    if (count == 0) {
        return;
    }

    PalInputDevice* gamepads[count];
    if (palEnumerateInputDevices(
        PAL_INPUT_MASK_GAMEPAD,
        &count,
        gamepads
    ) != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // register the game device
    result = palRegisterInputDevice(gamepads[0]);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return;
    }

    // get gamepad state. If the keyboard device is not registered, the state struct will not be filled.
    // The state will be updated when palUpdateInput is called, so call this once before the loop.
    PalGamepadState state;

    bool running = true;
    while (running) {
        palUpdateInput();
        
        // if (state.buttons[PAL_KEY_ESCAPE]) {
        //     // terminate
        //     running = false;
        // }
    }

    palShutdownInput();
}