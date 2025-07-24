
#include "pal_pch.h"
#include "input/pal_input_internal.h"

#define SC_INDEX(sc, ext) ((Uint16) (sc) | ((ext) << 8))
#define MAP_SC(sc, ext, input, code) input->scancodes[SC_INDEX((sc), (ext))] = (code);

void palMapScancodes(PalInput input) {

    // Letters
    MAP_SC(0x1E, 0, input, PAL_SCANCODE_A);
    MAP_SC(0x30, 0, input, PAL_SCANCODE_B);
    MAP_SC(0x2E, 0, input, PAL_SCANCODE_C);
    MAP_SC(0x20, 0, input, PAL_SCANCODE_D);
    MAP_SC(0x12, 0, input, PAL_SCANCODE_E);
    MAP_SC(0x21, 0, input, PAL_SCANCODE_F);
    MAP_SC(0x22, 0, input, PAL_SCANCODE_G);
    MAP_SC(0x23, 0, input, PAL_SCANCODE_H);
    MAP_SC(0x17, 0, input, PAL_SCANCODE_I);
    MAP_SC(0x24, 0, input, PAL_SCANCODE_J);
    MAP_SC(0x25, 0, input, PAL_SCANCODE_K);
    MAP_SC(0x26, 0, input, PAL_SCANCODE_L);
    MAP_SC(0x32, 0, input, PAL_SCANCODE_M);
    MAP_SC(0x31, 0, input, PAL_SCANCODE_N);
    MAP_SC(0x18, 0, input, PAL_SCANCODE_O);
    MAP_SC(0x19, 0, input, PAL_SCANCODE_P);
    MAP_SC(0x10, 0, input, PAL_SCANCODE_Q);
    MAP_SC(0x13, 0, input, PAL_SCANCODE_R);
    MAP_SC(0x1F, 0, input, PAL_SCANCODE_S);
    MAP_SC(0x14, 0, input, PAL_SCANCODE_T);
    MAP_SC(0x16, 0, input, PAL_SCANCODE_U);
    MAP_SC(0x2F, 0, input, PAL_SCANCODE_V);
    MAP_SC(0x11, 0, input, PAL_SCANCODE_W);
    MAP_SC(0x2D, 0, input, PAL_SCANCODE_X);
    MAP_SC(0x15, 0, input, PAL_SCANCODE_Y);
    MAP_SC(0x2C, 0, input, PAL_SCANCODE_Z);

    // Numbers (top row)
    MAP_SC(0x0B, 0, input, PAL_SCANCODE_0);
    MAP_SC(0x02, 0, input, PAL_SCANCODE_1);
    MAP_SC(0x03, 0, input, PAL_SCANCODE_2);
    MAP_SC(0x04, 0, input, PAL_SCANCODE_3);
    MAP_SC(0x05, 0, input, PAL_SCANCODE_4);
    MAP_SC(0x06, 0, input, PAL_SCANCODE_5);
    MAP_SC(0x07, 0, input, PAL_SCANCODE_6);
    MAP_SC(0x08, 0, input, PAL_SCANCODE_7);
    MAP_SC(0x09, 0, input, PAL_SCANCODE_8);
    MAP_SC(0x0A, 0, input, PAL_SCANCODE_9);

    // Function keys
    MAP_SC(0x3B, 0, input, PAL_SCANCODE_F1);
    MAP_SC(0x3C, 0, input, PAL_SCANCODE_F2);
    MAP_SC(0x3D, 0, input, PAL_SCANCODE_F3);
    MAP_SC(0x3E, 0, input, PAL_SCANCODE_F4);
    MAP_SC(0x3F, 0, input, PAL_SCANCODE_F5);
    MAP_SC(0x40, 0, input, PAL_SCANCODE_F6);
    MAP_SC(0x41, 0, input, PAL_SCANCODE_F7);
    MAP_SC(0x42, 0, input, PAL_SCANCODE_F8);
    MAP_SC(0x43, 0, input, PAL_SCANCODE_F9);
    MAP_SC(0x44, 0, input, PAL_SCANCODE_F10);
    MAP_SC(0x57, 0, input, PAL_SCANCODE_F11);
    MAP_SC(0x58, 0, input, PAL_SCANCODE_F12);

    // Control
    MAP_SC(0x01, 0, input, PAL_SCANCODE_ESCAPE);
    MAP_SC(0x1C, 0, input, PAL_SCANCODE_ENTER);
    MAP_SC(0x0F, 0, input, PAL_SCANCODE_TAB);
    MAP_SC(0x0E, 0, input, PAL_SCANCODE_BACKSPACE);
    MAP_SC(0x39, 0, input, PAL_SCANCODE_SPACE);
    MAP_SC(0x3A, 0, input, PAL_SCANCODE_CAPSLOCK);
    MAP_SC(0x45, 0, input, PAL_SCANCODE_NUMLOCK);
    MAP_SC(0x46, 0, input, PAL_SCANCODE_SCROLLLOCK);
    MAP_SC(0x2A, 0, input, PAL_SCANCODE_LSHIFT);
    MAP_SC(0x36, 0, input, PAL_SCANCODE_RSHIFT);
    MAP_SC(0x1D, 0, input, PAL_SCANCODE_LCTRL);
    MAP_SC(0x1D, 1, input, PAL_SCANCODE_RCTRL);
    MAP_SC(0x38, 0, input, PAL_SCANCODE_LALT);
    MAP_SC(0x38, 1, input, PAL_SCANCODE_RALT);

    // Arrows
    MAP_SC(0x4B, 1, input, PAL_SCANCODE_LEFT);
    MAP_SC(0x4D, 1, input, PAL_SCANCODE_RIGHT);
    MAP_SC(0x48, 1, input, PAL_SCANCODE_UP);
    MAP_SC(0x50, 1, input, PAL_SCANCODE_DOWN);

    // // Navigation
    // input->scancodes[0x152] = PAL_SCANCODE_INSERT;
    // input->scancodes[0x153] = PAL_SCANCODE_DELETE;
    // input->scancodes[0x147] = PAL_SCANCODE_HOME;
    // input->scancodes[0x14F] = PAL_SCANCODE_END;
    // input->scancodes[0x149] = PAL_SCANCODE_PAGEUP;
    // input->scancodes[0x151] = PAL_SCANCODE_PAGEDOWN;

    // // Keypad
    // input->scancodes[0x052] = PAL_SCANCODE_KP_0;
    // input->scancodes[0x04F] = PAL_SCANCODE_KP_1;
    // input->scancodes[0x050] = PAL_SCANCODE_KP_2;
    // input->scancodes[0x051] = PAL_SCANCODE_KP_3;
    // input->scancodes[0x04B] = PAL_SCANCODE_KP_4;
    // input->scancodes[0x04C] = PAL_SCANCODE_KP_5;
    //input->scancodes[0x04D] = PAL_SCANCODE_KP_6;
    // input->scancodes[0x047] = PAL_SCANCODE_KP_7;
    // input->scancodes[0x048] = PAL_SCANCODE_KP_8;
    // input->scancodes[0x049] = PAL_SCANCODE_KP_9;
    // input->scancodes[0x11C] = PAL_SCANCODE_KP_ENTER; // 1
    // input->scancodes[0x04E] = PAL_SCANCODE_KP_PLUS;
    // input->scancodes[0x04A] = PAL_SCANCODE_KP_MINUS;
    // input->scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    // input->scancodes[0x135] = PAL_SCANCODE_KP_DIVIDE;
    // input->scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;
    // input->scancodes[0x059] = PAL_SCANCODE_KP_EQUAL;

    // // Misc
    // input->scancodes[0x137] = PAL_SCANCODE_PRINTSCREEN;
    // input->scancodes[0x045] = PAL_SCANCODE_PAUSE;
    // input->scancodes[0x15D] = PAL_SCANCODE_MENU;
    // input->scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    // input->scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    // input->scancodes[0x033] = PAL_SCANCODE_COMMA;
    // input->scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    // input->scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    // input->scancodes[0x00C] = PAL_SCANCODE_MINUS;
    // input->scancodes[0x034] = PAL_SCANCODE_PERIOD;
    // input->scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    // input->scancodes[0x035] = PAL_SCANCODE_SLASH;
    // input->scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    // input->scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    // input->scancodes[0x15B] = PAL_SCANCODE_LSUPER;
    // input->scancodes[0x15C] = PAL_SCANCODE_RSUPER;
}
