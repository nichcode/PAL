
#pragma once

#include "PAL_defines.h"

enum PAL_Keys
{
    PAL_KEY_A,
    PAL_KEY_B,
    PAL_KEY_C,
    PAL_KEY_D,
    PAL_KEY_E,
    PAL_KEY_F,
    PAL_KEY_G,
    PAL_KEY_H,
    PAL_KEY_I,
    PAL_KEY_J,
    PAL_KEY_K,
    PAL_KEY_L,
    PAL_KEY_M,
    PAL_KEY_N,
    PAL_KEY_O,
    PAL_KEY_P,
    PAL_KEY_Q,
    PAL_KEY_R,
    PAL_KEY_S,
    PAL_KEY_T,
    PAL_KEY_U,
    PAL_KEY_V,
    PAL_KEY_W,
    PAL_KEY_X,
    PAL_KEY_Y,
    PAL_KEY_Z,

    PAL_KEY_ESCAPE,
    PAL_KEY_ENTER,
    PAL_KEY_TAB,
    PAL_KEY_BACK_SPACE,
    PAL_KEY_INSERT,
    PAL_KEY_DELETE,
    PAL_KEY_RIGHT,
    PAL_KEY_LEFT,
    PAL_KEY_DOWN,
    PAL_KEY_UP,
    PAL_KEY_PAGE_UP,
    PAL_KEY_PAGE_DOWN,
    PAL_KEY_HOME,
    PAL_KEY_END,
    PAL_KEY_CAPS_LOCK,
    PAL_KEY_SCROLL_LOCK,
    PAL_KEY_NUM_LOCK,
    PAL_KEY_PAUSE,

    PAL_KEY_F1,
    PAL_KEY_F2,
    PAL_KEY_F3,
    PAL_KEY_F4,
    PAL_KEY_F5,
    PAL_KEY_F6,
    PAL_KEY_F7,
    PAL_KEY_F8,
    PAL_KEY_F9,
    PAL_KEY_F10,
    PAL_KEY_F11,
    PAL_KEY_F12,

    PAL_KEY_LEFT_SHIFT,
    PAL_KEY_LEFT_CONTROL,
    PAL_KEY_LEFT_ALT,
    PAL_KEY_LEFT_SUPER,
    PAL_KEY_RIGHT_SHIFT,
    PAL_KEY_RIGHT_CONTROL,
    PAL_KEY_RIGHT_ALT,
    PAL_KEY_RIGHT_SUPER,
    PAL_KEY_MENU,

    PAL_KEY_LEFT_BRACKET, /* [ */
    PAL_KEY_BACK_SLASH, /* \ */
    PAL_KEY_RIGHT_BRACKET,  /* ] */
    PAL_KEY_GRAVE_ACCENT,  /* ` */
    PAL_KEY_SEMi_COLON, /* ; */
    PAL_KEY_EQUAL, /* = */

    PAL_KEY_SPACE,
    PAL_KEY_APOSTROPHE, /* ' */
    PAL_KEY_COMMA, /* , */
    PAL_KEY_MINUS, /* - */
    PAL_KEY_PERIOD, /* . */
    PAL_KEY_SLASH, /* / */

    PAL_KEY_0, /* 0 */
    PAL_KEY_1, /* 1 */
    PAL_KEY_2, /* 2 */
    PAL_KEY_3, /* 3 */
    PAL_KEY_4, /* 4 */
    PAL_KEY_5, /* 5 */
    PAL_KEY_6, /* 6 */
    PAL_KEY_7, /* 7 */
    PAL_KEY_8, /* 8 */
    PAL_KEY_9, /* 9 */

    PAL_KEY_MAX
};

enum PAL_MouseButtons
{
    PAL_MOUSE_BUTTON_LEFT,
    PAL_MOUSE_BUTTON_RIGHT,
    PAL_MOUSE_BUTTON_MIDDLE,

    PAL_MOUSE_BUTTON_MAX
};

enum PAL_Actions
{
    PAL_RELEASE,
    PAL_PRESS,
    PAL_REPEAT,

    PAL_ACTION_MAX
};

PAL_API b8 PAL_GetKeyState(u32 key);
PAL_API b8 PAL_GetMouseButtonState(u32 mouse_button);

PAL_API const char* PAL_GetKeyName(u32 key);
PAL_API const char* PAL_GetMouseButtonName(u32 mouse_button);
PAL_API const char* PAL_GetActionName(u32 action);