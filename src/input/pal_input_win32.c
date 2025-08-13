
/**

Copyright (C) 2025 Nicholas Agbo
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

#include "pal/pal_input.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>
#include <xinput.h>
#include <hidsdi.h>
#include <dbt.h>
#include <wchar.h>

#define PAL_WIN32_INPUT_CLASS L"PALInputClass"
#define PAL_WIN32_INPUT_PROP L"PALInput"

#define XINPUT_1_4 "xinput1_4.dll"
#define XINPUT_1_3 "xinput1_3.dll"
#define XINPUT_9_1_0 "xinput9_1_0.dll"
#define XINPUT_COUNT 4
#define XINPUT_TAG 0xFFFF0000

typedef DWORD (WINAPI* XInputGetStateFn)(DWORD, XINPUT_STATE*);
typedef DWORD (WINAPI* XInputSetStateFn)(DWORD, XINPUT_VIBRATION*);

typedef BOOLEAN (WINAPI* HidD_GetProductStringFn)(HANDLE, PVOID, ULONG);
typedef BOOLEAN (WINAPI* HidD_GetManufacturerStringFn)(HANDLE, PVOID, ULONG);
typedef BOOLEAN (WINAPI* HidD_GetAttributesFn)(HANDLE, PHIDD_ATTRIBUTES);
typedef void (WINAPI* HidD_GetGuidFn)(GUID*);

static XInputGetStateFn s_GetXinputState = nullptr;
static XInputSetStateFn s_SetXinputState = nullptr;

static HidD_GetProductStringFn s_HidD_GetProductString;
static HidD_GetManufacturerStringFn s_HidD_GetManufacturerString;
static HidD_GetAttributesFn s_HidD_GetAttributes;
static HidD_GetGuidFn s_HidD_GetGuid;

static HINSTANCE s_XInput;
static HINSTANCE s_Hid;
static GUID s_HidGuid;

static bool s_Loaded = false;

typedef struct PalInputSystem {
    PalAllocator* allocator;
    HINSTANCE instance;
    HWND window;
    HDEVNOTIFY notifyHandle;
    PalEventDriver* eventDriver;
} PalInputSystem;

typedef struct KeyboardWin32 {
    PalScancode scancodes[512];
    PalKey keycodes[256];
    bool scancodeState[PAL_SCANCODE_MAX];
    bool keyState[PAL_KEY_MAX];
} KeyboardWin32;

typedef struct MouseWin32 {
    PalMousePosition motion;
    PalMouseWheel wheel;
    bool state[PAL_MOUSE_BUTTON_MAX];
    bool active;
} MouseWin32;

static KeyboardWin32 s_Keyboard;
static MouseWin32 s_Mouse;

static void getHidProperties(PalInputDeviceInfo* info);
static void createKeyTable();
static void createScancodeTable();

static PalResult registerRawDevice(
    PalInputSystem* system,
    PalInputDevice* inputDevice,
    bool remove);

static void handleKeyboardInput(RAWKEYBOARD* keyboard);
static void handleMouseInput(RAWMOUSE* mouse);

LRESULT CALLBACK inputProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam);

static inline PalInputDevice* palMakeXinputHandle(Uint32 index) {

    return (PalInputDevice*)(uintptr_t)(XINPUT_TAG | index);
}

static inline bool palIsXinputHandle(PalInputDevice* handle) {

    return ((uintptr_t)handle & XINPUT_TAG) == XINPUT_TAG;
}

static inline Uint32 palGetXinputIndex(PalInputDevice* handle) {

    return ((uintptr_t)handle & XINPUT_TAG);
}

// ==================================================
// Public API
// ==================================================

PalResult _PCALL palCreateInputSystem(
    const PalInputSystemCreateInfo *info,
    PalInputSystem **outSystem) {

    if (!info || !outSystem) {
        return PAL_RESULT_NULL_POINTER;
    }

    PalInputSystem* system = nullptr;
    if (info->allocator && (!info->allocator->allocate || !info->allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    system = palAllocate(info->allocator, sizeof(PalInputSystem), 0);
    if (!system) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    memset(system, 0, sizeof(PalInputSystem));
    if (info->allocator) {
        system->allocator = info->allocator;
    }

    if (info->eventDriver) {
        system->eventDriver = info->eventDriver;
    }

    system->instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    if (!GetClassInfoExW(system->instance, PAL_WIN32_INPUT_CLASS, &wc)) {
        wc.lpfnWndProc = inputProc;
        wc.hInstance = system->instance;
        wc.lpszClassName = PAL_WIN32_INPUT_CLASS;

        if (!RegisterClassExW(&wc)) {
            return PAL_RESULT_ACCESS_DENIED;
        }
    }

    system->window = CreateWindowExW(
        0,
        PAL_WIN32_INPUT_CLASS,
        L"",
        0,
        0,
        0,
        1,
        1,
        nullptr,
        nullptr,
        system->instance,
        nullptr
    );

    if (!system->window) {
        palFree(system->allocator, system);
        return PAL_RESULT_VIDEO_DEVICE_NOT_FOUND;
    }

    // load libraries
    if (!s_Loaded) {
        // Xinput
        s_XInput = LoadLibraryA(XINPUT_1_4);
        if (!s_XInput) {
            s_XInput = LoadLibraryA(XINPUT_1_3);

        } if (!s_XInput) {
            s_XInput = LoadLibraryA(XINPUT_9_1_0);
        }

        if (s_XInput) {
            s_GetXinputState = (XInputGetStateFn)GetProcAddress(
                s_XInput,
                "XInputGetState"
            );

            s_SetXinputState = (XInputSetStateFn)GetProcAddress(
                s_XInput,
                "XInputSetState"
            );
        }

        // hid
        s_Hid = LoadLibraryA("hid.dll");
        if (s_Hid) {
            s_HidD_GetAttributes = (HidD_GetAttributesFn)GetProcAddress(
                s_Hid,
                "HidD_GetAttributes"
            );

            s_HidD_GetManufacturerString = (HidD_GetManufacturerStringFn)GetProcAddress(
                s_Hid,
                "HidD_GetManufacturerString"
            );

            s_HidD_GetProductString = (HidD_GetProductStringFn)GetProcAddress(
                s_Hid,
                "HidD_GetProductString"
            );

            s_HidD_GetGuid = (HidD_GetGuidFn)GetProcAddress(
                s_Hid,
                "HidD_GetHidGuid"
            );
        }

        createScancodeTable();
        createKeyTable();
        s_Loaded = true;     
    }

    // register hid notifications
    if (s_HidD_GetGuid) {
        s_HidD_GetGuid(&s_HidGuid);

        DEV_BROADCAST_DEVICEINTERFACE_W filter = {0};
        filter.dbcc_size = sizeof(filter);
        filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        filter.dbcc_classguid = s_HidGuid;

        system->notifyHandle = RegisterDeviceNotificationW(
            system->window,
            &filter,
            DEVICE_NOTIFY_WINDOW_HANDLE
        );
    }

    ShowWindow(system->window, SW_HIDE);
    SetPropW(system->window, PAL_WIN32_INPUT_PROP, system);
    *outSystem = system;
    return PAL_RESULT_SUCCESS;
}

void _PCALL palDestroyInputSystem(PalInputSystem *system) {

    if (!system || (system && !system->instance)) {
        return;
    }

    DestroyWindow(system->window);
    UnregisterClassW(PAL_WIN32_INPUT_CLASS, system->instance);
    UnregisterDeviceNotification(system->notifyHandle);
    palFree(system->allocator, system);
}

void _PCALL palUpdateInput(PalInputSystem* system) {

    if (!system) {
        return;
    }

    if (s_Mouse.active) {
        // reset mouse state
        s_Mouse.motion.dx = 0;
        s_Mouse.motion.dy = 0;
        s_Mouse.wheel.x = 0;
        s_Mouse.wheel.y = 0;

        // update mouse position
        POINT pos;
        GetCursorPos(&pos);
        s_Mouse.motion.x = pos.x;
        s_Mouse.motion.y = pos.y;
    }

    MSG msg;
    while (PeekMessageA(&msg, system->window, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

PalResult _PCALL palEnumerateInputDevices(
    PalInputSystem* system,
    PalInputMask mask,
    Int32* count,
    PalInputDevice** inputDevices) {
    
    if (!system || !count) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (count == 0 && !inputDevices) {
        PAL_RESULT_INSUFFICIENT_BUFFER;
    }

    Int32 deviceCount = 0;
    Int32 rawDeviceCount = 0;

    GetRawInputDeviceList(
        nullptr, 
        &rawDeviceCount, 
        sizeof(RAWINPUTDEVICELIST)
    );

    RAWINPUTDEVICELIST* devices = palAllocate(
        system->allocator, 
        sizeof(RAWINPUTDEVICELIST) * rawDeviceCount,
        0
    );

    if (!devices) {
        return PAL_RESULT_OUT_OF_MEMORY;
    }

    GetRawInputDeviceList(
        devices, 
        &rawDeviceCount, 
        sizeof(RAWINPUTDEVICELIST)
    );

    RID_DEVICE_INFO info = {};
    info.cbSize = sizeof(RID_DEVICE_INFO);
    Uint32 size = sizeof(RID_DEVICE_INFO);
    Int32 maxDeviceCount = inputDevices ? *count : rawDeviceCount;

    for (Int32 i = 0; i < rawDeviceCount; ++i) {
        HANDLE device = devices[i].hDevice;
        if (GetRawInputDeviceInfoA(
            device,
            RIDI_DEVICEINFO,
            &info,
            &size) == (UINT)-1) {
            return PAL_RESULT_INPUT_DEVICE_NOT_FOUND;
        }

        switch (info.dwType) {
            case RIM_TYPEKEYBOARD: {
                if (mask & PAL_INPUT_MASK_KEYBOARD) {
                    if (inputDevices) {
                        if (deviceCount < maxDeviceCount) {
                            inputDevices[deviceCount] = (PalInputDevice*)device;
                        }
                    }
                    deviceCount++;
                }
                break;
            }

            case RIM_TYPEMOUSE: {
                if (mask & PAL_INPUT_MASK_MOUSE) {
                    if (inputDevices) {
                        if (deviceCount < maxDeviceCount) {
                            inputDevices[deviceCount] = (PalInputDevice*)device;
                        }
                    }
                    deviceCount++;
                }
                break;
            }

            case RIM_TYPEHID: {
                Int32 usagePage = info.hid.usUsagePage;
                Int32 usage = info.hid.usUsage;

                if (mask & PAL_INPUT_MASK_KEYBOARD) {
                    if (usagePage == 0x01 && usage == 0x06) {
                        if (inputDevices) {
                            if (deviceCount < maxDeviceCount) {
                                inputDevices[deviceCount] = (PalInputDevice*)device;
                            }
                        }
                        deviceCount++;
                    }
                }

                if (mask & PAL_INPUT_MASK_MOUSE) {
                    if (usagePage == 0x01 && usage == 0x02) {
                        if (inputDevices) {
                            if (deviceCount < maxDeviceCount) {
                                inputDevices[deviceCount] = (PalInputDevice*)device;
                            }
                        }
                        deviceCount++;
                    }
                }

                if (mask & PAL_INPUT_MASK_GAMEPAD) {
                    if (usagePage == 0x01 && usage == 0x05) {
                        if (inputDevices) {
                            if (deviceCount < maxDeviceCount) {
                                inputDevices[deviceCount] = (PalInputDevice*)device;
                            }
                        }
                        deviceCount++;
                    }
                }

            break;
            }
        }
    }

    palFree(system->allocator, devices);

    // Xnput devices (Xbox controllers)
    if (mask & PAL_INPUT_MASK_GAMEPAD) {
        if (s_XInput) {
            XINPUT_STATE state;
            for (DWORD i = 0; i < XINPUT_COUNT; i++) {
                if (s_GetXinputState(i, &state) == ERROR_SUCCESS) {
                    if (inputDevices) {
                        if (deviceCount < maxDeviceCount) {
                            PalInputDevice* handle = palMakeXinputHandle(i);
                            inputDevices[deviceCount] = handle;
                        }
                    }
                    deviceCount++;
                }
            }
        }
    }

    if (!inputDevices) {
        *count = deviceCount;
    }
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palGetInputDeviceInfo(
    PalInputDevice* inputDevice, 
    PalInputDeviceInfo* info) {
    
    if (!inputDevice || !info) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (palIsXinputHandle(inputDevice)) {
        // TODO: XInput

    } else {
        RID_DEVICE_INFO deviceInfo = {};
        deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
        Uint32 size = sizeof(RID_DEVICE_INFO);
        HANDLE device = (HANDLE)inputDevice;
        if (GetRawInputDeviceInfoA(
            device,
            RIDI_DEVICEINFO,
            &deviceInfo,
            &size) == (UINT)-1) {
            return PAL_RESULT_INVALID_INPUT_DEVICE;
        }

        // get device path string
        size = sizeof(info->path);
        if (GetRawInputDeviceInfoA(
            device,
            RIDI_DEVICENAME,
            &info->path,
            &size) == (UINT)-1) {
            return PAL_RESULT_INVALID_INPUT_DEVICE;
        }

        switch (deviceInfo.dwType) {
            case RIM_TYPEKEYBOARD: {
                strcpy(info->name, "Standard Keyboard");
                info->productID = 0;
                info->vendorID = 0;
                info->type = PAL_INPUT_DEVICE_KEYBOARD;
                break;
            }

            case RIM_TYPEMOUSE: {
                strcpy(info->name, "Standard Mouse");
                info->productID = 0;
                info->vendorID = 0;
                info->type = PAL_INPUT_DEVICE_MOUSE;
                break;
            }

            case RIM_TYPEHID: {
                Int32 usagePage = deviceInfo.hid.usUsagePage;
                Int32 usage = deviceInfo.hid.usUsage;

                if (usagePage == 0x01 && usage == 0x06) {
                    info->type = PAL_INPUT_DEVICE_KEYBOARD;
                    getHidProperties(info);
                    break;

                } else if (usagePage == 0x01 && usage == 0x02) {
                    info->type = PAL_INPUT_DEVICE_MOUSE;
                    getHidProperties(info);
                    break;

                } else if (usagePage == 0x01 && usage == 0x05) {
                    getHidProperties(info);
                    break;
                }
            }
        }

    }
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palRegisterInputDevice(
    PalInputSystem* system,
    PalInputDevice* inputDevice) {

    if (!system || !inputDevice) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (palIsXinputHandle(inputDevice)) {
        // TODO: XInput

    } else {
        return registerRawDevice(system, inputDevice, false);
    }
}

PalResult _PCALL palUnregisterInputDevice(
    PalInputSystem* system,
    PalInputDevice* inputDevice) {
    
    if (!system || !inputDevice) {
        return PAL_RESULT_NULL_POINTER;
    }

    if (palIsXinputHandle(inputDevice)) {
        // TODO: XInput

    } else {
        return registerRawDevice(system, inputDevice, true);
    }
}

void _PCALL palGetKeyboardState(
    PalInputSystem* system,
    PalKeyboardState* state) {
    
    if (!system || !state) {
        return;
    }

    state->keys = s_Keyboard.keyState;
    state->scancodes = s_Keyboard.scancodeState;
}

void _PCALL palGetMouseState(
    PalInputSystem* system,
    PalMouseState* state) {
    
    if (!system || !state) {
        return;
    }

    state->buttons = s_Mouse.state;
    state->wheel = &s_Mouse.wheel;
    state->motion = &s_Mouse.motion;
}


// ==================================================
// Internal API
// ==================================================

static void getHidProperties(PalInputDeviceInfo* info) {

    HANDLE file = CreateFileA(
        info->path,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (!file) {
        info->productID = 0;
        info->vendorID = 0;
        info->type = PAL_INPUT_DEVICE_GENERIC_GAMEPAD;
        return;
    }

    WCHAR manufacturer[64] = {};
    WCHAR product[64] = {};
    bool sucessManufacturer = s_HidD_GetManufacturerString(
        file,
        manufacturer,
        sizeof(manufacturer)
    );

    bool sucessproduct = s_HidD_GetProductString(
        file,
        product,
        sizeof(product)
    );

    if (sucessManufacturer && sucessproduct) {
        WCHAR combined[128] = {};
        snwprintf(combined, 128, L"%s %s", manufacturer, product);
        WideCharToMultiByte(CP_UTF8, 0, combined, -1, info->name, 128, NULL, NULL);

    } else if (sucessManufacturer) {
        WideCharToMultiByte(CP_UTF8, 0, manufacturer, -1, info->name, 128, NULL, NULL);

    } else if (sucessproduct) {
        WideCharToMultiByte(CP_UTF8, 0, product, -1, info->name, 128, NULL, NULL);

    } else {
        strcpy(info->name, "Generic Gamepad");
    }

    // get attributes
    HIDD_ATTRIBUTES attrs = {};
    attrs.Size = sizeof(HIDD_ATTRIBUTES);
    if (s_HidD_GetAttributes(file, &attrs)) {
        info->productID = 0;
        info->vendorID = 0;
        info->type = PAL_INPUT_DEVICE_GENERIC_GAMEPAD;
        return;
        CloseHandle(file);
    }

    info->productID = attrs.ProductID;
    info->vendorID = attrs.VendorID;

    // get gamepad type
    if (attrs.VendorID == 0x054C) {
        // sony controllers
        switch (attrs.ProductID) {
            case 0x05C4:
            info->type = PAL_INPUT_DEVICE_DUALSHOCK4;
            break;

            case 0x0CE6:
            info->type = PAL_INPUT_DEVICE_DUALSENSE;
            break;
        }

    } else if (attrs.VendorID == 0x057E) {
        // nintendo
        switch (attrs.ProductID) {
            case 0x2009:
            info->type = PAL_INPUT_DEVICE_SWITCH_PRO;
            break;
        }

    } else {
        info->type = PAL_INPUT_DEVICE_GENERIC_GAMEPAD;
    }

    CloseHandle(file);
}

static void createKeyTable() {

    // Letters
    s_Keyboard.keycodes['A'] = PAL_KEY_A;
    s_Keyboard.keycodes['B'] = PAL_KEY_B;
    s_Keyboard.keycodes['C'] = PAL_KEY_C;
    s_Keyboard.keycodes['D'] = PAL_KEY_D;
    s_Keyboard.keycodes['E'] = PAL_KEY_E;
    s_Keyboard.keycodes['F'] = PAL_KEY_F;
    s_Keyboard.keycodes['G'] = PAL_KEY_G;
    s_Keyboard.keycodes['H'] = PAL_KEY_H;
    s_Keyboard.keycodes['I'] = PAL_KEY_I;
    s_Keyboard.keycodes['J'] = PAL_KEY_J;
    s_Keyboard.keycodes['K'] = PAL_KEY_K;
    s_Keyboard.keycodes['L'] = PAL_KEY_L;
    s_Keyboard.keycodes['M'] = PAL_KEY_M;
    s_Keyboard.keycodes['N'] = PAL_KEY_N;
    s_Keyboard.keycodes['O'] = PAL_KEY_O;
    s_Keyboard.keycodes['P'] = PAL_KEY_P;
    s_Keyboard.keycodes['Q'] = PAL_KEY_Q;
    s_Keyboard.keycodes['R'] = PAL_KEY_R;
    s_Keyboard.keycodes['S'] = PAL_KEY_S;
    s_Keyboard.keycodes['T'] = PAL_KEY_T;
    s_Keyboard.keycodes['U'] = PAL_KEY_U;
    s_Keyboard.keycodes['V'] = PAL_KEY_V;
    s_Keyboard.keycodes['W'] = PAL_KEY_W;
    s_Keyboard.keycodes['Z'] = PAL_KEY_Z;
    s_Keyboard.keycodes['Y'] = PAL_KEY_Y;
    s_Keyboard.keycodes['Z'] = PAL_KEY_Z;

    // Numbers (top row)
    s_Keyboard.keycodes['0'] = PAL_KEY_0;
    s_Keyboard.keycodes['1'] = PAL_KEY_1;
    s_Keyboard.keycodes['2'] = PAL_KEY_2;
    s_Keyboard.keycodes['3'] = PAL_KEY_3;
    s_Keyboard.keycodes['4'] = PAL_KEY_4;
    s_Keyboard.keycodes['5'] = PAL_KEY_5;
    s_Keyboard.keycodes['6'] = PAL_KEY_6;
    s_Keyboard.keycodes['7'] = PAL_KEY_7;
    s_Keyboard.keycodes['8'] = PAL_KEY_8;
    s_Keyboard.keycodes['9'] = PAL_KEY_9;

    // Function
    s_Keyboard.keycodes[VK_F1] = PAL_KEY_F1;
    s_Keyboard.keycodes[VK_F2] = PAL_KEY_F2;
    s_Keyboard.keycodes[VK_F3] = PAL_KEY_F3;
    s_Keyboard.keycodes[VK_F4] = PAL_KEY_F4;
    s_Keyboard.keycodes[VK_F5] = PAL_KEY_F5;
    s_Keyboard.keycodes[VK_F6] = PAL_KEY_F6;
    s_Keyboard.keycodes[VK_F7] = PAL_KEY_F7;
    s_Keyboard.keycodes[VK_F8] = PAL_KEY_F8;
    s_Keyboard.keycodes[VK_F9] = PAL_KEY_F9;
    s_Keyboard.keycodes[VK_F10] = PAL_KEY_F10;
    s_Keyboard.keycodes[VK_F11] = PAL_KEY_F11;
    s_Keyboard.keycodes[VK_F12] = PAL_KEY_F12;

    // Control
    s_Keyboard.keycodes[VK_ESCAPE] = PAL_KEY_ESCAPE;
    s_Keyboard.keycodes[VK_RETURN] = PAL_KEY_ENTER;
    s_Keyboard.keycodes[VK_TAB] = PAL_KEY_TAB;
    s_Keyboard.keycodes[VK_BACK] = PAL_KEY_BACKSPACE;
    s_Keyboard.keycodes[VK_SPACE] = PAL_KEY_SPACE;
    s_Keyboard.keycodes[VK_CAPITAL] = PAL_KEY_CAPSLOCK;
    s_Keyboard.keycodes[VK_NUMLOCK] = PAL_KEY_NUMLOCK;
    s_Keyboard.keycodes[VK_SCROLL] = PAL_KEY_SCROLLLOCK;
    s_Keyboard.keycodes[VK_LSHIFT] = PAL_KEY_LSHIFT;
    s_Keyboard.keycodes[VK_RSHIFT] = PAL_KEY_RSHIFT;
    s_Keyboard.keycodes[VK_LCONTROL] = PAL_KEY_LCTRL;
    s_Keyboard.keycodes[VK_RCONTROL] = PAL_KEY_RCTRL;
    s_Keyboard.keycodes[VK_LMENU] = PAL_KEY_LALT;
    s_Keyboard.keycodes[VK_RMENU] = PAL_KEY_RALT;

    // Arrows
    s_Keyboard.keycodes[VK_LEFT] = PAL_KEY_LEFT;
    s_Keyboard.keycodes[VK_RIGHT] = PAL_KEY_RIGHT;
    s_Keyboard.keycodes[VK_UP] = PAL_KEY_UP;
    s_Keyboard.keycodes[VK_DOWN] = PAL_KEY_DOWN;

    // Navigation
    s_Keyboard.keycodes[VK_INSERT] = PAL_KEY_INSERT;
    s_Keyboard.keycodes[VK_DELETE] = PAL_KEY_DELETE;
    s_Keyboard.keycodes[VK_HOME] = PAL_KEY_HOME;
    s_Keyboard.keycodes[VK_END] = PAL_KEY_END;
    s_Keyboard.keycodes[VK_PRIOR] = PAL_KEY_PAGEUP;
    s_Keyboard.keycodes[VK_NEXT] = PAL_KEY_PAGEDOWN;

    // Keypad
    s_Keyboard.keycodes[VK_NUMPAD0] = PAL_KEY_KP_0;
    s_Keyboard.keycodes[VK_NUMPAD1] = PAL_KEY_KP_1;
    s_Keyboard.keycodes[VK_NUMPAD2] = PAL_KEY_KP_2;
    s_Keyboard.keycodes[VK_NUMPAD3] = PAL_KEY_KP_3;
    s_Keyboard.keycodes[VK_NUMPAD4] = PAL_KEY_KP_4;
    s_Keyboard.keycodes[VK_NUMPAD5] = PAL_KEY_KP_5;
    s_Keyboard.keycodes[VK_NUMPAD6] = PAL_KEY_KP_6;
    s_Keyboard.keycodes[VK_NUMPAD7] = PAL_KEY_KP_7;
    s_Keyboard.keycodes[VK_NUMPAD8] = PAL_KEY_KP_8;
    s_Keyboard.keycodes[VK_NUMPAD9] = PAL_KEY_KP_9;
    
    s_Keyboard.keycodes[VK_ADD] = PAL_KEY_KP_ADD;
    s_Keyboard.keycodes[VK_SUBTRACT] = PAL_KEY_KP_SUBTRACT;
    s_Keyboard.keycodes[VK_MULTIPLY] = PAL_KEY_KP_MULTIPLY;
    s_Keyboard.keycodes[VK_DIVIDE] = PAL_KEY_KP_DIVIDE;
    s_Keyboard.keycodes[VK_DECIMAL] = PAL_KEY_KP_DECIMAL;

    // Misc
    s_Keyboard.keycodes[VK_SNAPSHOT] = PAL_KEY_PRINTSCREEN;
    s_Keyboard.keycodes[VK_PAUSE] = PAL_KEY_PAUSE;    
    s_Keyboard.keycodes[VK_APPS] = PAL_KEY_MENU;
    s_Keyboard.keycodes[VK_OEM_7] = PAL_KEY_APOSTROPHE;
    s_Keyboard.keycodes[VK_OEM_5] = PAL_KEY_BACKSLASH;
    s_Keyboard.keycodes[VK_OEM_COMMA] = PAL_KEY_COMMA;
    s_Keyboard.keycodes[VK_OEM_PLUS] = PAL_KEY_EQUAL;
    s_Keyboard.keycodes[VK_OEM_3] = PAL_KEY_GRAVEACCENT;
    s_Keyboard.keycodes[VK_OEM_MINUS] = PAL_KEY_SUBTRACT;
    s_Keyboard.keycodes[VK_OEM_PERIOD] = PAL_KEY_PERIOD;
    s_Keyboard.keycodes[VK_OEM_1] = PAL_KEY_SEMICOLON;
    s_Keyboard.keycodes[VK_OEM_2] = PAL_KEY_SLASH;
    s_Keyboard.keycodes[VK_OEM_4] = PAL_KEY_LBRACKET;
    s_Keyboard.keycodes[VK_OEM_6] = PAL_KEY_RBRACKET;
    s_Keyboard.keycodes[VK_LWIN] = PAL_KEY_LSUPER;
    s_Keyboard.keycodes[VK_RWIN] = PAL_KEY_RSUPER;
}

static void createScancodeTable() {

    // Scancodes are made from OR'ed (scancode | extended)

    // Letters
    s_Keyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    s_Keyboard.scancodes[0x030] = PAL_SCANCODE_B;
    s_Keyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    s_Keyboard.scancodes[0x020] = PAL_SCANCODE_D;
    s_Keyboard.scancodes[0x012] = PAL_SCANCODE_E;
    s_Keyboard.scancodes[0x021] = PAL_SCANCODE_F;
    s_Keyboard.scancodes[0x022] = PAL_SCANCODE_G;
    s_Keyboard.scancodes[0x023] = PAL_SCANCODE_H;
    s_Keyboard.scancodes[0x017] = PAL_SCANCODE_I;
    s_Keyboard.scancodes[0x024] = PAL_SCANCODE_J;
    s_Keyboard.scancodes[0x025] = PAL_SCANCODE_K;
    s_Keyboard.scancodes[0x026] = PAL_SCANCODE_L;
    s_Keyboard.scancodes[0x032] = PAL_SCANCODE_M;
    s_Keyboard.scancodes[0x031] = PAL_SCANCODE_N;
    s_Keyboard.scancodes[0x018] = PAL_SCANCODE_O;
    s_Keyboard.scancodes[0x019] = PAL_SCANCODE_P;
    s_Keyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    s_Keyboard.scancodes[0x013] = PAL_SCANCODE_R;
    s_Keyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    s_Keyboard.scancodes[0x014] = PAL_SCANCODE_T;
    s_Keyboard.scancodes[0x016] = PAL_SCANCODE_U;
    s_Keyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    s_Keyboard.scancodes[0x011] = PAL_SCANCODE_W;
    s_Keyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    s_Keyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    s_Keyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    s_Keyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    s_Keyboard.scancodes[0x002] = PAL_SCANCODE_1;
    s_Keyboard.scancodes[0x003] = PAL_SCANCODE_2;
    s_Keyboard.scancodes[0x004] = PAL_SCANCODE_3;
    s_Keyboard.scancodes[0x005] = PAL_SCANCODE_4;
    s_Keyboard.scancodes[0x006] = PAL_SCANCODE_5;
    s_Keyboard.scancodes[0x007] = PAL_SCANCODE_6;
    s_Keyboard.scancodes[0x008] = PAL_SCANCODE_7;
    s_Keyboard.scancodes[0x009] = PAL_SCANCODE_8;
    s_Keyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    s_Keyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    s_Keyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    s_Keyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    s_Keyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    s_Keyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    s_Keyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    s_Keyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    s_Keyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    s_Keyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    s_Keyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    s_Keyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    s_Keyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    s_Keyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    s_Keyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    s_Keyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    s_Keyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    s_Keyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    s_Keyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    s_Keyboard.scancodes[0x145] = PAL_SCANCODE_NUMLOCK;
    s_Keyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    s_Keyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    s_Keyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    s_Keyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    s_Keyboard.scancodes[0x11D] = PAL_SCANCODE_RCTRL;
    s_Keyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    s_Keyboard.scancodes[0x138] = PAL_SCANCODE_RALT;

    // Arrows
    s_Keyboard.scancodes[0x14B] = PAL_SCANCODE_LEFT;
    s_Keyboard.scancodes[0x14D] = PAL_SCANCODE_RIGHT;
    s_Keyboard.scancodes[0x148] = PAL_SCANCODE_UP;
    s_Keyboard.scancodes[0x150] = PAL_SCANCODE_DOWN;

    // Navigation
    s_Keyboard.scancodes[0x152] = PAL_SCANCODE_INSERT;
    s_Keyboard.scancodes[0x153] = PAL_SCANCODE_DELETE;
    s_Keyboard.scancodes[0x147] = PAL_SCANCODE_HOME;
    s_Keyboard.scancodes[0x14F] = PAL_SCANCODE_END;
    s_Keyboard.scancodes[0x149] = PAL_SCANCODE_PAGEUP;
    s_Keyboard.scancodes[0x151] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    s_Keyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    s_Keyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    s_Keyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    s_Keyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    s_Keyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    s_Keyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    s_Keyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    s_Keyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    s_Keyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    s_Keyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    s_Keyboard.scancodes[0x11C] = PAL_SCANCODE_KP_ENTER;
    s_Keyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    s_Keyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    s_Keyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    s_Keyboard.scancodes[0x135] = PAL_SCANCODE_KP_DIVIDE;
    s_Keyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;
    s_Keyboard.scancodes[0x059] = PAL_SCANCODE_KP_EQUAL;

    // Misc
    s_Keyboard.scancodes[0x137] = PAL_SCANCODE_PRINTSCREEN;
    s_Keyboard.scancodes[0x146] = PAL_SCANCODE_PAUSE;    
    s_Keyboard.scancodes[0x045] = PAL_SCANCODE_PAUSE;
    s_Keyboard.scancodes[0x15D] = PAL_SCANCODE_MENU;
    s_Keyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    s_Keyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    s_Keyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    s_Keyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    s_Keyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    s_Keyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    s_Keyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    s_Keyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    s_Keyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    s_Keyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    s_Keyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    s_Keyboard.scancodes[0x15B] = PAL_SCANCODE_LSUPER;
    s_Keyboard.scancodes[0x15C] = PAL_SCANCODE_RSUPER;
}

static PalResult registerRawDevice(
    PalInputSystem* system,
    PalInputDevice* inputDevice,
    bool remove) {

    RID_DEVICE_INFO info = {};
    info.cbSize = sizeof(RID_DEVICE_INFO);
    Uint32 size = sizeof(RID_DEVICE_INFO);
    if (GetRawInputDeviceInfoA(
        (HANDLE)inputDevice,
        RIDI_DEVICEINFO,
        &info,
        &size) == (UINT)-1) {
        return PAL_RESULT_INVALID_INPUT_DEVICE;
    }

    DWORD flag = RIDEV_INPUTSINK;
    HWND window = system->window;
    if (remove) {
        flag = RIDEV_REMOVE;
        window = nullptr;
    }

    switch (info.dwType) {
        case RIM_TYPEKEYBOARD: {
            // register a keyboard device
            RAWINPUTDEVICE rid = {};
            rid.dwFlags = flag;
            rid.hwndTarget = window;
            rid.usUsage = 0x06; 
            rid.usUsagePage = 0x01;

            if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
                if (remove) {
                    return PAL_RESULT_INPUT_DEVICE_NOT_REGISTERED;
                } else {
                    return PAL_RESULT_INPUT_DEVICE_NOT_FOUND;
                }
            }
            return PAL_RESULT_SUCCESS;
        }

        case RIM_TYPEMOUSE: {
            RAWINPUTDEVICE rid = {};
            rid.dwFlags = flag;
            rid.hwndTarget = window;
            rid.usUsage = 0x02; 
            rid.usUsagePage = 0x01;

            if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
                if (remove) {
                    return PAL_RESULT_INPUT_DEVICE_NOT_REGISTERED;
                } else {
                    return PAL_RESULT_INPUT_DEVICE_NOT_FOUND;
                }
            }

            // will be deactivated at WM_DEVICECHANGE if mouse is removed.
            if (remove) {
                s_Mouse.active = false;
            } else {
                s_Mouse.active = true;
            }
            return PAL_RESULT_SUCCESS;
        }

        case RIM_TYPEHID: {
            // TODO: HID
            return PAL_RESULT_INVALID_INPUT_DEVICE;
        }
    }
}

static void handleKeyboardInput(RAWKEYBOARD* keyboard) {

    PalScancode scancode = PAL_SCANCODE_UNKNOWN;
    PalKey key = PAL_KEY_UNKNOWN;
    bool extended = (keyboard->Flags & RI_KEY_E0) != 0;

    // special scancode handling
    if (!extended && keyboard->MakeCode == 0x045) {
        scancode = PAL_SCANCODE_NUMLOCK;

    } else {
        Uint16 index = keyboard->MakeCode | (extended << 8);
        scancode = s_Keyboard.scancodes[index];
    }

    // special keycode handling
    if (extended && keyboard->VKey == VK_RETURN) {
        key = PAL_KEY_KP_ENTER;

    } else if (extended && keyboard->VKey == VK_OEM_PLUS) {
        key = PAL_KEY_KP_EQUAL;

    } else {
        key = s_Keyboard.keycodes[keyboard->VKey];
    }

    bool isKeyDown = !(keyboard->Flags & RI_KEY_BREAK);
    bool keyRepeat = s_Keyboard.keyState[key];
    bool scancodeRepeat = s_Keyboard.scancodeState[scancode];

    // check press and release
    if (keyboard->Flags & RI_KEY_BREAK) {
        s_Keyboard.keyState[key] = false;
        s_Keyboard.scancodeState[scancode] = false;
        
    } else {
        s_Keyboard.keyState[key] = true;
        s_Keyboard.scancodeState[scancode] = true;  
    }
}

static void handleMouseInput(RAWMOUSE* mouse) {

    USHORT flags = mouse->usButtonFlags;
    s_Mouse.motion.dx += mouse->lLastX;
    s_Mouse.motion.dy += mouse->lLastY;

    // left
    if (flags & RI_MOUSE_LEFT_BUTTON_DOWN) {
        s_Mouse.state[PAL_MOUSE_BUTTON_LEFT] = true;
    } else {
        s_Mouse.state[PAL_MOUSE_BUTTON_LEFT] = false;
    }

    // right
    if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
        s_Mouse.state[PAL_MOUSE_BUTTON_RIGHT] = true;
    } else {
        s_Mouse.state[PAL_MOUSE_BUTTON_RIGHT] = false;
    }

    // middle
    if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
        s_Mouse.state[PAL_MOUSE_BUTTON_MIDDLE] = true;
    } else {
        s_Mouse.state[PAL_MOUSE_BUTTON_MIDDLE] = false;
    }

    // x1
    if (flags & RI_MOUSE_BUTTON_4_DOWN) {
        s_Mouse.state[PAL_MOUSE_BUTTON_X1] = true;
    } else {
        s_Mouse.state[PAL_MOUSE_BUTTON_X1] = false;
    }

    // x2
    if (flags & RI_MOUSE_BUTTON_5_DOWN) {
        s_Mouse.state[PAL_MOUSE_BUTTON_X2] = true;
    } else {
        s_Mouse.state[PAL_MOUSE_BUTTON_X2] = false;
    }

    // wheel delta
    if (flags & RI_MOUSE_WHEEL) {
        SHORT delta = (SHORT)HIWORD(mouse->usButtonData);
        s_Mouse.wheel.y += (delta / WHEEL_DELTA);
    }

    if (flags & RI_MOUSE_HWHEEL) {
        SHORT delta = (SHORT)HIWORD(mouse->usButtonData);
        s_Mouse.wheel.x += (delta / WHEEL_DELTA);
    }
}

LRESULT CALLBACK inputProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    PalInputSystem* input = GetPropW(hwnd, PAL_WIN32_INPUT_PROP);
    if (!input) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

     switch (msg) {
        case WM_INPUT: {
            UINT size;
            GetRawInputData(
                (HRAWINPUT)lParam, 
                RID_INPUT, 
                NULL, 
                &size, 
                sizeof(RAWINPUTHEADER)
            );

            BYTE buffer[sizeof(RAWINPUT)];
            if (GetRawInputData(
                (HRAWINPUT)lParam, 
                RID_INPUT, 
                buffer, 
                &size, 
                sizeof(RAWINPUTHEADER)
            ) != size) {
                break;
            }

            RAWINPUT* raw = (RAWINPUT*)buffer;
            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                // keyboard
                handleKeyboardInput(&raw->data.keyboard);
            }

            if (raw->header.dwType == RIM_TYPEMOUSE) {
                // mouse
                handleMouseInput(&raw->data.mouse);
            }
            break;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}