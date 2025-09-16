
#include "pal/pal_video.h"
#include "tests.h"

static const char* s_KeyNames[PAL_KEYCODE_MAX] = {

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
    [PAL_KEYCODE_F1]  = "F1",
    [PAL_KEYCODE_F2]  = "F2",
    [PAL_KEYCODE_F3]  = "F3",
    [PAL_KEYCODE_F4]  = "F4",
    [PAL_KEYCODE_F5]  = "F5",
    [PAL_KEYCODE_F6]  = "F6",
    [PAL_KEYCODE_F7]  = "F7",
    [PAL_KEYCODE_F8]  = "F8",
    [PAL_KEYCODE_F9]  = "F9",
    [PAL_KEYCODE_F10] = "F10",
    [PAL_KEYCODE_F11] = "F11",
    [PAL_KEYCODE_F12] = "F12",

    // Control
    [PAL_KEYCODE_ESCAPE]     = "Escape",
    [PAL_KEYCODE_ENTER]      = "Enter",
    [PAL_KEYCODE_TAB]        = "Tab",
    [PAL_KEYCODE_BACKSPACE]  = "Backspace",
    [PAL_KEYCODE_SPACE]      = "Space",
    [PAL_KEYCODE_CAPSLOCK]   = "Capslock",
    [PAL_KEYCODE_NUMLOCK]    = "Numlock",
    [PAL_KEYCODE_SCROLLLOCK] = "Scrolllock",
    [PAL_KEYCODE_LSHIFT]     = "LeftShift",
    [PAL_KEYCODE_RSHIFT]     = "RightShift",
    [PAL_KEYCODE_LCTRL]      = "LeftControl",
    [PAL_KEYCODE_RCTRL]      = "RightControl",
    [PAL_KEYCODE_LALT]       = "LeftAlt",
    [PAL_KEYCODE_RALT]       = "RightAlt",

    // Arrows
    [PAL_KEYCODE_LEFT]  = "Left",
    [PAL_KEYCODE_RIGHT] = "Right",
    [PAL_KEYCODE_UP]    = "Up",
    [PAL_KEYCODE_DOWN]  = "Down",

    // Navigation

    [PAL_KEYCODE_INSERT]   = "Insert",
    [PAL_KEYCODE_DELETE]   = "Delete",
    [PAL_KEYCODE_HOME]     = "Home",
    [PAL_KEYCODE_END]      = "End",
    [PAL_KEYCODE_PAGEUP]   = "Pageup",
    [PAL_KEYCODE_PAGEDOWN] = "Pagedown",

    // Keypad
    [PAL_KEYCODE_KP_0]        = "Keypad 0",
    [PAL_KEYCODE_KP_1]        = "Keypad 1",
    [PAL_KEYCODE_KP_2]        = "Keypad 2",
    [PAL_KEYCODE_KP_3]        = "Keypad 3",
    [PAL_KEYCODE_KP_4]        = "Keypad 4",
    [PAL_KEYCODE_KP_5]        = "Keypad 5",
    [PAL_KEYCODE_KP_6]        = "Keypad 6",
    [PAL_KEYCODE_KP_7]        = "Keypad 7",
    [PAL_KEYCODE_KP_8]        = "Keypad 8",
    [PAL_KEYCODE_KP_9]        = "Keypad 9",
    [PAL_KEYCODE_KP_ENTER]    = "Keypad Enter",
    [PAL_KEYCODE_KP_ADD]      = "Keypad Add",
    [PAL_KEYCODE_KP_SUBTRACT] = "Keypad Subtract",
    [PAL_KEYCODE_KP_MULTIPLY] = "Keypad Multiply",
    [PAL_KEYCODE_KP_DIVIDE]   = "Keypad Divide",
    [PAL_KEYCODE_KP_DECIMAL]  = "Keypad Decimal",
    [PAL_KEYCODE_KP_EQUAL]    = "Keypad Equal",

    // Misc
    [PAL_KEYCODE_PRINTSCREEN] = "Printscreen",
    [PAL_KEYCODE_PAUSE]       = "Pause",
    [PAL_KEYCODE_MENU]        = "Menu",
    [PAL_KEYCODE_APOSTROPHE]  = "Apostrophe",
    [PAL_KEYCODE_BACKSLASH]   = "Backslash",
    [PAL_KEYCODE_COMMA]       = "Comma",
    [PAL_KEYCODE_EQUAL]       = "Equal",
    [PAL_KEYCODE_GRAVEACCENT] = "Graveaccent",
    [PAL_KEYCODE_SUBTRACT]    = "Subtract",
    [PAL_KEYCODE_PERIOD]      = "Period",
    [PAL_KEYCODE_SEMICOLON]   = "Semicolon",
    [PAL_KEYCODE_SLASH]       = "Slash",
    [PAL_KEYCODE_LBRACKET]    = "LeftBracket",
    [PAL_KEYCODE_RBRACKET]    = "RightBracket",
    [PAL_KEYCODE_LSUPER]      = "LeftSuper",
    [PAL_KEYCODE_RSUPER]      = "RightSuper",
};

static const char* s_ScancodeNames[PAL_SCANCODE_MAX] = {

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
    [PAL_SCANCODE_F1]  = "F1",
    [PAL_SCANCODE_F2]  = "F2",
    [PAL_SCANCODE_F3]  = "F3",
    [PAL_SCANCODE_F4]  = "F4",
    [PAL_SCANCODE_F5]  = "F5",
    [PAL_SCANCODE_F6]  = "F6",
    [PAL_SCANCODE_F7]  = "F7",
    [PAL_SCANCODE_F8]  = "F8",
    [PAL_SCANCODE_F9]  = "F9",
    [PAL_SCANCODE_F10] = "F10",
    [PAL_SCANCODE_F11] = "F11",
    [PAL_SCANCODE_F12] = "F12",

    // Control
    [PAL_SCANCODE_ESCAPE]     = "Escape",
    [PAL_SCANCODE_ENTER]      = "Enter",
    [PAL_SCANCODE_TAB]        = "Tab",
    [PAL_SCANCODE_BACKSPACE]  = "Backspace",
    [PAL_SCANCODE_SPACE]      = "Space",
    [PAL_SCANCODE_CAPSLOCK]   = "Capslock",
    [PAL_SCANCODE_NUMLOCK]    = "Numlock",
    [PAL_SCANCODE_SCROLLLOCK] = "Scrolllock",
    [PAL_SCANCODE_LSHIFT]     = "LeftShift",
    [PAL_SCANCODE_RSHIFT]     = "RightShift",
    [PAL_SCANCODE_LCTRL]      = "LeftControl",
    [PAL_SCANCODE_RCTRL]      = "RightControl",
    [PAL_SCANCODE_LALT]       = "LeftAlt",
    [PAL_SCANCODE_RALT]       = "RightAlt",

    // Arrows
    [PAL_SCANCODE_LEFT]  = "Left",
    [PAL_SCANCODE_RIGHT] = "Right",
    [PAL_SCANCODE_UP]    = "Up",
    [PAL_SCANCODE_DOWN]  = "Down",

    // Navigation
    [PAL_SCANCODE_INSERT]   = "Insert",
    [PAL_SCANCODE_DELETE]   = "Delete",
    [PAL_SCANCODE_HOME]     = "Home",
    [PAL_SCANCODE_END]      = "End",
    [PAL_SCANCODE_PAGEUP]   = "Pageup",
    [PAL_SCANCODE_PAGEDOWN] = "Pagedown",

    // Keypad
    [PAL_SCANCODE_KP_0]        = "Keypad 0",
    [PAL_SCANCODE_KP_1]        = "Keypad 1",
    [PAL_SCANCODE_KP_2]        = "Keypad 2",
    [PAL_SCANCODE_KP_3]        = "Keypad 3",
    [PAL_SCANCODE_KP_4]        = "Keypad 4",
    [PAL_SCANCODE_KP_5]        = "Keypad 5",
    [PAL_SCANCODE_KP_6]        = "Keypad 6",
    [PAL_SCANCODE_KP_7]        = "Keypad 7",
    [PAL_SCANCODE_KP_8]        = "Keypad 8",
    [PAL_SCANCODE_KP_9]        = "Keypad 9",
    [PAL_SCANCODE_KP_ENTER]    = "Keypad Enter",
    [PAL_SCANCODE_KP_ADD]      = "Keypad Add",
    [PAL_SCANCODE_KP_SUBTRACT] = "Keypad Subtract",
    [PAL_SCANCODE_KP_MULTIPLY] = "Keypad Multiply",
    [PAL_SCANCODE_KP_DIVIDE]   = "Keypad Divide",
    [PAL_SCANCODE_KP_DECIMAL]  = "Keypad Decimal",
    [PAL_SCANCODE_KP_EQUAL]    = "Keypad Equal",

    // Misc
    [PAL_SCANCODE_PRINTSCREEN] = "Printscreen",
    [PAL_SCANCODE_PAUSE]       = "Pause",
    [PAL_SCANCODE_MENU]        = "Menu",
    [PAL_SCANCODE_APOSTROPHE]  = "Apostrophe",
    [PAL_SCANCODE_BACKSLASH]   = "Backslash",
    [PAL_SCANCODE_COMMA]       = "Comma",
    [PAL_SCANCODE_EQUAL]       = "Equal",
    [PAL_SCANCODE_GRAVEACCENT] = "Graveaccent",
    [PAL_SCANCODE_SUBTRACT]    = "Subtract",
    [PAL_SCANCODE_PERIOD]      = "Period",
    [PAL_SCANCODE_SEMICOLON]   = "Semicolon",
    [PAL_SCANCODE_SLASH]       = "Slash",
    [PAL_SCANCODE_LBRACKET]    = "LeftBracket",
    [PAL_SCANCODE_RBRACKET]    = "RightBracket",
    [PAL_SCANCODE_LSUPER]      = "LeftSuper",
    [PAL_SCANCODE_RSUPER]      = "RightSuper"};

static const char* s_MouseButtonNames[PAL_MOUSE_BUTTON_MAX] = {

    [PAL_MOUSE_BUTTON_UNKNOWN] = "Unknown",

    [PAL_MOUSE_BUTTON_LEFT]   = "Left",
    [PAL_MOUSE_BUTTON_RIGHT]  = "Right",
    [PAL_MOUSE_BUTTON_MIDDLE] = "Middle",
    [PAL_MOUSE_BUTTON_X1]     = "X1",
    [PAL_MOUSE_BUTTON_X2]     = "X2"};

#if DISPATCH_MODE_POLL
static const char* dispatchString = "Poll Mode";
#else
static const char* dispatchString = "Callback Mode";
#endif // DISPATCH_MODE_POLL

// inline helpers
static inline void onKeydown(const PalEvent* event)
{

    Uint32 keycode, scancode; // keycode == low, scancode == high
    palUnpackUint32(event->data, &keycode, &scancode);
    PalWindow* window = palUnpackPointer(event->data2);

    // get keycode and scancode name
    const char* keyName      = s_KeyNames[keycode];
    const char* scancodeName = s_ScancodeNames[scancode];
    palLog(nullptr, "%s: Key pressed: (%s, %s)", dispatchString, keyName, scancodeName);
}

static inline void onKeyrepeat(const PalEvent* event)
{

    Uint32 keycode, scancode; // keycode == low, scancode == high
    palUnpackUint32(event->data, &keycode, &scancode);
    PalWindow* window = palUnpackPointer(event->data2);

    // get keycode and scancode name
    const char* keyName      = s_KeyNames[keycode];
    const char* scancodeName = s_ScancodeNames[scancode];
    palLog(nullptr, "%s: Key repeat: (%s, %s)", dispatchString, keyName, scancodeName);
}

static inline void onKeyup(const PalEvent* event)
{

    Uint32 keycode, scancode; // keycode == low, scancode == high
    palUnpackUint32(event->data, &keycode, &scancode);
    PalWindow* window = palUnpackPointer(event->data2);

    // get keycode and scancode name
    const char* keyName      = s_KeyNames[keycode];
    const char* scancodeName = s_ScancodeNames[scancode];
    palLog(nullptr, "%s: Key released: (%s, %s)", dispatchString, keyName, scancodeName);
}

static inline void onMouseButtondown(const PalEvent* event)
{

    PalWindow* window = palUnpackPointer(event->data2);

    // get mouse button name
    const char* name = s_MouseButtonNames[event->data];
    palLog(nullptr, "%s: Mouse Button pressed: %s", dispatchString, name);
}

static inline void onMouseButtonup(const PalEvent* event)
{

    PalWindow* window = palUnpackPointer(event->data2);

    // get mouse button name
    const char* name = s_MouseButtonNames[event->data];
    palLog(nullptr, "%s: Mouse Button released: %s", dispatchString, name);
}

static inline void onMouseMove(const PalEvent* event)
{

    Int32 x, y; // x == low, y == high
    palUnpackInt32(event->data, &x, &y);
    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Mouse Moved: (%d, %d)", dispatchString, x, y);
}

static inline void onMouseDelta(const PalEvent* event)
{

    Int32 dx, dy; // dx == low, dy == high
    palUnpackInt32(event->data, &dx, &dy);
    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Mouse Delta: (%d, %d)", dispatchString, dx, dy);
}

static inline void onMouseWheel(const PalEvent* event)
{

    Int32 dx, dy; // dx == low, dy == high
    palUnpackInt32(event->data, &dx, &dy);
    PalWindow* window = palUnpackPointer(event->data2);
    palLog(nullptr, "%s: Mouse Wheel: (%d, %d)", dispatchString, dx, dy);
}

static void PAL_CALL onEvent(void* userData, const PalEvent* event)
{

    if (event->type == PAL_EVENT_KEYDOWN) {
        onKeydown(event);

    } else if (event->type == PAL_EVENT_KEYREPEAT) {
        onKeyrepeat(event);

    } else if (event->type == PAL_EVENT_KEYUP) {
        onKeyup(event);

    } else if (event->type == PAL_EVENT_MOUSE_BUTTONDOWN) {
        onMouseButtondown(event);

    } else if (event->type == PAL_EVENT_MOUSE_BUTTONUP) {
        onMouseButtonup(event);

    } else if (event->type == PAL_EVENT_MOUSE_MOVE) {
        onMouseMove(event);

    } else if (event->type == PAL_EVENT_MOUSE_DELTA) {
        onMouseDelta(event);

    } else if (event->type == PAL_EVENT_MOUSE_WHEEL) {
        onMouseWheel(event);
    }
}

bool inputWindowTest()
{

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Input Window Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult           result;
    PalWindow*          window     = nullptr;
    PalWindowCreateInfo createInfo = {0};
    bool                running    = false;

    // event driver
    PalEventDriver*          eventDriver           = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {};

    // fill the event driver create info
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback  = onEvent; // for callback dispatch
    eventDriverCreateInfo.queue     = nullptr; // default queue
    eventDriverCreateInfo.userData  = nullptr; // null

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create event driver %s", palFormatResult(result));
        return false;
    }

    // initialize the video system. We pass the event driver to recieve video related events
    // the video system does not copy the event driver, it must be valid till the video system is
    // shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to initialize video %s", palFormatResult(result));
        return false;
    }

    // fill the create info struct
    createInfo.monitor = nullptr; // use primary monitor
    createInfo.height  = 480;
    createInfo.width   = 640;
    createInfo.show    = true;
    createInfo.style   = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title   = "PAL Input Window";

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create window %s", palFormatResult(result));
        return false;
    }

    // we set window close to poll
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);

    PalDispatchMode dispatchMode = PAL_DISPATCH_NONE;
#if DISPATCH_MODE_POLL
    dispatchMode = PAL_DISPATCH_POLL;
#else
    dispatchMode = PAL_DISPATCH_CALLBACK;
#endif // DISPATCH_MODE_POLL

    // set dispatch mode for all events.
    for (Uint32 e = PAL_EVENT_KEYDOWN; e < PAL_EVENT_USER; e++) {
        palSetEventDispatchMode(eventDriver, e, dispatchMode);
    }

    running = true;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
            case PAL_EVENT_WINDOW_CLOSE: {
                running = false;
                break;
            }

            case PAL_EVENT_KEYDOWN: {
                onKeydown(&event);
            }

            case PAL_EVENT_KEYREPEAT: {
                onKeyrepeat(&event);
            }

            case PAL_EVENT_KEYUP: {
                onKeyup(&event);
            }

            case PAL_EVENT_MOUSE_BUTTONDOWN: {
                onMouseButtondown(&event);
            }

            case PAL_EVENT_MOUSE_BUTTONUP: {
                onMouseButtonup(&event);
            }

            case PAL_EVENT_MOUSE_MOVE: {
                onMouseMove(&event);
            }

            case PAL_EVENT_MOUSE_DELTA: {
                onMouseDelta(&event);
            }

            case PAL_EVENT_MOUSE_WHEEL: {
                onMouseWheel(&event);
            }
            }
        }
    }

    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    return true;
}