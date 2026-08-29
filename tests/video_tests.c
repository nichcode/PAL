
#include "tests.h"
#include "pal2/pal_video.h"

#if PAL_HAS_VIDEO_MODULE

static const char* s_KeyNames[PAL_KEYCODE_COUNT] = {
    [PAL_KEYCODE_UNKNOWN] = "Unknown",
    // Letters
    [PAL_KEYCODE_A] = "A",
    [PAL_KEYCODE_B] = "B",
    [PAL_KEYCODE_C] = "C",
    [PAL_KEYCODE_D] = "D",
    [PAL_KEYCODE_E] = "E",
    [PAL_KEYCODE_F] = "F",
    [PAL_KEYCODE_G] = "G",
    [PAL_KEYCODE_H] = "H",
    [PAL_KEYCODE_I] = "I",
    [PAL_KEYCODE_J] = "J",
    [PAL_KEYCODE_K] = "K",
    [PAL_KEYCODE_L] = "L",
    [PAL_KEYCODE_M] = "M",
    [PAL_KEYCODE_N] = "N",
    [PAL_KEYCODE_O] = "O",
    [PAL_KEYCODE_P] = "P",
    [PAL_KEYCODE_Q] = "Q",
    [PAL_KEYCODE_R] = "R",
    [PAL_KEYCODE_S] = "S",
    [PAL_KEYCODE_T] = "T",
    [PAL_KEYCODE_U] = "U",
    [PAL_KEYCODE_V] = "V",
    [PAL_KEYCODE_W] = "W",
    [PAL_KEYCODE_X] = "X",
    [PAL_KEYCODE_Y] = "Y",
    [PAL_KEYCODE_Z] = "Z",

    // Numbers (top row)
    [PAL_KEYCODE_0] = "0",
    [PAL_KEYCODE_1] = "1",
    [PAL_KEYCODE_2] = "2",
    [PAL_KEYCODE_3] = "3",
    [PAL_KEYCODE_4] = "4",
    [PAL_KEYCODE_5] = "5",
    [PAL_KEYCODE_6] = "6",
    [PAL_KEYCODE_7] = "7",
    [PAL_KEYCODE_8] = "8",
    [PAL_KEYCODE_9] = "9",

    // Function keys
    [PAL_KEYCODE_F1] = "F1",
    [PAL_KEYCODE_F2] = "F2",
    [PAL_KEYCODE_F3] = "F3",
    [PAL_KEYCODE_F4] = "F4",
    [PAL_KEYCODE_F5] = "F5",
    [PAL_KEYCODE_F6] = "F6",
    [PAL_KEYCODE_F7] = "F7",
    [PAL_KEYCODE_F8] = "F8",
    [PAL_KEYCODE_F9] = "F9",
    [PAL_KEYCODE_F10] = "F10",
    [PAL_KEYCODE_F11] = "F11",
    [PAL_KEYCODE_F12] = "F12",

    // Control
    [PAL_KEYCODE_ESCAPE] = "Escape",
    [PAL_KEYCODE_ENTER] = "Enter",
    [PAL_KEYCODE_TAB] = "Tab",
    [PAL_KEYCODE_BACKSPACE] = "Backspace",
    [PAL_KEYCODE_SPACE] = "Space",
    [PAL_KEYCODE_CAPSLOCK] = "Capslock",
    [PAL_KEYCODE_NUMLOCK] = "Numlock",
    [PAL_KEYCODE_SCROLLLOCK] = "Scrolllock",
    [PAL_KEYCODE_LSHIFT] = "LeftShift",
    [PAL_KEYCODE_RSHIFT] = "RightShift",
    [PAL_KEYCODE_LCTRL] = "LeftControl",
    [PAL_KEYCODE_RCTRL] = "RightControl",
    [PAL_KEYCODE_LALT] = "LeftAlt",
    [PAL_KEYCODE_RALT] = "RightAlt",

    // Arrows
    [PAL_KEYCODE_LEFT] = "Left",
    [PAL_KEYCODE_RIGHT] = "Right",
    [PAL_KEYCODE_UP] = "Up",
    [PAL_KEYCODE_DOWN] = "Down",

    // Navigation

    [PAL_KEYCODE_INSERT] = "Insert",
    [PAL_KEYCODE_DELETE] = "Delete",
    [PAL_KEYCODE_HOME] = "Home",
    [PAL_KEYCODE_END] = "End",
    [PAL_KEYCODE_PAGEUP] = "Pageup",
    [PAL_KEYCODE_PAGEDOWN] = "Pagedown",

    // Keypad
    [PAL_KEYCODE_KP_0] = "Keypad 0",
    [PAL_KEYCODE_KP_1] = "Keypad 1",
    [PAL_KEYCODE_KP_2] = "Keypad 2",
    [PAL_KEYCODE_KP_3] = "Keypad 3",
    [PAL_KEYCODE_KP_4] = "Keypad 4",
    [PAL_KEYCODE_KP_5] = "Keypad 5",
    [PAL_KEYCODE_KP_6] = "Keypad 6",
    [PAL_KEYCODE_KP_7] = "Keypad 7",
    [PAL_KEYCODE_KP_8] = "Keypad 8",
    [PAL_KEYCODE_KP_9] = "Keypad 9",
    [PAL_KEYCODE_KP_ENTER] = "Keypad Enter",
    [PAL_KEYCODE_KP_ADD] = "Keypad Add",
    [PAL_KEYCODE_KP_SUBTRACT] = "Keypad Subtract",
    [PAL_KEYCODE_KP_MULTIPLY] = "Keypad Multiply",
    [PAL_KEYCODE_KP_DIVIDE] = "Keypad Divide",
    [PAL_KEYCODE_KP_DECIMAL] = "Keypad Decimal",
    [PAL_KEYCODE_KP_EQUAL] = "Keypad Equal",

    // Misc
    [PAL_KEYCODE_PRINTSCREEN] = "Printscreen",
    [PAL_KEYCODE_PAUSE] = "Pause",
    [PAL_KEYCODE_MENU] = "Menu",
    [PAL_KEYCODE_APOSTROPHE] = "Apostrophe",
    [PAL_KEYCODE_BACKSLASH] = "Backslash",
    [PAL_KEYCODE_COMMA] = "Comma",
    [PAL_KEYCODE_EQUAL] = "Equal",
    [PAL_KEYCODE_GRAVEACCENT] = "Graveaccent",
    [PAL_KEYCODE_SUBTRACT] = "Subtract",
    [PAL_KEYCODE_PERIOD] = "Period",
    [PAL_KEYCODE_SEMICOLON] = "Semicolon",
    [PAL_KEYCODE_SLASH] = "Slash",
    [PAL_KEYCODE_LBRACKET] = "LeftBracket",
    [PAL_KEYCODE_RBRACKET] = "RightBracket",
    [PAL_KEYCODE_LSUPER] = "LeftSuper",
    [PAL_KEYCODE_RSUPER] = "RightSuper",
};

static const char* s_ScancodeNames[PAL_SCANCODE_COUNT] = {
    [PAL_SCANCODE_UNKNOWN] = "Unknown",
    // Letters
    [PAL_SCANCODE_A] = "A",
    [PAL_SCANCODE_B] = "B",
    [PAL_SCANCODE_C] = "C",
    [PAL_SCANCODE_D] = "D",
    [PAL_SCANCODE_E] = "E",
    [PAL_SCANCODE_F] = "F",
    [PAL_SCANCODE_G] = "G",
    [PAL_SCANCODE_H] = "H",
    [PAL_SCANCODE_I] = "I",
    [PAL_SCANCODE_J] = "J",
    [PAL_SCANCODE_K] = "K",
    [PAL_SCANCODE_L] = "L",
    [PAL_SCANCODE_M] = "M",
    [PAL_SCANCODE_N] = "N",
    [PAL_SCANCODE_O] = "O",
    [PAL_SCANCODE_P] = "P",
    [PAL_SCANCODE_Q] = "Q",
    [PAL_SCANCODE_R] = "R",
    [PAL_SCANCODE_S] = "S",
    [PAL_SCANCODE_T] = "T",
    [PAL_SCANCODE_U] = "U",
    [PAL_SCANCODE_V] = "V",
    [PAL_SCANCODE_W] = "W",
    [PAL_SCANCODE_X] = "X",
    [PAL_SCANCODE_Y] = "Y",
    [PAL_SCANCODE_Z] = "Z",

    // Numbers (top row)
    [PAL_SCANCODE_0] = "0",
    [PAL_SCANCODE_1] = "1",
    [PAL_SCANCODE_2] = "2",
    [PAL_SCANCODE_3] = "3",
    [PAL_SCANCODE_4] = "4",
    [PAL_SCANCODE_5] = "5",
    [PAL_SCANCODE_6] = "6",
    [PAL_SCANCODE_7] = "7",
    [PAL_SCANCODE_8] = "8",
    [PAL_SCANCODE_9] = "9",

    // Function keys
    [PAL_SCANCODE_F1] = "F1",
    [PAL_SCANCODE_F2] = "F2",
    [PAL_SCANCODE_F3] = "F3",
    [PAL_SCANCODE_F4] = "F4",
    [PAL_SCANCODE_F5] = "F5",
    [PAL_SCANCODE_F6] = "F6",
    [PAL_SCANCODE_F7] = "F7",
    [PAL_SCANCODE_F8] = "F8",
    [PAL_SCANCODE_F9] = "F9",
    [PAL_SCANCODE_F10] = "F10",
    [PAL_SCANCODE_F11] = "F11",
    [PAL_SCANCODE_F12] = "F12",

    // Control
    [PAL_SCANCODE_ESCAPE] = "Escape",
    [PAL_SCANCODE_ENTER] = "Enter",
    [PAL_SCANCODE_TAB] = "Tab",
    [PAL_SCANCODE_BACKSPACE] = "Backspace",
    [PAL_SCANCODE_SPACE] = "Space",
    [PAL_SCANCODE_CAPSLOCK] = "Capslock",
    [PAL_SCANCODE_NUMLOCK] = "Numlock",
    [PAL_SCANCODE_SCROLLLOCK] = "Scrolllock",
    [PAL_SCANCODE_LSHIFT] = "LeftShift",
    [PAL_SCANCODE_RSHIFT] = "RightShift",
    [PAL_SCANCODE_LCTRL] = "LeftControl",
    [PAL_SCANCODE_RCTRL] = "RightControl",
    [PAL_SCANCODE_LALT] = "LeftAlt",
    [PAL_SCANCODE_RALT] = "RightAlt",

    // Arrows
    [PAL_SCANCODE_LEFT] = "Left",
    [PAL_SCANCODE_RIGHT] = "Right",
    [PAL_SCANCODE_UP] = "Up",
    [PAL_SCANCODE_DOWN] = "Down",

    // Navigation
    [PAL_SCANCODE_INSERT] = "Insert",
    [PAL_SCANCODE_DELETE] = "Delete",
    [PAL_SCANCODE_HOME] = "Home",
    [PAL_SCANCODE_END] = "End",
    [PAL_SCANCODE_PAGEUP] = "Pageup",
    [PAL_SCANCODE_PAGEDOWN] = "Pagedown",

    // Keypad
    [PAL_SCANCODE_KP_0] = "Keypad 0",
    [PAL_SCANCODE_KP_1] = "Keypad 1",
    [PAL_SCANCODE_KP_2] = "Keypad 2",
    [PAL_SCANCODE_KP_3] = "Keypad 3",
    [PAL_SCANCODE_KP_4] = "Keypad 4",
    [PAL_SCANCODE_KP_5] = "Keypad 5",
    [PAL_SCANCODE_KP_6] = "Keypad 6",
    [PAL_SCANCODE_KP_7] = "Keypad 7",
    [PAL_SCANCODE_KP_8] = "Keypad 8",
    [PAL_SCANCODE_KP_9] = "Keypad 9",
    [PAL_SCANCODE_KP_ENTER] = "Keypad Enter",
    [PAL_SCANCODE_KP_ADD] = "Keypad Add",
    [PAL_SCANCODE_KP_SUBTRACT] = "Keypad Subtract",
    [PAL_SCANCODE_KP_MULTIPLY] = "Keypad Multiply",
    [PAL_SCANCODE_KP_DIVIDE] = "Keypad Divide",
    [PAL_SCANCODE_KP_DECIMAL] = "Keypad Decimal",
    [PAL_SCANCODE_KP_EQUAL] = "Keypad Equal",

    // Misc
    [PAL_SCANCODE_PRINTSCREEN] = "Printscreen",
    [PAL_SCANCODE_PAUSE] = "Pause",
    [PAL_SCANCODE_MENU] = "Menu",
    [PAL_SCANCODE_APOSTROPHE] = "Apostrophe",
    [PAL_SCANCODE_BACKSLASH] = "Backslash",
    [PAL_SCANCODE_COMMA] = "Comma",
    [PAL_SCANCODE_EQUAL] = "Equal",
    [PAL_SCANCODE_GRAVEACCENT] = "Graveaccent",
    [PAL_SCANCODE_SUBTRACT] = "Subtract",
    [PAL_SCANCODE_PERIOD] = "Period",
    [PAL_SCANCODE_SEMICOLON] = "Semicolon",
    [PAL_SCANCODE_SLASH] = "Slash",
    [PAL_SCANCODE_LBRACKET] = "LeftBracket",
    [PAL_SCANCODE_RBRACKET] = "RightBracket",
    [PAL_SCANCODE_LSUPER] = "LeftSuper",
    [PAL_SCANCODE_RSUPER] = "RightSuper"
};

static const char* s_MouseButtonNames[PAL_MOUSE_BUTTON_COUNT] = {
    [PAL_MOUSE_BUTTON_UNKNOWN] = "Unknown",
    [PAL_MOUSE_BUTTON_LEFT] = "Left",
    [PAL_MOUSE_BUTTON_RIGHT] = "Right",
    [PAL_MOUSE_BUTTON_MIDDLE] = "Middle",
    [PAL_MOUSE_BUTTON_X1] = "X1",
    [PAL_MOUSE_BUTTON_X2] = "X2"
};

static const char* orientationToString(PalOrientation orientation)
{
    switch (orientation) 
    {
        case PAL_ORIENTATION_LANDSCAPE:
            return "Landscape";

        case PAL_ORIENTATION_PORTRAIT:
            return "Portrait";

        case PAL_ORIENTATION_LANDSCAPE_FLIPPED:
            return "Landscape Flipped";

        case PAL_ORIENTATION_PORTRAIT_FLIPPED:
            return "Portrait Flipped";
    }
    return nullptr;
}

PalBool monitorTest()
{
    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    PalVideoFeatures features = palGetVideoFeatures();

    uint32_t count = 0;
    result = palEnumerateMonitors(&count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    if (count == 0) {
        return PAL_FALSE;
    }
    palLog(nullptr, "Monitor Count: %lu", count);

    PalMonitor** monitors = nullptr;
    monitors = palAllocate(nullptr, sizeof(PalMonitor*) * count, 0);
    if (!monitors) {
        return PAL_FALSE;
    }

    result = palEnumerateMonitors(&count, monitors);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    PalMonitorInfo info = {0};
    for (int32_t i = 0; i < count; i++) {
        PalMonitor* monitor = monitors[i];
        palGetMonitorInfo(monitor, &info);

        // log monitor info
        palLog(nullptr, "Monitor Name: %s", info.name);
        palLog(nullptr, " Size: (%d, %d)", info.width, info.height);
        palLog(nullptr, " DPI: %d", info.dpi);
        palLog(nullptr, " RefreshRate: %d", info.refreshRate);

        // check if the monitor is the primary monitor
        const char* boolToString;
        if (info.primary) {
            boolToString = "True";
        } else {
            boolToString = "False";
        }

        palLog(nullptr, " Primary: %s", boolToString);
        palLog(nullptr, " Orientation: %s", orientationToString(info.orientation));

        uint32_t modeCount = 0;
        palEnumerateMonitorModes(monitor, &modeCount, nullptr);
        palLog(nullptr, "");
        palLog(nullptr, " Mode Count: %d", modeCount);

        PalMonitorMode* modes = nullptr;
        modes = palAllocate(nullptr, sizeof(PalMonitorMode) * modeCount, 0);
        if (!modes) {
            palLog(nullptr, "Failed to allocate memory");
            palFree(nullptr, monitors);
            return PAL_FALSE;
        }

        palEnumerateMonitorModes(monitor, &modeCount, modes);
        for (int32_t i = 0; i < modeCount; i++) {
            // log monitor mode
            PalMonitorMode* mode = &modes[i];
            palLog(nullptr, "  Mode Index: %d", i);
            palLog(nullptr, "   Size: (%d, %d)", mode->width, mode->height);
            palLog(nullptr, "   RefreshRate: %d", mode->refreshRate);
            palLog(nullptr, "   Bits Per Pixel: %d", mode->bpp);
            palLog(nullptr, "");
        }

        palFree(nullptr, modes);

        if (features & PAL_VIDEO_FEATURE_MONITOR_GET_MODE) {
            PalMonitorMode current;
            palGetCurrentMonitorMode(monitor, &current);
            palLog(nullptr, "");
            palLog(nullptr, "  Current Mode:");
            palLog(nullptr, "   Size: (%d, %d)", current.width, current.height);
            palLog(nullptr, "   RefreshRate: %d", current.refreshRate);
            palLog(nullptr, "   Bits Per Pixel: %d", current.bpp);
        }
    }

    palShutdownVideo();
    palFree(nullptr, monitors);
    return PAL_TRUE;
}

PalBool simpleWindowTest()
{
    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};

    PalResult result = palInitVideo(nullptr, nullptr, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    createInfo.width = 640;
    createInfo.height = 480;
    createInfo.title = "Simple Window";
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.show = PAL_TRUE;
    createInfo.state = PAL_WINDOW_STATE_NORMAL;

    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    palLog(nullptr, "Window Created");
    palDestroyWindow(window);
    palLog(nullptr, "Window Destroyed");
    palShutdownVideo();

    return PAL_TRUE;
}

PalBool eventWindowTest()
{
    PalEventDriver* eventDriver = nullptr;
    PalWindow* window = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};
    PalWindowCreateInfo windowCreateInfo = {0};

    PalResult result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    for (uint32_t e = 0; e < PAL_EVENT_TYPE_COUNT; e++) {
        palSetEventDispatchMode(eventDriver, e, PAL_DISPATCH_MODE_POLL);
    }

    result = palInitVideo(nullptr, eventDriver, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.title = "Event Window";
    windowCreateInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    windowCreateInfo.show = PAL_TRUE;
    windowCreateInfo.state = PAL_WINDOW_STATE_NORMAL;

    // check if we support decorated windows
    PalVideoFeatures features = palGetVideoFeatures();
    if (!(features & PAL_VIDEO_FEATURE_DECORATED_WINDOW)) {
        windowCreateInfo.style |= PAL_WINDOW_STYLE_BORDERLESS;
    }

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    PalBool running = PAL_TRUE;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_TYPE_WINDOW_CLOSE: {
                    running = PAL_FALSE;
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_SIZE: {
                    uint32_t width, height;
                    palUnpackUint32(event.data, &width, &height);
                    PalWindow* window = palUnpackPointer(event.data2);
                    palLog(nullptr, "Window Resized: (%d, %d)", width, height);
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_MOVE: {
                    int32_t x, y;
                    palUnpackInt32(event.data, &x, &y);
                    PalWindow* window = palUnpackPointer(event.data2);
                    palLog(nullptr, "Window Moved: (%d, %d)", x, y);
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_VISIBILITY: {
                    PalWindow* window = palUnpackPointer(event.data2);
                    if (event.data) {
                        palLog(nullptr, "Window is shown");

                    } else {
                        palLog(nullptr, "Window is hidden");
                    }
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_STATE: {
                    PalWindow* window = palUnpackPointer(event.data2);
                    if (event.data == PAL_WINDOW_STATE_MAXIMIZED) {
                        palLog(nullptr, "Window maximized");

                    } else if (event.data == PAL_WINDOW_STATE_MINIMIZED) {
                        palLog(nullptr, "Window minimized");

                    } else {
                        palLog(nullptr, "Window restored");
                    }
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_FOCUS: {
                    PalWindow* window = palUnpackPointer(event.data2);
                    if (event.data) {
                        palLog(nullptr, "Window has gained focus");

                    } else {
                        palLog(nullptr, "Window has lost focus");
                    }
                    break;
                }

                case PAL_EVENT_TYPE_MONITOR_DPI_CHANGED: {
                    PalWindow* window = palUnpackPointer(event.data2);
                    palLog(nullptr, "Monitor DPI: %d", event.data);
                    break;
                }

                case PAL_EVENT_TYPE_MONITOR_LIST_CHANGED: {
                    PalWindow* window = palUnpackPointer(event.data2);
                    palLog(nullptr, "Monitor List has been changed");
                    break;
                }

                case PAL_EVENT_TYPE_KEYDOWN:
                case PAL_EVENT_TYPE_KEYREPEAT:
                case PAL_EVENT_TYPE_KEYUP: {
                    uint32_t keycode, scancode;
                    palUnpackUint32(event.data, &keycode, &scancode);
                    PalWindow* window = palUnpackPointer(event.data2);

                    const char* action = "pressed";
                    if (event.type == PAL_EVENT_TYPE_KEYREPEAT) {
                        action = "repeat";

                    } else if (event.type == PAL_EVENT_TYPE_KEYUP) {
                        action = "released";
                    }

                    // get keycode and scancode name
                    const char* keyName = s_KeyNames[keycode];
                    const char* scancodeName = s_ScancodeNames[scancode];
                    palLog(nullptr, "Key %s: (%s, %s)", action, keyName, scancodeName);

                    if (keycode == PAL_KEYCODE_ESCAPE && event.type == PAL_EVENT_TYPE_KEYDOWN) {
                        running = PAL_FALSE;
                    }
                    break;
                }

                case PAL_EVENT_TYPE_MOUSE_BUTTONDOWN:
                case PAL_EVENT_TYPE_MOUSE_BUTTONUP: {
                    uint32_t button, serial;
                    palUnpackUint32(event.data, &button, &serial);
                    PalWindow* window = palUnpackPointer(event.data2);

                    const char* action = "pressed";
                    if (event.type == PAL_EVENT_TYPE_MOUSE_BUTTONUP) {
                        action = "released";
                    }

                    // get mouse button name
                    const char* name = s_MouseButtonNames[button];
                    palLog(nullptr, "Mouse Button %s: %s", action, name);
                    break;
                }

                case PAL_EVENT_TYPE_MOUSE_MOVE: {
                    int32_t x, y;
                    palUnpackInt32(event.data, &x, &y);
                    PalWindow* window = palUnpackPointer(event.data2);
                    palLog(nullptr, "Mouse Moved: (%d, %d)", x, y);
                    break;
                }

                case PAL_EVENT_TYPE_MOUSE_DELTA:
                case PAL_EVENT_TYPE_MOUSE_WHEEL: {
                    float dx, dy;
                    palUnpackFloat(event.data, &dx, &dy);
                    PalWindow* window = palUnpackPointer(event.data2);

                    const char* action = "Wheel";
                    if (event.type == PAL_EVENT_TYPE_MOUSE_DELTA) {
                        action = "Delta";
                    }

                    palLog(nullptr, "Mouse %s: (%.2f, %.2f)", action, dx, dy);
                    break;
                }

                case PAL_EVENT_TYPE_WINDOW_DECORATION_MODE: {
                    if (event.data == PAL_DECORATION_MODE_CLIENT_SIDE) {
                        palLog(nullptr, "Window Decoration Mode: Client Side");
                    } else {
                        palLog(nullptr, "Window Decoration Mode: Server Side");
                    }
                    break;
                }
            }
        }
    }

    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);

    return PAL_TRUE;
}

#endif // PAL_HAS_VIDEO_MODULE

void registerVideoTests()
{
#if PAL_HAS_VIDEO_MODULE
    // registerTest(monitorTest, "Monitor Test");
    // registerTest(simpleWindowTest, "Simple Window Test");
    registerTest(eventWindowTest, "Event Window Test");
#endif // PAL_HAS_VIDEO_MODULE
}