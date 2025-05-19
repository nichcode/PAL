
#include "PAL_pch.h"
#include "PAL_internal.h"
#include "PAL/PAL_input.h"

static std::string s_ActionNames[PAL_ACTION_MAX + 1] = {};
static std::string s_MouseButtonNames[PAL_MOUSE_BUTTON_MAX + 1] = {};
static std::string s_KeyNames[PAL_KEY_MAX + 1] = {};

void PAL_InitInput()
{
    s_KeyNames[PAL_KEY_A] = "Key A";
    s_KeyNames[PAL_KEY_B] = "Key B";
    s_KeyNames[PAL_KEY_C] = "Key C";
    s_KeyNames[PAL_KEY_D] = "Key D";
    s_KeyNames[PAL_KEY_E] = "Key E";
    s_KeyNames[PAL_KEY_F] = "Key F";
    s_KeyNames[PAL_KEY_G] = "Key G";
    s_KeyNames[PAL_KEY_H] = "Key H";
    s_KeyNames[PAL_KEY_I] = "Key I";
    s_KeyNames[PAL_KEY_J] = "Key J";
    s_KeyNames[PAL_KEY_K] = "Key K";
    s_KeyNames[PAL_KEY_L] = "Key L";
    s_KeyNames[PAL_KEY_M] = "Key M";
    s_KeyNames[PAL_KEY_N] = "Key N";
    s_KeyNames[PAL_KEY_O] = "Key O";
    s_KeyNames[PAL_KEY_P] = "Key P";
    s_KeyNames[PAL_KEY_Q] = "Key Q";
    s_KeyNames[PAL_KEY_R] = "Key R";
    s_KeyNames[PAL_KEY_S] = "Key S";
    s_KeyNames[PAL_KEY_T] = "Key T";
    s_KeyNames[PAL_KEY_U] = "Key U";
    s_KeyNames[PAL_KEY_V] = "Key V";
    s_KeyNames[PAL_KEY_W] = "Key W";
    s_KeyNames[PAL_KEY_X] = "Key X";
    s_KeyNames[PAL_KEY_Y] = "Key Y";
    s_KeyNames[PAL_KEY_Z] = "Key Z";

    s_KeyNames[PAL_KEY_ESCAPE] = "Key Escape";
    s_KeyNames[PAL_KEY_ENTER] = "Key Enter";
    s_KeyNames[PAL_KEY_TAB] = "Key Tab";
    s_KeyNames[PAL_KEY_BACK_SPACE] = "Key Backspace";
    s_KeyNames[PAL_KEY_INSERT] = "Key Insert";
    s_KeyNames[PAL_KEY_DELETE] = "Key Delete";
    s_KeyNames[PAL_KEY_RIGHT] = "Key Right";
    s_KeyNames[PAL_KEY_LEFT] = "Key Left";
    s_KeyNames[PAL_KEY_DOWN] = "Key Down";
    s_KeyNames[PAL_KEY_UP] = "Key Up";
    s_KeyNames[PAL_KEY_PAGE_UP] = "Key PageUp";
    s_KeyNames[PAL_KEY_PAGE_DOWN] = "Key PageDown";
    s_KeyNames[PAL_KEY_HOME] = "Key Home";
    s_KeyNames[PAL_KEY_END] = "Key End";
    s_KeyNames[PAL_KEY_CAPS_LOCK] = "Key CapsLock";
    s_KeyNames[PAL_KEY_SCROLL_LOCK] = "Key ScrollLock";
    s_KeyNames[PAL_KEY_NUM_LOCK] = "Key NumLock";
    s_KeyNames[PAL_KEY_PAUSE] = "Key Pause";
    s_KeyNames[PAL_KEY_F1] = "Key F1";
    s_KeyNames[PAL_KEY_F2] = "Key F2";
    s_KeyNames[PAL_KEY_F3] = "Key F3";
    s_KeyNames[PAL_KEY_F4] = "Key F4";
    s_KeyNames[PAL_KEY_F5] = "Key F5";
    s_KeyNames[PAL_KEY_F6] = "Key F6";
    s_KeyNames[PAL_KEY_F7] = "Key F7";
    s_KeyNames[PAL_KEY_F8] = "Key F8";
    s_KeyNames[PAL_KEY_F9] = "Key F9";
    s_KeyNames[PAL_KEY_F10] = "Key F10";
    s_KeyNames[PAL_KEY_F11] = "Key F11";
    s_KeyNames[PAL_KEY_F12] = "Key F12";

    s_KeyNames[PAL_KEY_LEFT_SHIFT] = "Key LeftShift";
    s_KeyNames[PAL_KEY_LEFT_CONTROL] = "Key LeftControl";
    s_KeyNames[PAL_KEY_LEFT_ALT] = "Key LeftAlt";
    s_KeyNames[PAL_KEY_LEFT_SUPER] = "Key LeftSuper";
    s_KeyNames[PAL_KEY_LEFT_BRACKET] = "Key LeftBracket";
    s_KeyNames[PAL_KEY_RIGHT_SHIFT] = "Key RightShift";
    s_KeyNames[PAL_KEY_RIGHT_CONTROL] = "Key RightControl";
    s_KeyNames[PAL_KEY_RIGHT_ALT] = "Key RightAlt";
    s_KeyNames[PAL_KEY_RIGHT_SUPER] = "Key RightSuper";
    s_KeyNames[PAL_KEY_RIGHT_BRACKET] = "Key RightBracket";

    s_KeyNames[PAL_KEY_MENU] = "Key Menu";
    s_KeyNames[PAL_KEY_BACK_SLASH] = "Key Backslash";
    s_KeyNames[PAL_KEY_GRAVE_ACCENT] = "Key GraveAccent";
    s_KeyNames[PAL_KEY_SEMi_COLON] = "Key SemiColon";
    s_KeyNames[PAL_KEY_EQUAL] = "Key Equal";

    s_KeyNames[PAL_KEY_SPACE] = "Key Space";
    s_KeyNames[PAL_KEY_APOSTROPHE] = "Key Apostrophe";
    s_KeyNames[PAL_KEY_COMMA] = "Key Comma";
    s_KeyNames[PAL_KEY_MINUS] = "Key Minus";
    s_KeyNames[PAL_KEY_PERIOD] = "Key Period";
    s_KeyNames[PAL_KEY_SLASH] = "Key Slash";

    s_KeyNames[PAL_KEY_0] = "Key 0";
    s_KeyNames[PAL_KEY_1] = "Key 1";
    s_KeyNames[PAL_KEY_2] = "Key 2";
    s_KeyNames[PAL_KEY_3] = "Key 3";
    s_KeyNames[PAL_KEY_4] = "Key 4";
    s_KeyNames[PAL_KEY_5] = "Key 5";
    s_KeyNames[PAL_KEY_6] = "Key 6";
    s_KeyNames[PAL_KEY_7] = "Key 7";
    s_KeyNames[PAL_KEY_8] = "Key 8";
    s_KeyNames[PAL_KEY_9] = "Key 9";

    s_MouseButtonNames[PAL_MOUSE_BUTTON_LEFT] = "MouseButton Left";
    s_MouseButtonNames[PAL_MOUSE_BUTTON_RIGHT] = "MouseButton Right";
    s_MouseButtonNames[PAL_MOUSE_BUTTON_MIDDLE] = "MouseButton Middle";

    s_ActionNames[PAL_RELEASE] = "Release";
    s_ActionNames[PAL_PRESS] = "Press";
    s_ActionNames[PAL_REPEAT] = "Repeat";
}

const char* PAL_GetKeyName(u32 key)
{
    b8 valid = key >=0 || key < PAL_KEY_MAX;
    if (!valid) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid key");
        return "";
    }
    return s_KeyNames[key].c_str();
}

const char* PAL_GetMouseButtonName(u32 mouse_button)
{
    b8 valid = mouse_button >=0 || mouse_button < PAL_MOUSE_BUTTON_MAX;
    if (!valid) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid mouse button");
        return "";
    }
    return s_MouseButtonNames[mouse_button].c_str();
}

const char* PAL_GetActionName(u32 action)
{
    b8 valid = action >=0 || action < PAL_ACTION_MAX;
    if (!valid) {
        PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid action");
        return "";
    }
    return s_ActionNames[action].c_str();
}