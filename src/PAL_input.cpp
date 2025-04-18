
#include "PAL_pch.h"
#include "PAL/PAL_input.h"

static std::string s_ActionNames[PAL_Actions_Max + 1] = {};
static std::string s_ButtonNames[PAL_Buttons_Max + 1] = {};
static std::string s_KeyNames[PAL_Keys_Max + 1] = {};

void _InitInput()
{
    s_ActionNames[PAL_Actions_Release] = "Release";
    s_ActionNames[PAL_Actions_Press] = "Press";
    s_ActionNames[PAL_Actions_Repeat] = "Repeat";

    s_ButtonNames[PAL_Buttons_Left] = "Button Left";
    s_ButtonNames[PAL_Buttons_Right] = "Button Right";
    s_ButtonNames[PAL_Buttons_Middle] = "Button Middle";

    s_KeyNames[PAL_Keys_A] = "Key A";
    s_KeyNames[PAL_Keys_B] = "Key B";
    s_KeyNames[PAL_Keys_C] = "Key C";
    s_KeyNames[PAL_Keys_D] = "Key D";
    s_KeyNames[PAL_Keys_E] = "Key E";
    s_KeyNames[PAL_Keys_F] = "Key F";
    s_KeyNames[PAL_Keys_G] = "Key G";
    s_KeyNames[PAL_Keys_H] = "Key H";
    s_KeyNames[PAL_Keys_I] = "Key I";
    s_KeyNames[PAL_Keys_J] = "Key J";
    s_KeyNames[PAL_Keys_K] = "Key K";
    s_KeyNames[PAL_Keys_L] = "Key L";
    s_KeyNames[PAL_Keys_M] = "Key M";
    s_KeyNames[PAL_Keys_N] = "Key N";
    s_KeyNames[PAL_Keys_O] = "Key O";
    s_KeyNames[PAL_Keys_P] = "Key P";
    s_KeyNames[PAL_Keys_Q] = "Key Q";
    s_KeyNames[PAL_Keys_R] = "Key R";
    s_KeyNames[PAL_Keys_S] = "Key S";
    s_KeyNames[PAL_Keys_T] = "Key T";
    s_KeyNames[PAL_Keys_U] = "Key U";
    s_KeyNames[PAL_Keys_V] = "Key V";
    s_KeyNames[PAL_Keys_W] = "Key W";
    s_KeyNames[PAL_Keys_X] = "Key X";
    s_KeyNames[PAL_Keys_Y] = "Key Y";
    s_KeyNames[PAL_Keys_Z] = "Key Z";

    s_KeyNames[PAL_Keys_Escape] = "Key Escape";
    s_KeyNames[PAL_Keys_Enter] = "Key Enter";
    s_KeyNames[PAL_Keys_Tab] = "Key Tab";
    s_KeyNames[PAL_Keys_Backspace] = "Key Backspace";
    s_KeyNames[PAL_Keys_Insert] = "Key Insert";
    s_KeyNames[PAL_Keys_Delete] = "Key Delete";
    s_KeyNames[PAL_Keys_Right] = "Key Right";
    s_KeyNames[PAL_Keys_Left] = "Key Left";
    s_KeyNames[PAL_Keys_Down] = "Key Down";
    s_KeyNames[PAL_Keys_Up] = "Key Up";
    s_KeyNames[PAL_Keys_PageUp] = "Key PageUp";
    s_KeyNames[PAL_Keys_PageDown] = "Key PageDown";
    s_KeyNames[PAL_Keys_Home] = "Key Home";
    s_KeyNames[PAL_Keys_End] = "Key End";
    s_KeyNames[PAL_Keys_CapsLock] = "Key CapsLock";
    s_KeyNames[PAL_Keys_ScrollLock] = "Key ScrollLock";
    s_KeyNames[PAL_Keys_NumLock] = "Key NumLock";
    s_KeyNames[PAL_Keys_PAL_intScreen] = "Key PAL_u16Screen";
    s_KeyNames[PAL_Keys_Pause] = "Key Pause";
    s_KeyNames[PAL_Keys_F1] = "Key F1";
    s_KeyNames[PAL_Keys_F2] = "Key F2";
    s_KeyNames[PAL_Keys_F3] = "Key F3";
    s_KeyNames[PAL_Keys_F4] = "Key F4";
    s_KeyNames[PAL_Keys_F5] = "Key F5";
    s_KeyNames[PAL_Keys_F6] = "Key F6";
    s_KeyNames[PAL_Keys_F7] = "Key F7";
    s_KeyNames[PAL_Keys_F8] = "Key F8";
    s_KeyNames[PAL_Keys_F9] = "Key F9";
    s_KeyNames[PAL_Keys_F10] = "Key F10";
    s_KeyNames[PAL_Keys_F11] = "Key F11";
    s_KeyNames[PAL_Keys_F12] = "Key F12";

    s_KeyNames[PAL_Keys_LeftShift] = "Key LeftShift";
    s_KeyNames[PAL_Keys_LeftControl] = "Key LeftControl";
    s_KeyNames[PAL_Keys_LeftAlt] = "Key LeftAlt";
    s_KeyNames[PAL_Keys_LeftSuper] = "Key LeftSuper";
    s_KeyNames[PAL_Keys_LeftBracket] = "Key LeftBracket";
    s_KeyNames[PAL_Keys_RightShift] = "Key RightShift";
    s_KeyNames[PAL_Keys_RightControl] = "Key RightControl";
    s_KeyNames[PAL_Keys_RightAlt] = "Key RightAlt";
    s_KeyNames[PAL_Keys_RightSuper] = "Key RightSuper";
    s_KeyNames[PAL_Keys_RightBracket] = "Key RightBracket";

    s_KeyNames[PAL_Keys_Menu] = "Key Menu";
    s_KeyNames[PAL_Keys_Backslash] = "Key Backslash";
    s_KeyNames[PAL_Keys_GraveAccent] = "Key GraveAccent";
    s_KeyNames[PAL_Keys_SemiColon] = "Key SemiColon";
    s_KeyNames[PAL_Keys_Equal] = "Key Equal";

    s_KeyNames[PAL_Keys_Space] = "Key Space";
    s_KeyNames[PAL_Keys_Apostrophe] = "Key Apostrophe";
    s_KeyNames[PAL_Keys_Comma] = "Key Comma";
    s_KeyNames[PAL_Keys_Minus] = "Key Minus";
    s_KeyNames[PAL_Keys_Period] = "Key Period";
    s_KeyNames[PAL_Keys_Slash] = "Key Slash";

    s_KeyNames[PAL_Keys_0] = "Key 0";
    s_KeyNames[PAL_Keys_1] = "Key 1";
    s_KeyNames[PAL_Keys_2] = "Key 2";
    s_KeyNames[PAL_Keys_3] = "Key 3";
    s_KeyNames[PAL_Keys_4] = "Key 4";
    s_KeyNames[PAL_Keys_5] = "Key 5";
    s_KeyNames[PAL_Keys_6] = "Key 6";
    s_KeyNames[PAL_Keys_7] = "Key 7";
    s_KeyNames[PAL_Keys_8] = "Key 8";
    s_KeyNames[PAL_Keys_9] = "Key 9";

    s_KeyNames[PAL_Keys_P0] = "Key P0";
    s_KeyNames[PAL_Keys_P1] = "Key P1";
    s_KeyNames[PAL_Keys_P2] = "Key P2";
    s_KeyNames[PAL_Keys_P3] = "Key P3";
    s_KeyNames[PAL_Keys_P4] = "Key P4";
    s_KeyNames[PAL_Keys_P5] = "Key P5";
    s_KeyNames[PAL_Keys_P6] = "Key P6";
    s_KeyNames[PAL_Keys_P7] = "Key P7";
    s_KeyNames[PAL_Keys_P8] = "Key P8";
    s_KeyNames[PAL_Keys_P9] = "Key P9";
    s_KeyNames[PAL_Keys_PDecimal] = "Key PDecimal";
    s_KeyNames[PAL_Keys_PDivide] = "Key PDivide";
    s_KeyNames[PAL_Keys_PMultiply] = "Key PMultiply";
    s_KeyNames[PAL_Keys_PSubtract] = "Key PSubtract";
    s_KeyNames[PAL_Keys_PAdd] = "Key PAdd";
    s_KeyNames[PAL_Keys_PEnter] = "Key PEnter";
    s_KeyNames[PAL_Keys_PEqual] = "Key PEqual";
}

const char* PAL_GetKeyName(u32 key)
{
    CHECK_ERR(key > 0 || key < PAL_Keys_Max, "Invalid key", return nullptr);
    return s_KeyNames[key].c_str();
}

const char* PAL_GetButtonName(u32 button)
{
    CHECK_ERR(button > 0 || button < PAL_Buttons_Max, "Invalid button", return nullptr);
    return s_ButtonNames[button].c_str();
}

const char* PAL_GetActionName(u32 action)
{
    CHECK_ERR(action > 0 || action < PAL_Actions_Max, "Invalid action", return nullptr);
    return s_ActionNames[action].c_str();
}