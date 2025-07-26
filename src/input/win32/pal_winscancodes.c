
#include "pal_pch.h"
#include "input/pal_input_internal.h"

// ==================================================
// Internal API
// ==================================================

// Scancodes are made from (scancode | extended)

void palMapScancodes(PalInput input) {

    // Letters

    input->scancodes[0x01E] = PAL_SCANCODE_A;
    input->scancodes[0x030] = PAL_SCANCODE_B;
    input->scancodes[0x02E] = PAL_SCANCODE_C;
    input->scancodes[0x020] = PAL_SCANCODE_D;
    input->scancodes[0x012] = PAL_SCANCODE_E;
    input->scancodes[0x021] = PAL_SCANCODE_F;
    input->scancodes[0x022] = PAL_SCANCODE_G;
    input->scancodes[0x023] = PAL_SCANCODE_H;
    input->scancodes[0x017] = PAL_SCANCODE_I;
    input->scancodes[0x024] = PAL_SCANCODE_J;
    input->scancodes[0x025] = PAL_SCANCODE_K;
    input->scancodes[0x026] = PAL_SCANCODE_L;
    input->scancodes[0x032] = PAL_SCANCODE_M;
    input->scancodes[0x031] = PAL_SCANCODE_N;
    input->scancodes[0x018] = PAL_SCANCODE_O;
    input->scancodes[0x019] = PAL_SCANCODE_P;
    input->scancodes[0x010] = PAL_SCANCODE_Q;
    input->scancodes[0x013] = PAL_SCANCODE_R;
    input->scancodes[0x01F] = PAL_SCANCODE_S;
    input->scancodes[0x014] = PAL_SCANCODE_T;
    input->scancodes[0x016] = PAL_SCANCODE_U;
    input->scancodes[0x02F] = PAL_SCANCODE_V;
    input->scancodes[0x011] = PAL_SCANCODE_W;
    input->scancodes[0x02D] = PAL_SCANCODE_X;
    input->scancodes[0x015] = PAL_SCANCODE_Y;
    input->scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)

    input->scancodes[0x00B] = PAL_SCANCODE_0;
    input->scancodes[0x002] = PAL_SCANCODE_1;
    input->scancodes[0x003] = PAL_SCANCODE_2;
    input->scancodes[0x004] = PAL_SCANCODE_3;
    input->scancodes[0x005] = PAL_SCANCODE_4;
    input->scancodes[0x006] = PAL_SCANCODE_5;
    input->scancodes[0x007] = PAL_SCANCODE_6;
    input->scancodes[0x008] = PAL_SCANCODE_7;
    input->scancodes[0x009] = PAL_SCANCODE_8;
    input->scancodes[0x00A] = PAL_SCANCODE_9;

    // Function

    input->scancodes[0x03B] = PAL_SCANCODE_F1;
    input->scancodes[0x03C] = PAL_SCANCODE_F2;
    input->scancodes[0x03D] = PAL_SCANCODE_F3;
    input->scancodes[0x03E] = PAL_SCANCODE_F4;
    input->scancodes[0x03F] = PAL_SCANCODE_F5;
    input->scancodes[0x040] = PAL_SCANCODE_F6;
    input->scancodes[0x041] = PAL_SCANCODE_F7;
    input->scancodes[0x042] = PAL_SCANCODE_F8;
    input->scancodes[0x043] = PAL_SCANCODE_F9;
    input->scancodes[0x044] = PAL_SCANCODE_F10;
    input->scancodes[0x057] = PAL_SCANCODE_F11;
    input->scancodes[0x058] = PAL_SCANCODE_F12;

    // Control

    input->scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    input->scancodes[0x01C] = PAL_SCANCODE_ENTER;
    input->scancodes[0x00F] = PAL_SCANCODE_TAB;
    input->scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    input->scancodes[0x039] = PAL_SCANCODE_SPACE;
    input->scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    input->scancodes[0x145] = PAL_SCANCODE_NUMLOCK;
    input->scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    input->scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    input->scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    input->scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    input->scancodes[0x11D] = PAL_SCANCODE_RCTRL;
    input->scancodes[0x038] = PAL_SCANCODE_LALT;
    input->scancodes[0x138] = PAL_SCANCODE_RALT;

    // Arrows

    input->scancodes[0x14B] = PAL_SCANCODE_LEFT;
    input->scancodes[0x14D] = PAL_SCANCODE_RIGHT;
    input->scancodes[0x148] = PAL_SCANCODE_UP;
    input->scancodes[0x150] = PAL_SCANCODE_DOWN;

    // Navigation

    input->scancodes[0x152] = PAL_SCANCODE_INSERT;
    input->scancodes[0x153] = PAL_SCANCODE_DELETE;
    input->scancodes[0x147] = PAL_SCANCODE_HOME;
    input->scancodes[0x14F] = PAL_SCANCODE_END;
    input->scancodes[0x149] = PAL_SCANCODE_PAGEUP;
    input->scancodes[0x151] = PAL_SCANCODE_PAGEDOWN;

    // Keypad

    input->scancodes[0x052] = PAL_SCANCODE_KP_0;
    input->scancodes[0x04F] = PAL_SCANCODE_KP_1;
    input->scancodes[0x050] = PAL_SCANCODE_KP_2;
    input->scancodes[0x051] = PAL_SCANCODE_KP_3;
    input->scancodes[0x04B] = PAL_SCANCODE_KP_4;
    input->scancodes[0x04C] = PAL_SCANCODE_KP_5;
    input->scancodes[0x04D] = PAL_SCANCODE_KP_6;
    input->scancodes[0x047] = PAL_SCANCODE_KP_7;
    input->scancodes[0x048] = PAL_SCANCODE_KP_8;
    input->scancodes[0x049] = PAL_SCANCODE_KP_9;
    input->scancodes[0x11C] = PAL_SCANCODE_KP_ENTER;
    input->scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    input->scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    input->scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    input->scancodes[0x135] = PAL_SCANCODE_KP_DIVIDE;
    input->scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;
    input->scancodes[0x059] = PAL_SCANCODE_KP_EQUAL;

    // Misc

    input->scancodes[0x137] = PAL_SCANCODE_PRINTSCREEN;
    input->scancodes[0x146] = PAL_SCANCODE_PAUSE;    
    input->scancodes[0x045] = PAL_SCANCODE_PAUSE;
    input->scancodes[0x15D] = PAL_SCANCODE_MENU;
    input->scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    input->scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    input->scancodes[0x033] = PAL_SCANCODE_COMMA;
    input->scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    input->scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    input->scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    input->scancodes[0x034] = PAL_SCANCODE_PERIOD;
    input->scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    input->scancodes[0x035] = PAL_SCANCODE_SLASH;
    input->scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    input->scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    input->scancodes[0x15B] = PAL_SCANCODE_LSUPER;
    input->scancodes[0x15C] = PAL_SCANCODE_RSUPER;
}