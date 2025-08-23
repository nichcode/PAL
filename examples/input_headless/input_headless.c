
#include "key_string.h"

static const char* gamepadTypeToString(PalInputDeviceType type);

int main(int argc, char**) {

    PalResult result;
    PalEventDriver* eventDriver = nullptr;
    PalInputDeviceInfo deviceInfo;
    PalInputDevice** inputDevices = nullptr;

    PalInputDevice* keyboard = nullptr;
    PalInputDevice* mouse = nullptr;
    PalInputDevice* gamepad = nullptr;

    Int32 count = 0;
    PalInputMask mask = PAL_INPUT_MASK_ALL; // use for device enumeration
    bool running = false;

    // event driver create info
    PalEventDriverCreateInfo eventDriverCreateInfo;
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = nullptr; // callback to recieve callback events (PAL_DISPATCH_CALLBACK)
    eventDriverCreateInfo.queue = nullptr; // optional user event queue to recieve queue eevents (PAL_DISPATCH_POLL)
    eventDriverCreateInfo.userData = nullptr; // for event callback

    // we want events, so we create an event driver to recieve events
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // initialize the input system
    result = palInitInput(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // enumerate all connected input devices
    result = palEnumerateInputDevices(mask, &count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    palLog("Input Device Count: %d", count);
    inputDevices = palAllocate(nullptr, sizeof(PalInputDevice*) * count, 0);
    if (!inputDevices) {
        palLog("PAL Error - %s", palResultToString(PAL_RESULT_OUT_OF_MEMORY));
        return -1;
    }

    result = palEnumerateInputDevices(mask, &count, inputDevices);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // get device type and set keyboard and mouse pointers from the input devices array
    for (Int32 i = 0; i < count; i++) {
        PalInputDevice* device = inputDevices[i];
        result = palGetInputDeviceInfo(device, &deviceInfo);
        if (result != PAL_RESULT_SUCCESS) {
            goto Failed;
        }

        if (deviceInfo.type == PAL_INPUT_DEVICE_KEYBOARD) {
            // any of the keyboard will do for this example
            // you can be specific by check the name or the vendor and produc ID
            // we will pick the first one in the array and log its information
            keyboard = device;

            palLog("");
            palLog("Selected Keyboard");
            palLog(" Name: %s", deviceInfo.name);
            palLog(" Path: %s", deviceInfo.path);
            palLog(" Vender ID: %i", deviceInfo.vendorID);
            palLog(" Product ID: %i", deviceInfo.productID);

        } else if (deviceInfo.type == PAL_INPUT_DEVICE_MOUSE) {
            // any of the mice will do for this example
            // you can be specific by check the name or the vendor and produc ID
            // we will pick the first one in the array and log its information
            mouse = device;

            palLog("");
            palLog("Selected Mouse");
            palLog(" Name: %s", deviceInfo.name);
            palLog(" Path: %s", deviceInfo.path);
            palLog(" Vender ID: %i", deviceInfo.vendorID);
            palLog(" Product ID: %i", deviceInfo.productID);

        } else {
            // you can also be specific with the gamepad also
            // PAL_INPUT_DEVICE_DUALSHOCK4 for PS4 controller
            // PAL_INPUT_DEVICE_XBOX_360 for Xbox60 controller

            // you can also check by the name or the vendor and produc ID
            // but we will pick the first one and log its informatation
            gamepad = gamepad;

            palLog("");
            palLog("Selected Gamepad");
            palLog(" Name: %s", deviceInfo.name);
            palLog(" Type: %s", gamepadTypeToString(deviceInfo.type)); // for specific gamepad
            palLog(" Path: %s", deviceInfo.path);
            palLog(" Vender ID: %i", deviceInfo.vendorID);
            palLog(" Product ID: %i", deviceInfo.productID);
        }
    }

    // we check if we got a connected keyboard and register it for input
    if (keyboard) {
        palRegisterInputDevice(keyboard);
        if (result != PAL_RESULT_SUCCESS) {
            goto Failed;
        }

        palLog("Keyboard device registered");
    }

    // we check if we got a connected mouse and register it for input
    if (mouse) {
        palRegisterInputDevice(mouse);
        if (result != PAL_RESULT_SUCCESS) {
            goto Failed;
        }

        palLog("Mouse device registered");
    }

    // we check if we got a connected gamepad and register it for input
    if (gamepad) {
        palRegisterInputDevice(gamepad);
        if (result != PAL_RESULT_SUCCESS) {
            goto Failed;
        }

        palLog("Gamepad device registered");
    }

    // no need to get the state, since this example we will use events rather
    // we set all input events to poll dispatch mode but you can choose any one you want
    palSetEventDispatchMode(eventDriver, PAL_EVENT_KEYDOWN, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_KEYUP, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_MOUSE_BUTTONDOWN, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_MOUSE_BUTTONUP, PAL_DISPATCH_POLL);

    running = true;
    while (running) {
        palUpdateInput();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_KEYDOWN: {
                    Uint32 key, scancode;
                    bool repeat = event.data2;
                    palUnpackUint32(event.data, &key, &scancode);

                    const char* keyString = s_KeyNames[key];
                    const char* scancodeString = s_ScancodeNames[scancode];

                    // you can get the device id
                    // palGetInputDeviceID() and check with event.sourceID to know which keyboard was pressed
                    if (key == PAL_KEY_ESCAPE) {
                        // the device must be registered
                        // Uint64 keyboardID = palGetInputDeviceID(keyboard);
                        // if (keyboardID == event.sourceID) {
                        //     running = false;
                        // }
                        running = false;
                        break;
                    }

                    if (repeat) {
                        palLog("Key repeat - (key - %s, scancode - %s)", keyString, scancodeString);

                    } else {
                        palLog("Keydown - (key - %s, scancode - %s)", keyString, scancodeString);
                    }
                    break;
                }

                case PAL_EVENT_KEYUP: {
                    Uint32 key, scancode;
                    bool repeat = event.data2;
                    palUnpackUint32(event.data, &key, &scancode);

                    const char* keyString = s_KeyNames[key];
                    const char* scancodeString = s_ScancodeNames[scancode];
                    palLog("Keyup - (key - %s, scancode - %s)", keyString, scancodeString);
                    break;
                }

                case PAL_EVENT_MOUSE_BUTTONDOWN: {
                    const char* mouseButtonString = s_MouseButtonNames[event.data];
                    palLog("mouse button down - button - %s", mouseButtonString);
                    break;
                }

                case PAL_EVENT_MOUSE_BUTTONUP: {
                    const char* mouseButtonString = s_MouseButtonNames[event.data];
                    palLog("mouse button up - button - %s", mouseButtonString);
                    break;
                }

                case PAL_EVENT_MOUSE_WHEEL: {
                    Int32 scrollX, scrollY;
                    palUnpackInt32(event.data, &scrollX, &scrollY);
                    palLog("mouse scroll - (%s, %s)", scrollX, scrollY);
                    break;
                }
            }
        }

    }

    // cleanup
    palShutdownInput();
    palDestroyEventDriver(eventDriver);
    palFree(nullptr, inputDevices);

    return 0;

    // a goto for failure. No need to duplicate that result check
    Failed:
        if (inputDevices) {
            palFree(nullptr, inputDevices);
        }

        const char* resultString = palResultToString(result);
        palLog("PAL Error - %s", resultString);
        return -1;
}

static const char* gamepadTypeToString(PalInputDeviceType type) {

    switch (type) {
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