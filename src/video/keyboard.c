/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "platform.h"
#include "pal2/video/keyboard.h"

Keyboard g_Keyboard = {0};

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
    [PAL_KEYCODE_CAPSLOCK] = "CapsLock",
    [PAL_KEYCODE_NUMLOCK] = "NumLock",
    [PAL_KEYCODE_SCROLLLOCK] = "ScrollLock",
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
    [PAL_KEYCODE_PAGEUP] = "PageUp",
    [PAL_KEYCODE_PAGEDOWN] = "PageDown",

    // Keypad
    [PAL_KEYCODE_KP_0] = "Keypad0",
    [PAL_KEYCODE_KP_1] = "Keypad1",
    [PAL_KEYCODE_KP_2] = "Keypad2",
    [PAL_KEYCODE_KP_3] = "Keypad3",
    [PAL_KEYCODE_KP_4] = "Keypad4",
    [PAL_KEYCODE_KP_5] = "Keypad5",
    [PAL_KEYCODE_KP_6] = "Keypad6",
    [PAL_KEYCODE_KP_7] = "Keypad7",
    [PAL_KEYCODE_KP_8] = "Keypad8",
    [PAL_KEYCODE_KP_9] = "Keypad9",
    [PAL_KEYCODE_KP_ENTER] = "KeypadEnter",
    [PAL_KEYCODE_KP_ADD] = "KeypadAdd",
    [PAL_KEYCODE_KP_SUBTRACT] = "KeypadSubtract",
    [PAL_KEYCODE_KP_MULTIPLY] = "KeypadMultiply",
    [PAL_KEYCODE_KP_DIVIDE] = "KeypadDivide",
    [PAL_KEYCODE_KP_DECIMAL] = "KeypadDecimal",
    [PAL_KEYCODE_KP_EQUAL] = "KeypadEqual",

    // Misc
    [PAL_KEYCODE_PRINTSCREEN] = "PrintScreen",
    [PAL_KEYCODE_PAUSE] = "Pause",
    [PAL_KEYCODE_MENU] = "Menu",
    [PAL_KEYCODE_APOSTROPHE] = "Apostrophe",
    [PAL_KEYCODE_BACKSLASH] = "Backslash",
    [PAL_KEYCODE_COMMA] = "Comma",
    [PAL_KEYCODE_EQUAL] = "Equal",
    [PAL_KEYCODE_GRAVEACCENT] = "GraveAccent",
    [PAL_KEYCODE_SUBTRACT] = "Subtract",
    [PAL_KEYCODE_PERIOD] = "Period",
    [PAL_KEYCODE_SEMICOLON] = "Semicolon",
    [PAL_KEYCODE_SLASH] = "Slash",
    [PAL_KEYCODE_LBRACKET] = "LeftBracket",
    [PAL_KEYCODE_RBRACKET] = "RightBracket",
    [PAL_KEYCODE_LSUPER] = "LeftSuper",
    [PAL_KEYCODE_RSUPER] = "RightSuper",
};

const PalBool* PAL_CALL palGetKeycodeState(void)
{
    return g_Keyboard.keycodeState;
}

const PalBool* PAL_CALL palGetScancodeState(void)
{
    return g_Keyboard.scancodeState;
}

const char* PAL_CALL palGetScancodeName(PalScancode scancode)
{
    // We keep one mapping and do a direct cast from a scancode to keycode.
    // This works because both share the same literal.
    PalKeycode key = (PalKeycode)scancode;
    if (key >= PAL_KEYCODE_COUNT || !s_KeyNames[key]) {
        return s_KeyNames[PAL_KEYCODE_UNKNOWN];
    }
    
    return s_KeyNames[key];
}

const char* PAL_CALL palGetKeycodeName(PalKeycode keycode)
{
    if (keycode >= PAL_KEYCODE_COUNT || !s_KeyNames[keycode]) {
        return s_KeyNames[PAL_KEYCODE_UNKNOWN];
    }

    return s_KeyNames[keycode];
}