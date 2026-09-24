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

#ifndef PAL_VIDEO_KEYBOARD_H
#define PAL_VIDEO_KEYBOARD_H

#include "pal2/core/defines.h"

/**
 * @defgroup keycodes Keycodes
 * @brief Keycodes
 * 
 * @{
 */
#define PAL_KEYCODE_UNKNOWN 0
#define PAL_KEYCODE_A 1
#define PAL_KEYCODE_B 2
#define PAL_KEYCODE_C 3
#define PAL_KEYCODE_D 4
#define PAL_KEYCODE_E 5
#define PAL_KEYCODE_F 6
#define PAL_KEYCODE_G 7
#define PAL_KEYCODE_H 8
#define PAL_KEYCODE_I 9
#define PAL_KEYCODE_J 10
#define PAL_KEYCODE_K 11
#define PAL_KEYCODE_L 12
#define PAL_KEYCODE_M 13
#define PAL_KEYCODE_N 14
#define PAL_KEYCODE_O 15
#define PAL_KEYCODE_P 16
#define PAL_KEYCODE_Q 17
#define PAL_KEYCODE_R 18
#define PAL_KEYCODE_S 19
#define PAL_KEYCODE_T 20
#define PAL_KEYCODE_U 21
#define PAL_KEYCODE_V 22
#define PAL_KEYCODE_W 23
#define PAL_KEYCODE_X 24
#define PAL_KEYCODE_Y 25
#define PAL_KEYCODE_Z 26
#define PAL_KEYCODE_0 27
#define PAL_KEYCODE_1 28
#define PAL_KEYCODE_2 29
#define PAL_KEYCODE_3 30
#define PAL_KEYCODE_4 31
#define PAL_KEYCODE_5 32
#define PAL_KEYCODE_6 33
#define PAL_KEYCODE_7 34
#define PAL_KEYCODE_8 35
#define PAL_KEYCODE_9 36
#define PAL_KEYCODE_F1 37
#define PAL_KEYCODE_F2 38
#define PAL_KEYCODE_F3 39
#define PAL_KEYCODE_F4 40
#define PAL_KEYCODE_F5 41
#define PAL_KEYCODE_F6 42
#define PAL_KEYCODE_F7 43
#define PAL_KEYCODE_F8 44
#define PAL_KEYCODE_F9 45
#define PAL_KEYCODE_F10 46
#define PAL_KEYCODE_F11 47
#define PAL_KEYCODE_F12 48
#define PAL_KEYCODE_ESCAPE 49
#define PAL_KEYCODE_ENTER 50
#define PAL_KEYCODE_TAB 51
#define PAL_KEYCODE_BACKSPACE 52
#define PAL_KEYCODE_SPACE 53
#define PAL_KEYCODE_CAPSLOCK 54
#define PAL_KEYCODE_NUMLOCK 55
#define PAL_KEYCODE_SCROLLLOCK 56
#define PAL_KEYCODE_LSHIFT 57
#define PAL_KEYCODE_RSHIFT 58
#define PAL_KEYCODE_LCTRL 59
#define PAL_KEYCODE_RCTRL 60
#define PAL_KEYCODE_LALT 61
#define PAL_KEYCODE_RALT 62
#define PAL_KEYCODE_LEFT 63
#define PAL_KEYCODE_RIGHT 64
#define PAL_KEYCODE_UP 65
#define PAL_KEYCODE_DOWN 66
#define PAL_KEYCODE_INSERT 67
#define PAL_KEYCODE_DELETE 68
#define PAL_KEYCODE_HOME 69
#define PAL_KEYCODE_END 70
#define PAL_KEYCODE_PAGEUP 71
#define PAL_KEYCODE_PAGEDOWN 72
#define PAL_KEYCODE_KP_0 73
#define PAL_KEYCODE_KP_1 74
#define PAL_KEYCODE_KP_2 75
#define PAL_KEYCODE_KP_3 76
#define PAL_KEYCODE_KP_4 77
#define PAL_KEYCODE_KP_5 78
#define PAL_KEYCODE_KP_6 79
#define PAL_KEYCODE_KP_7 80
#define PAL_KEYCODE_KP_8 81
#define PAL_KEYCODE_KP_9 82
#define PAL_KEYCODE_KP_ENTER 83
#define PAL_KEYCODE_KP_ADD 84
#define PAL_KEYCODE_KP_SUBTRACT 85
#define PAL_KEYCODE_KP_MULTIPLY 86
#define PAL_KEYCODE_KP_DIVIDE 87
#define PAL_KEYCODE_KP_DECIMAL 88
#define PAL_KEYCODE_KP_EQUAL 89
#define PAL_KEYCODE_PRINTSCREEN 90
#define PAL_KEYCODE_PAUSE 91
#define PAL_KEYCODE_MENU 92
#define PAL_KEYCODE_APOSTROPHE 93
#define PAL_KEYCODE_BACKSLASH 94
#define PAL_KEYCODE_COMMA 95
#define PAL_KEYCODE_EQUAL 96
#define PAL_KEYCODE_GRAVEACCENT 97
#define PAL_KEYCODE_SUBTRACT 98
#define PAL_KEYCODE_PERIOD 99
#define PAL_KEYCODE_SEMICOLON 100
#define PAL_KEYCODE_SLASH 101
#define PAL_KEYCODE_LBRACKET 102
#define PAL_KEYCODE_RBRACKET 103
#define PAL_KEYCODE_LSUPER 104
#define PAL_KEYCODE_RSUPER 105
#define PAL_KEYCODE_COUNT 106
/** @} */

/**
 * @defgroup scancodes Scancodes
 * @brief Scancodes
 * 
 * @{
 */
#define PAL_SCANCODE_UNKNOWN 0
#define PAL_SCANCODE_A 1
#define PAL_SCANCODE_B 2
#define PAL_SCANCODE_C 3
#define PAL_SCANCODE_D 4
#define PAL_SCANCODE_E 5
#define PAL_SCANCODE_F 6
#define PAL_SCANCODE_G 7
#define PAL_SCANCODE_H 8
#define PAL_SCANCODE_I 9
#define PAL_SCANCODE_J 10
#define PAL_SCANCODE_K 11
#define PAL_SCANCODE_L 12
#define PAL_SCANCODE_M 13
#define PAL_SCANCODE_N 14
#define PAL_SCANCODE_O 15
#define PAL_SCANCODE_P 16
#define PAL_SCANCODE_Q 17
#define PAL_SCANCODE_R 18
#define PAL_SCANCODE_S 19
#define PAL_SCANCODE_T 20
#define PAL_SCANCODE_U 21
#define PAL_SCANCODE_V 22
#define PAL_SCANCODE_W 23
#define PAL_SCANCODE_X 24
#define PAL_SCANCODE_Y 25
#define PAL_SCANCODE_Z 26
#define PAL_SCANCODE_0 27
#define PAL_SCANCODE_1 28
#define PAL_SCANCODE_2 29
#define PAL_SCANCODE_3 30
#define PAL_SCANCODE_4 31
#define PAL_SCANCODE_5 32
#define PAL_SCANCODE_6 33
#define PAL_SCANCODE_7 34
#define PAL_SCANCODE_8 35
#define PAL_SCANCODE_9 36
#define PAL_SCANCODE_F1 37
#define PAL_SCANCODE_F2 38
#define PAL_SCANCODE_F3 39
#define PAL_SCANCODE_F4 40
#define PAL_SCANCODE_F5 41
#define PAL_SCANCODE_F6 42
#define PAL_SCANCODE_F7 43
#define PAL_SCANCODE_F8 44
#define PAL_SCANCODE_F9 45
#define PAL_SCANCODE_F10 46
#define PAL_SCANCODE_F11 47
#define PAL_SCANCODE_F12 48
#define PAL_SCANCODE_ESCAPE 49
#define PAL_SCANCODE_ENTER 50
#define PAL_SCANCODE_TAB 51
#define PAL_SCANCODE_BACKSPACE 52
#define PAL_SCANCODE_SPACE 53
#define PAL_SCANCODE_CAPSLOCK 54
#define PAL_SCANCODE_NUMLOCK 55
#define PAL_SCANCODE_SCROLLLOCK 56
#define PAL_SCANCODE_LSHIFT 57
#define PAL_SCANCODE_RSHIFT 58
#define PAL_SCANCODE_LCTRL 59
#define PAL_SCANCODE_RCTRL 60
#define PAL_SCANCODE_LALT 61
#define PAL_SCANCODE_RALT 62
#define PAL_SCANCODE_LEFT 63
#define PAL_SCANCODE_RIGHT 64
#define PAL_SCANCODE_UP 65
#define PAL_SCANCODE_DOWN 66
#define PAL_SCANCODE_INSERT 67
#define PAL_SCANCODE_DELETE 68
#define PAL_SCANCODE_HOME 69
#define PAL_SCANCODE_END 70
#define PAL_SCANCODE_PAGEUP 71
#define PAL_SCANCODE_PAGEDOWN 72
#define PAL_SCANCODE_KP_0 73
#define PAL_SCANCODE_KP_1 74
#define PAL_SCANCODE_KP_2 75
#define PAL_SCANCODE_KP_3 76
#define PAL_SCANCODE_KP_4 77
#define PAL_SCANCODE_KP_5 78
#define PAL_SCANCODE_KP_6 79
#define PAL_SCANCODE_KP_7 80
#define PAL_SCANCODE_KP_8 81
#define PAL_SCANCODE_KP_9 82
#define PAL_SCANCODE_KP_ENTER 83
#define PAL_SCANCODE_KP_ADD 84
#define PAL_SCANCODE_KP_SUBTRACT 85
#define PAL_SCANCODE_KP_MULTIPLY 86
#define PAL_SCANCODE_KP_DIVIDE 87
#define PAL_SCANCODE_KP_DECIMAL 88
#define PAL_SCANCODE_KP_EQUAL 89
#define PAL_SCANCODE_PRINTSCREEN 90
#define PAL_SCANCODE_PAUSE 91
#define PAL_SCANCODE_MENU 92
#define PAL_SCANCODE_APOSTROPHE 93
#define PAL_SCANCODE_BACKSLASH 94
#define PAL_SCANCODE_COMMA 95
#define PAL_SCANCODE_EQUAL 96
#define PAL_SCANCODE_GRAVEACCENT 97
#define PAL_SCANCODE_SUBTRACT 98
#define PAL_SCANCODE_PERIOD 99
#define PAL_SCANCODE_SEMICOLON 100
#define PAL_SCANCODE_SLASH 101
#define PAL_SCANCODE_LBRACKET 102
#define PAL_SCANCODE_RBRACKET 103
#define PAL_SCANCODE_LSUPER 104
#define PAL_SCANCODE_RSUPER 105
#define PAL_SCANCODE_COUNT 106
/** @} */

/**
 * @typedef PalKeycode
 * @brief Keycodes (layout aware keys) of a keyboard.
 * 
 * All values of this type follow the format `PAL_KEYCODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalKeycode;

/**
 * @typedef PalScancode
 * @brief scancodes (layout independent keys) of a keyboard.
 * 
 * All values of this type follow the format `PAL_SCANCODE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalScancode;

/**
 * @brief Gets the state of the keycodes (layout aware keys) of the
 * keyboard.
 *
 * The returned pointer must not be freed. The state is updated when
 * `palUpdateVideo()` is called. The array must be index with 
 * PalKeycodes values and not exceed `PAL_KEYCODE_COUNT`.
 *
 * @return A pointer to the keycodes array on success or `nullptr` on failure.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API const PalBool* PAL_CALL palGetKeycodeState();

/**
 * @brief Gets the state of the scancodes (layout independent keys) of
 * the keyboard.
 *
 * The returned pointer must not be freed. The state is updated when
 * `palUpdateVideo()` is called. The array must be index with 
 * PalScancodes values and not exceed `PAL_SCANCODE_COUNT`.
 *
 * @return A pointer to the scancodes array on success or `nullptr` on failure.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API const PalBool* PAL_CALL palGetScancodeState();

#endif // PAL_VIDEO_KEYBOARD_H