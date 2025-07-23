
#ifndef _PAL_WIN32_INPUT_H
#define _PAL_WIN32_INPUT_H

#define WIN32_INPUT_CLASS L"PALInputClass"
#define WIN32_INPUT_PROP L"PALInput"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#define XINPUT_COUNT 4
#define XINPUT_TAG 0xFFFF0000

#include <windows.h>
#include <xinput.h>

#include "input/pal_input_internal.h"

typedef DWORD (WINAPI* XInputGetStateFn)(DWORD, XINPUT_STATE*);
typedef DWORD (WINAPI* XInputSetStateFn)(DWORD, XINPUT_VIBRATION*);

typedef struct InputDataWin32 {
    HINSTANCE instance;
    HINSTANCE xInput;
    HWND window;

    XInputGetStateFn getXinputState;
    XInputSetStateFn setXinputState;
    bool XinputSupport;
} InputDataWin32;

LRESULT CALLBACK palInputProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool palRegisterInputClass(HINSTANCE instance);
void palUnregisterInputClass(HINSTANCE instance);

bool palGetRawDeviceInfo(
    PalInputDevice device, 
    PalInputDeviceInfo* info);

bool palGetXinputDeviceInfo(
    PalInputDevice device, 
    PalInputDeviceInfo* info);

static inline PalInputDevice palMakeXinputHandle(Uint32 index) {

    return (PalInputDevice)(uintptr_t)(XINPUT_TAG | index);
}

static inline bool palIsXinputHandle(PalInputDevice handle) {

    return ((uintptr_t)handle & XINPUT_TAG) == XINPUT_TAG;
}

static inline Uint32 palGetXinputIndex(PalInputDevice handle) {

    return ((uintptr_t)handle & XINPUT_TAG);
}

#endif // _PAL_WIN32_INPUT_H