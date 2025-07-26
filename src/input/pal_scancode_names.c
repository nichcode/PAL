
#include "pal_pch.h"
#include "pal_input_internal.h"

// ==================================================
// Internal API
// ==================================================

void palMapScancodeNames(PalInput input) {

    // Letters
    input->scancodeNames[PAL_SCANCODE_A] = "Scancode A";
    input->scancodeNames[PAL_SCANCODE_B] = "Scancode B";
    input->scancodeNames[PAL_SCANCODE_C] = "Scancode C";
    input->scancodeNames[PAL_SCANCODE_D] = "Scancode D";
    input->scancodeNames[PAL_SCANCODE_E] = "Scancode E";
    input->scancodeNames[PAL_SCANCODE_F] = "Scancode F";
    input->scancodeNames[PAL_SCANCODE_G] = "Scancode G";
    input->scancodeNames[PAL_SCANCODE_H] = "Scancode H";
    input->scancodeNames[PAL_SCANCODE_I] = "Scancode I";
    input->scancodeNames[PAL_SCANCODE_J] = "Scancode J";
    input->scancodeNames[PAL_SCANCODE_K] = "Scancode K";
    input->scancodeNames[PAL_SCANCODE_L] = "Scancode L";
    input->scancodeNames[PAL_SCANCODE_M] = "Scancode M";
    input->scancodeNames[PAL_SCANCODE_N] = "Scancode N";
    input->scancodeNames[PAL_SCANCODE_O] = "Scancode O";
    input->scancodeNames[PAL_SCANCODE_P] = "Scancode P";
    input->scancodeNames[PAL_SCANCODE_Q] = "Scancode Q";
    input->scancodeNames[PAL_SCANCODE_R] = "Scancode R";
    input->scancodeNames[PAL_SCANCODE_S] = "Scancode S";
    input->scancodeNames[PAL_SCANCODE_T] = "Scancode T";
    input->scancodeNames[PAL_SCANCODE_U] = "Scancode U";
    input->scancodeNames[PAL_SCANCODE_V] = "Scancode V";
    input->scancodeNames[PAL_SCANCODE_W] = "Scancode W";
    input->scancodeNames[PAL_SCANCODE_X] = "Scancode X";
    input->scancodeNames[PAL_SCANCODE_Y] = "Scancode Y";
    input->scancodeNames[PAL_SCANCODE_Z] = "Scancode Z";

    // Numbers (top row)
    input->scancodeNames[PAL_SCANCODE_0] = "Scancode 0";
    input->scancodeNames[PAL_SCANCODE_1] = "Scancode 1";
    input->scancodeNames[PAL_SCANCODE_2] = "Scancode 2";
    input->scancodeNames[PAL_SCANCODE_3] = "Scancode 3";
    input->scancodeNames[PAL_SCANCODE_4] = "Scancode 4";
    input->scancodeNames[PAL_SCANCODE_5] = "Scancode 5";
    input->scancodeNames[PAL_SCANCODE_6] = "Scancode 6";
    input->scancodeNames[PAL_SCANCODE_7] = "Scancode 7";
    input->scancodeNames[PAL_SCANCODE_8] = "Scancode 8";
    input->scancodeNames[PAL_SCANCODE_9] = "Scancode 9";

    // Function keys
    input->scancodeNames[PAL_SCANCODE_F1] = "Scancode F1";
    input->scancodeNames[PAL_SCANCODE_F2] = "Scancode F2";
    input->scancodeNames[PAL_SCANCODE_F3] = "Scancode F3";
    input->scancodeNames[PAL_SCANCODE_F4] = "Scancode F4";
    input->scancodeNames[PAL_SCANCODE_F5] = "Scancode F5";
    input->scancodeNames[PAL_SCANCODE_F6] = "Scancode F6";
    input->scancodeNames[PAL_SCANCODE_F7] = "Scancode F7";
    input->scancodeNames[PAL_SCANCODE_F8] = "Scancode F8";
    input->scancodeNames[PAL_SCANCODE_F9] = "Scancode F9";
    input->scancodeNames[PAL_SCANCODE_F10] = "Scancode F10";
    input->scancodeNames[PAL_SCANCODE_F11] = "Scancode F11";
    input->scancodeNames[PAL_SCANCODE_F12] = "Scancode F12";

    // Control
    input->scancodeNames[PAL_SCANCODE_ESCAPE] = "Scancode Escape";
    input->scancodeNames[PAL_SCANCODE_ENTER] = "Scancode Enter";
    input->scancodeNames[PAL_SCANCODE_TAB] = "Scancode Tab";
    input->scancodeNames[PAL_SCANCODE_BACKSPACE] = "Scancode Backspace";
    input->scancodeNames[PAL_SCANCODE_SPACE] = "Scancode Space";
    input->scancodeNames[PAL_SCANCODE_CAPSLOCK] = "Scancode Capslock";
    input->scancodeNames[PAL_SCANCODE_NUMLOCK] = "Scancode Numlock";
    input->scancodeNames[PAL_SCANCODE_SCROLLLOCK] = "Scancode Scrolllock";
    input->scancodeNames[PAL_SCANCODE_LSHIFT] = "Scancode LShift";
    input->scancodeNames[PAL_SCANCODE_RSHIFT] = "Scancode RShift";
    input->scancodeNames[PAL_SCANCODE_LCTRL] = "Scancode LCtrl";
    input->scancodeNames[PAL_SCANCODE_RCTRL] = "Scancode RCtrl";
    input->scancodeNames[PAL_SCANCODE_LALT] = "Scancode LAlt";
    input->scancodeNames[PAL_SCANCODE_RALT] = "Scancode RAlt";

    // Arrows
    input->scancodeNames[PAL_SCANCODE_LEFT] = "Scancode Left";
    input->scancodeNames[PAL_SCANCODE_RIGHT] = "Scancode Right";
    input->scancodeNames[PAL_SCANCODE_UP] = "Scancode Up";
    input->scancodeNames[PAL_SCANCODE_DOWN] = "Scancode Down";

    // Navigation
    input->scancodeNames[PAL_SCANCODE_INSERT] = "Scancode Insert";
    input->scancodeNames[PAL_SCANCODE_DELETE] = "Scancode Delete";
    input->scancodeNames[PAL_SCANCODE_HOME] = "Scancode Home";
    input->scancodeNames[PAL_SCANCODE_END] = "Scancode End";
    input->scancodeNames[PAL_SCANCODE_PAGEUP] = "Scancode Pageup";
    input->scancodeNames[PAL_SCANCODE_PAGEDOWN] = "Scancode Pagedown";

    // Keypad
    input->scancodeNames[PAL_SCANCODE_KP_0] = "Scancode KP_0";
    input->scancodeNames[PAL_SCANCODE_KP_1] = "Scancode KP_1";
    input->scancodeNames[PAL_SCANCODE_KP_2] = "Scancode KP_2";
    input->scancodeNames[PAL_SCANCODE_KP_3] = "Scancode KP_3";
    input->scancodeNames[PAL_SCANCODE_KP_4] = "Scancode KP_4";
    input->scancodeNames[PAL_SCANCODE_KP_5] = "Scancode KP_5";
    input->scancodeNames[PAL_SCANCODE_KP_6] = "Scancode KP_6";
    input->scancodeNames[PAL_SCANCODE_KP_7] = "Scancode KP_7";
    input->scancodeNames[PAL_SCANCODE_KP_8] = "Scancode KP_8";
    input->scancodeNames[PAL_SCANCODE_KP_9] = "Scancode KP_9";
    input->scancodeNames[PAL_SCANCODE_KP_ENTER] = "Scancode KP_Enter";
    input->scancodeNames[PAL_SCANCODE_KP_ADD] = "Scancode KP_Add";
    input->scancodeNames[PAL_SCANCODE_KP_SUBTRACT] = "Scancode KP_Subtract";
    input->scancodeNames[PAL_SCANCODE_KP_MULTIPLY] = "Scancode KP_Multiply";
    input->scancodeNames[PAL_SCANCODE_KP_DIVIDE] = "Scancode KP_Divide";
    input->scancodeNames[PAL_SCANCODE_KP_DECIMAL] = "Scancode KP_Decimal";
    input->scancodeNames[PAL_SCANCODE_KP_EQUAL] = "Scancode KP_Equal";

    // Misc
    input->scancodeNames[PAL_SCANCODE_PRINTSCREEN] = "Scancode Printscreen";
    input->scancodeNames[PAL_SCANCODE_PAUSE] = "Scancode Pause";
    input->scancodeNames[PAL_SCANCODE_MENU] = "Scancode Menu";
    input->scancodeNames[PAL_SCANCODE_APOSTROPHE] = "Scancode APOSTROPHE";
    input->scancodeNames[PAL_SCANCODE_BACKSLASH] = "Scancode Backslash";
    input->scancodeNames[PAL_SCANCODE_COMMA] = "Scancode Comma";
    input->scancodeNames[PAL_SCANCODE_EQUAL] = "Scancode Equal";
    input->scancodeNames[PAL_SCANCODE_GRAVEACCENT] = "Scancode Graveaccent";
    input->scancodeNames[PAL_SCANCODE_SUBTRACT] = "Scancode Subtract";
    input->scancodeNames[PAL_SCANCODE_PERIOD] = "Scancode Period";
    input->scancodeNames[PAL_SCANCODE_SEMICOLON] = "Scancode Semicolon";
    input->scancodeNames[PAL_SCANCODE_SLASH] = "Scancode Slash";
    input->scancodeNames[PAL_SCANCODE_LBRACKET] = "Scancode LBracket";
    input->scancodeNames[PAL_SCANCODE_RBRACKET] = "Scancode RBracket";
    input->scancodeNames[PAL_SCANCODE_LSUPER] = "Scancode LSuper";
    input->scancodeNames[PAL_SCANCODE_RSUPER] = "Scancode RSuper";
}