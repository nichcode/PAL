
#include "pal_pch.h"
#include "input/pal_input_internal.h"

// ==================================================
// Internal API
// ==================================================

void palMapKeycodes(PalInput input) {

    // Letters
    
    input->keycodes['A'] = PAL_KEY_A;
    input->keycodes['B'] = PAL_KEY_B;
    input->keycodes['C'] = PAL_KEY_C;
    input->keycodes['D'] = PAL_KEY_D;
    input->keycodes['E'] = PAL_KEY_E;
    input->keycodes['F'] = PAL_KEY_F;

    input->keycodes['G'] = PAL_KEY_G;
    input->keycodes['H'] = PAL_KEY_H;
    input->keycodes['I'] = PAL_KEY_I;
    input->keycodes['J'] = PAL_KEY_J;
    input->keycodes['K'] = PAL_KEY_K;
    input->keycodes['L'] = PAL_KEY_L;
    input->keycodes['M'] = PAL_KEY_M;
    input->keycodes['N'] = PAL_KEY_N;
    input->keycodes['O'] = PAL_KEY_O;
    input->keycodes['P'] = PAL_KEY_P;
    input->keycodes['Q'] = PAL_KEY_Q;
    input->keycodes['R'] = PAL_KEY_R;
    input->keycodes['S'] = PAL_KEY_S;
    input->keycodes['T'] = PAL_KEY_T;
    input->keycodes['U'] = PAL_KEY_U;
    input->keycodes['V'] = PAL_KEY_V;
    input->keycodes['W'] = PAL_KEY_W;
    input->keycodes['Z'] = PAL_KEY_Z;
    input->keycodes['Y'] = PAL_KEY_Y;
    input->keycodes['Z'] = PAL_KEY_Z;

    // Numbers (top row)

    input->keycodes['0'] = PAL_KEY_0;
    input->keycodes['1'] = PAL_KEY_1;
    input->keycodes['2'] = PAL_KEY_2;
    input->keycodes['3'] = PAL_KEY_3;
    input->keycodes['4'] = PAL_KEY_4;
    input->keycodes['5'] = PAL_KEY_5;
    input->keycodes['6'] = PAL_KEY_6;
    input->keycodes['7'] = PAL_KEY_7;
    input->keycodes['8'] = PAL_KEY_8;
    input->keycodes['9'] = PAL_KEY_9;

    // Function

    input->keycodes[VK_F1] = PAL_KEY_F1;
    input->keycodes[VK_F2] = PAL_KEY_F2;
    input->keycodes[VK_F3] = PAL_KEY_F3;
    input->keycodes[VK_F4] = PAL_KEY_F4;
    input->keycodes[VK_F5] = PAL_KEY_F5;
    input->keycodes[VK_F6] = PAL_KEY_F6;
    input->keycodes[VK_F7] = PAL_KEY_F7;
    input->keycodes[VK_F8] = PAL_KEY_F8;
    input->keycodes[VK_F9] = PAL_KEY_F9;
    input->keycodes[VK_F10] = PAL_KEY_F10;
    input->keycodes[VK_F11] = PAL_KEY_F11;
    input->keycodes[VK_F12] = PAL_KEY_F12;

    // Control

    input->keycodes[VK_ESCAPE] = PAL_KEY_ESCAPE;
    input->keycodes[VK_RETURN] = PAL_KEY_ENTER;
    input->keycodes[VK_TAB] = PAL_KEY_TAB;
    input->keycodes[VK_BACK] = PAL_KEY_BACKSPACE;
    input->keycodes[VK_SPACE] = PAL_KEY_SPACE;
    input->keycodes[VK_CAPITAL] = PAL_KEY_CAPSLOCK;
    input->keycodes[VK_NUMLOCK] = PAL_KEY_NUMLOCK;
    input->keycodes[VK_SCROLL] = PAL_KEY_SCROLLLOCK;
    input->keycodes[VK_LSHIFT] = PAL_KEY_LSHIFT;
    input->keycodes[VK_RSHIFT] = PAL_KEY_RSHIFT;
    input->keycodes[VK_LCONTROL] = PAL_KEY_LCTRL;
    input->keycodes[VK_RCONTROL] = PAL_KEY_RCTRL;
    input->keycodes[VK_LMENU] = PAL_KEY_LALT;
    input->keycodes[VK_RMENU] = PAL_KEY_RALT;

    // Arrows

    input->keycodes[VK_LEFT] = PAL_KEY_LEFT;
    input->keycodes[VK_RIGHT] = PAL_KEY_RIGHT;
    input->keycodes[VK_UP] = PAL_KEY_UP;
    input->keycodes[VK_DOWN] = PAL_KEY_DOWN;

    // Navigation

    input->keycodes[VK_INSERT] = PAL_KEY_INSERT;
    input->keycodes[VK_DELETE] = PAL_KEY_DELETE;
    input->keycodes[VK_HOME] = PAL_KEY_HOME;
    input->keycodes[VK_END] = PAL_KEY_END;
    input->keycodes[VK_PRIOR] = PAL_KEY_PAGEUP;
    input->keycodes[VK_NEXT] = PAL_KEY_PAGEDOWN;

    // Keypad

    input->keycodes[VK_NUMPAD0] = PAL_KEY_KP_0;
    input->keycodes[VK_NUMPAD1] = PAL_KEY_KP_1;
    input->keycodes[VK_NUMPAD2] = PAL_KEY_KP_2;
    input->keycodes[VK_NUMPAD3] = PAL_KEY_KP_3;
    input->keycodes[VK_NUMPAD4] = PAL_KEY_KP_4;
    input->keycodes[VK_NUMPAD5] = PAL_KEY_KP_5;
    input->keycodes[VK_NUMPAD6] = PAL_KEY_KP_6;
    input->keycodes[VK_NUMPAD7] = PAL_KEY_KP_7;
    input->keycodes[VK_NUMPAD8] = PAL_KEY_KP_8;
    input->keycodes[VK_NUMPAD9] = PAL_KEY_KP_9;
    
    input->keycodes[VK_ADD] = PAL_KEY_KP_ADD;
    input->keycodes[VK_SUBTRACT] = PAL_KEY_KP_SUBTRACT;
    input->keycodes[VK_MULTIPLY] = PAL_KEY_KP_MULTIPLY;
    input->keycodes[VK_DIVIDE] = PAL_KEY_KP_DIVIDE;
    input->keycodes[VK_DECIMAL] = PAL_KEY_KP_DECIMAL;

    // Misc

    input->keycodes[VK_SNAPSHOT] = PAL_KEY_PRINTSCREEN;
    input->keycodes[VK_PAUSE] = PAL_KEY_PAUSE;    
    input->keycodes[VK_APPS] = PAL_KEY_MENU;
    input->keycodes[VK_OEM_7] = PAL_KEY_APOSTROPHE;
    input->keycodes[VK_OEM_5] = PAL_KEY_BACKSLASH;
    input->keycodes[VK_OEM_COMMA] = PAL_KEY_COMMA;
    input->keycodes[VK_OEM_PLUS] = PAL_KEY_EQUAL;
    input->keycodes[VK_OEM_3] = PAL_KEY_GRAVEACCENT;
    input->keycodes[VK_OEM_MINUS] = PAL_KEY_SUBTRACT;
    input->keycodes[VK_OEM_PERIOD] = PAL_KEY_PERIOD;
    input->keycodes[VK_OEM_1] = PAL_KEY_SEMICOLON;
    input->keycodes[VK_OEM_2] = PAL_KEY_SLASH;
    input->keycodes[VK_OEM_4] = PAL_KEY_LBRACKET;
    input->keycodes[VK_OEM_6] = PAL_KEY_RBRACKET;
    input->keycodes[VK_LWIN] = PAL_KEY_LSUPER;
    input->keycodes[VK_RWIN] = PAL_KEY_RSUPER;
}