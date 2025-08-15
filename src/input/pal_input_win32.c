
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
#include <time.h>

#define PAL_WIN32_INPUT_CLASS L"PALInputClass"

#define XINPUT_1_4 "xinput1_4.dll"
#define XINPUT_1_3 "xinput1_3.dll"
#define XINPUT_9_1_0 "xinput9_1_0.dll"
#define XINPUT_COUNT 4
#define XINPUT_TAG 0xFFFF0000

#define MAX_DEVICES 32

typedef DWORD (WINAPI* XInputGetStateFn)(DWORD, XINPUT_STATE*);
typedef DWORD (WINAPI* XInputSetStateFn)(DWORD, XINPUT_VIBRATION*);

typedef BOOLEAN (WINAPI* HidD_GetProductStringFn)(HANDLE, PVOID, ULONG);
typedef BOOLEAN (WINAPI* HidD_GetManufacturerStringFn)(HANDLE, PVOID, ULONG);
typedef BOOLEAN (WINAPI* HidD_GetAttributesFn)(HANDLE, PHIDD_ATTRIBUTES);
typedef void (WINAPI* HidD_GetGuidFn)(GUID*);

typedef struct InputSystemWin32 {
    PalAllocator* allocator;
    HINSTANCE instance;
    HINSTANCE xInput;
    HINSTANCE hid;
    GUID hidGuid;
    HWND window;
    HDEVNOTIFY notifyHandle;
    PalEventDriver* eventDriver;

    HidD_GetProductStringFn hidD_GetProductString;
    HidD_GetManufacturerStringFn hidD_GetManufacturerString;
    HidD_GetAttributesFn hidD_GetAttributes;
    HidD_GetGuidFn hidD_GetGuid;

    XInputGetStateFn getXinputState;
    XInputSetStateFn setXinputState;
    bool initialized;
} InputSystemWin32;

typedef struct Keycodes {
    PalScancode scancodes[512];
    PalKey keycodes[256];
} Keycodes;

typedef struct MousePosition {
    Int32 x, y;
    Int32 dx, dy;
} MousePosition;

typedef struct RegisteredDevice {
    HANDLE device;
    PalInputDeviceType type;
    Uint64 id;

    union {
        struct {
            bool scancodeState[PAL_SCANCODE_MAX];
            bool keyState[PAL_KEY_MAX];
        } keyboard;

        struct {
            PalMouseWheel wheel;
            bool state[PAL_MOUSE_BUTTON_MAX];
        } mouse;
    };
} RegisteredDevice;

static Keycodes s_Keycodes = {};
static MousePosition s_Mouse;
static InputSystemWin32 s_System = {};
static RegisteredDevice s_Devices[MAX_DEVICES] = {};
static Int16 s_DeviceCount = 0, s_Counter = 200;

static bool getHidProperties(PalInputDeviceInfo* info);
static bool parseDevicePath(PalInputDeviceInfo* info);

static void createKeyTable();
static void createScancodeTable();
static Uint64 generateDeviceID();

static PalResult registerRawDevice(
    PalInputDevice* inputDevice,
    bool remove);

static void handleKeyboardInput(
    RegisteredDevice* device, 
    RAWKEYBOARD* keyboard);

static void handleMouseInput(
    RegisteredDevice* device,
    RAWMOUSE* mouse);

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

PalResult _PCALL palInitInput(
    PalAllocator* allocator,
    PalEventDriver* eventDriver) {

    if (allocator && (allocator->allocate || allocator->free)) {
        return PAL_RESULT_INVALID_ALLOCATOR;
    }

    s_System.allocator = allocator;
    s_System.eventDriver = eventDriver;

    s_System.instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    if (!GetClassInfoExW(s_System.instance, PAL_WIN32_INPUT_CLASS, &wc)) {
        wc.lpfnWndProc = inputProc;
        wc.hInstance = s_System.instance;
        wc.lpszClassName = PAL_WIN32_INPUT_CLASS;

        if (!RegisterClassExW(&wc)) {
            return PAL_RESULT_ACCESS_DENIED;
        }
    }

    s_System.window = CreateWindowExW(
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
        s_System.instance,
        nullptr
    );

    if (!s_System.window) {
        return PAL_RESULT_PLATFORM_FAILURE;
    }

    // load libraries
    if (!s_System.initialized) {
        // Xinput
        s_System.xInput = LoadLibraryA(XINPUT_1_4);
        if (!s_System.xInput) {
            s_System.xInput = LoadLibraryA(XINPUT_1_3);

        } if (!s_System.xInput) {
            s_System.xInput = LoadLibraryA(XINPUT_9_1_0);
        }

        if (s_System.xInput) {
            s_System.getXinputState = (XInputGetStateFn)GetProcAddress(
                s_System.xInput,
                "XInputGetState"
            );

            s_System.setXinputState = (XInputSetStateFn)GetProcAddress(
                s_System.xInput,
                "XInputSetState"
            );
        }

        // hid
        s_System.hid = LoadLibraryA("hid.dll");
        if (s_System.hid) {
            s_System.hidD_GetAttributes = (HidD_GetAttributesFn)GetProcAddress(
                s_System.hid,
                "HidD_GetAttributes"
            );

            s_System.hidD_GetManufacturerString = (HidD_GetManufacturerStringFn)GetProcAddress(
                s_System.hid,
                "HidD_GetManufacturerString"
            );

            s_System.hidD_GetProductString = (HidD_GetProductStringFn)GetProcAddress(
                s_System.hid,
                "HidD_GetProductString"
            );

            s_System.hidD_GetGuid = (HidD_GetGuidFn)GetProcAddress(
                s_System.hid,
                "HidD_GetHidGuid"
            );
        }

        createScancodeTable();
        createKeyTable();

        // register hid notifications
        if (s_System.hidD_GetGuid) {
            s_System.hidD_GetGuid(&s_System.hidGuid);

            DEV_BROADCAST_DEVICEINTERFACE_W filter = {0};
            filter.dbcc_size = sizeof(filter);
            filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
            filter.dbcc_classguid = s_System.hidGuid;

            s_System.notifyHandle = RegisterDeviceNotificationW(
                s_System.window,
                &filter,
                DEVICE_NOTIFY_WINDOW_HANDLE
            );

            ShowWindow(s_System.window, SW_HIDE);
        }

        RAWINPUTDEVICE rid = {};
        rid.dwFlags = RIDEV_INPUTSINK;
        rid.hwndTarget = s_System.window;
        rid.usUsage = 0x06; 
        rid.usUsagePage = 0x01;

        if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        rid.usUsage = 0x02; 
        if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
            return PAL_RESULT_PLATFORM_FAILURE;
        }

        s_System.initialized = true;     
    }
    return PAL_RESULT_SUCCESS;
}

void _PCALL palShutdownInput() {

    if (!s_System.initialized) {
        return;
    }

    DestroyWindow(s_System.window);
    UnregisterClassW(PAL_WIN32_INPUT_CLASS, s_System.instance);
    UnregisterDeviceNotification(s_System.notifyHandle);
    s_System.initialized = false;
}

void _PCALL palUpdateInput() {

    if (!s_System.initialized) {
        return;
    }

    s_Mouse.dx = s_Mouse.dy = 0;
    POINT pos;
    GetCursorPos(&pos);
    s_Mouse.x = pos.x;
    s_Mouse.y = pos.y;

    MSG msg;
    while (PeekMessageA(&msg, s_System.window, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

PalResult _PCALL palEnumerateInputDevices(
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
        s_System.allocator, 
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
            return PAL_RESULT_PLATFORM_FAILURE;
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
                    if (usagePage == 0x01 && (usage == 0x05 || usage == 0x04)) {
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

    palFree(s_System.allocator, devices);

    // Xnput devices (Xbox controllers)
    if (mask & PAL_INPUT_MASK_GAMEPAD) {
        if (s_System.xInput) {
            XINPUT_STATE state;
            for (DWORD i = 0; i < XINPUT_COUNT; i++) {
                if (s_System.getXinputState(i, &state) == ERROR_SUCCESS) {
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
                if (!getHidProperties(info)) {
                    strcpy(info->name, "Keyboard");
                    if (!parseDevicePath(info)) {
                        strcpy(info->name, "Standard Keyboard");
                        info->productID = 0;
                        info->vendorID = 0;
                    }
                }
                info->type = PAL_INPUT_DEVICE_KEYBOARD;                
                break;
            }

            case RIM_TYPEMOUSE: {
                if (!getHidProperties(info)) {
                    strcpy(info->name, "Mouse");
                    if (!parseDevicePath(info)) {
                        strcpy(info->name, "Standard Mouse");
                        info->productID = 0;
                        info->vendorID = 0;
                    }
                }
                info->type = PAL_INPUT_DEVICE_MOUSE;                
                break;
            }

            case RIM_TYPEHID: {
                Int32 usagePage = deviceInfo.hid.usUsagePage;
                Int32 usage = deviceInfo.hid.usUsage;

                if (usagePage == 0x01 && usage == 0x06) {
                    if (!getHidProperties(info)) {
                        strcpy(info->name, "Keyboard");
                        if (!parseDevicePath(info)) {
                            strcpy(info->name, "Standard Keyboard");
                            info->productID = 0;
                            info->vendorID = 0;
                        }
                    }
                    info->type = PAL_INPUT_DEVICE_KEYBOARD;  
                    break;

                } else if (usagePage == 0x01 && usage == 0x02) {
                    if (!getHidProperties(info)) {
                        strcpy(info->name, "Mouse");
                        if (!parseDevicePath(info)) {
                            strcpy(info->name, "Standard Mouse");
                            info->productID = 0;
                            info->vendorID = 0;
                        }
                    }
                    info->type = PAL_INPUT_DEVICE_MOUSE;  
                    break;

                } else if (usagePage == 0x01 && (usage == 0x05 || usage == 0x04)) {
                    if (!getHidProperties(info)) {
                        if (!parseDevicePath(info)) {
                            info->productID = 0;
                            info->vendorID = 0;
                        }
                        strcpy(info->name, "Generic Gamepad");
                        info->type = PAL_INPUT_DEVICE_GENERIC_GAMEPAD;  
                    }
                    break;
                }
            }
        }
    }
    return PAL_RESULT_SUCCESS;
}

PalResult _PCALL palRegisterInputDevice(PalInputDevice* inputDevice) {

    if (!inputDevice) {
        return PAL_RESULT_NULL_POINTER;
    }

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

    for (Int32 i = 0; i < MAX_DEVICES; i++) {
        RegisteredDevice* device = &s_Devices[i];
        if (device->device) {
            continue;
        }

        device->device = (HANDLE)inputDevice;
        device->id = generateDeviceID();
        switch (info.dwType) {
            case RIM_TYPEKEYBOARD: {
                device->type = PAL_INPUT_DEVICE_KEYBOARD;
                break;
            }

            case RIM_TYPEMOUSE: {
                device->type = PAL_INPUT_DEVICE_MOUSE;
                break;
            }

            case RIM_TYPEHID: {
                // we set to generic gamepad cause we use to poll state
                device->type = PAL_INPUT_DEVICE_GENERIC_GAMEPAD;
                break;
            }
        }
        s_DeviceCount++;
        return PAL_RESULT_SUCCESS;
    }
    // rare
    return PAL_RESULT_PLATFORM_FAILURE;
}

PalResult _PCALL palUnregisterInputDevice(PalInputDevice* inputDevice) {
    
    if (!inputDevice) {
        return PAL_RESULT_NULL_POINTER;
    }

    for (Int32 i = 0; i < MAX_DEVICES; i++) {
        RegisteredDevice* device = &s_Devices[i];
        if (device->device == (HANDLE)inputDevice) {
            device->device = nullptr;
            device->type = PAL_INPUT_DEVICE_UNKNOWN;
            s_DeviceCount--;
            break;
        }
    }

    return PAL_RESULT_INPUT_DEVICE_NOT_REGISTERED;
}

void palGetMousePosition(Int32* x, Int32* y) {

    if (x) {
        *x = s_Mouse.x;
    }

    if (y) {
        *y = s_Mouse.y;
    }
}

void palGetMouseRelative(Int32* x, Int32* y) {

    if (x) {
        *x = s_Mouse.dx;
    }

    if (y) {
        *y = s_Mouse.dy;
    }
}

void _PCALL palGetKeyboardState(
    PalInputDevice* keyboard, 
    PalKeyboardState* state) {
    
    if (!state || !keyboard) {
        return;
    }

    for (Int32 i = 0; i < MAX_DEVICES; i++) {
        RegisteredDevice* device = &s_Devices[i];
        if (device->type == PAL_INPUT_DEVICE_KEYBOARD) {
                if (device->device == (HANDLE)keyboard) {
                state->keys = device->keyboard.keyState;
                state->scancodes = device->keyboard.scancodeState;
                return;
            }
        }
    }
}

void _PCALL palGetMouseState(
    PalInputDevice* mouse, 
    PalMouseState* state) {
    
    if (!state || !mouse) {
        return;
    }

    for (Int32 i = 0; i < MAX_DEVICES; i++) {
        RegisteredDevice* device = &s_Devices[i];
        if (device->type == PAL_INPUT_DEVICE_MOUSE) {
                if (device->device == (HANDLE)mouse) {
                state->buttons = device->mouse.state;
                state->wheel = &device->mouse.wheel;
                return;
            }
        }
    }
}


// ==================================================
// Internal API
// ==================================================

static bool getHidProperties(PalInputDeviceInfo* info) {

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
        return false;
    }

    WCHAR manufacturer[64] = {};
    WCHAR product[64] = {};
    bool sucessManufacturer = s_System.hidD_GetManufacturerString(
        file,
        manufacturer,
        sizeof(manufacturer)
    );

    bool sucessproduct = s_System.hidD_GetProductString(
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
    }

    // get attributes
    HIDD_ATTRIBUTES attrs = {};
    attrs.Size = sizeof(HIDD_ATTRIBUTES);
    if (!s_System.hidD_GetAttributes(file, &attrs)) {
        CloseHandle(file);
        return false;
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
    return true;
}

static bool parseDevicePath(PalInputDeviceInfo* info) {

    const char* vidString = strstr(info->path, "VID_");
    const char* pidString = strstr(info->path, "PID_");

    if (!vidString || !pidString) {
        return false;
    }

    info->vendorID = (Uint16)strtoul(vidString + 4, nullptr, 16);
    info->productID = (Uint16)strtoul(pidString + 4, nullptr, 16);
    return true;
}

static void createKeyTable() {

    // Letters
    s_Keycodes.keycodes['A'] = PAL_KEY_A;
    s_Keycodes.keycodes['B'] = PAL_KEY_B;
    s_Keycodes.keycodes['C'] = PAL_KEY_C;
    s_Keycodes.keycodes['D'] = PAL_KEY_D;
    s_Keycodes.keycodes['E'] = PAL_KEY_E;
    s_Keycodes.keycodes['F'] = PAL_KEY_F;
    s_Keycodes.keycodes['G'] = PAL_KEY_G;
    s_Keycodes.keycodes['H'] = PAL_KEY_H;
    s_Keycodes.keycodes['I'] = PAL_KEY_I;
    s_Keycodes.keycodes['J'] = PAL_KEY_J;
    s_Keycodes.keycodes['K'] = PAL_KEY_K;
    s_Keycodes.keycodes['L'] = PAL_KEY_L;
    s_Keycodes.keycodes['M'] = PAL_KEY_M;
    s_Keycodes.keycodes['N'] = PAL_KEY_N;
    s_Keycodes.keycodes['O'] = PAL_KEY_O;
    s_Keycodes.keycodes['P'] = PAL_KEY_P;
    s_Keycodes.keycodes['Q'] = PAL_KEY_Q;
    s_Keycodes.keycodes['R'] = PAL_KEY_R;
    s_Keycodes.keycodes['S'] = PAL_KEY_S;
    s_Keycodes.keycodes['T'] = PAL_KEY_T;
    s_Keycodes.keycodes['U'] = PAL_KEY_U;
    s_Keycodes.keycodes['V'] = PAL_KEY_V;
    s_Keycodes.keycodes['W'] = PAL_KEY_W;
    s_Keycodes.keycodes['Z'] = PAL_KEY_Z;
    s_Keycodes.keycodes['Y'] = PAL_KEY_Y;
    s_Keycodes.keycodes['Z'] = PAL_KEY_Z;

    // Numbers (top row)
    s_Keycodes.keycodes['0'] = PAL_KEY_0;
    s_Keycodes.keycodes['1'] = PAL_KEY_1;
    s_Keycodes.keycodes['2'] = PAL_KEY_2;
    s_Keycodes.keycodes['3'] = PAL_KEY_3;
    s_Keycodes.keycodes['4'] = PAL_KEY_4;
    s_Keycodes.keycodes['5'] = PAL_KEY_5;
    s_Keycodes.keycodes['6'] = PAL_KEY_6;
    s_Keycodes.keycodes['7'] = PAL_KEY_7;
    s_Keycodes.keycodes['8'] = PAL_KEY_8;
    s_Keycodes.keycodes['9'] = PAL_KEY_9;

    // Function
    s_Keycodes.keycodes[VK_F1] = PAL_KEY_F1;
    s_Keycodes.keycodes[VK_F2] = PAL_KEY_F2;
    s_Keycodes.keycodes[VK_F3] = PAL_KEY_F3;
    s_Keycodes.keycodes[VK_F4] = PAL_KEY_F4;
    s_Keycodes.keycodes[VK_F5] = PAL_KEY_F5;
    s_Keycodes.keycodes[VK_F6] = PAL_KEY_F6;
    s_Keycodes.keycodes[VK_F7] = PAL_KEY_F7;
    s_Keycodes.keycodes[VK_F8] = PAL_KEY_F8;
    s_Keycodes.keycodes[VK_F9] = PAL_KEY_F9;
    s_Keycodes.keycodes[VK_F10] = PAL_KEY_F10;
    s_Keycodes.keycodes[VK_F11] = PAL_KEY_F11;
    s_Keycodes.keycodes[VK_F12] = PAL_KEY_F12;

    // Control
    s_Keycodes.keycodes[VK_ESCAPE] = PAL_KEY_ESCAPE;
    s_Keycodes.keycodes[VK_RETURN] = PAL_KEY_ENTER;
    s_Keycodes.keycodes[VK_TAB] = PAL_KEY_TAB;
    s_Keycodes.keycodes[VK_BACK] = PAL_KEY_BACKSPACE;
    s_Keycodes.keycodes[VK_SPACE] = PAL_KEY_SPACE;
    s_Keycodes.keycodes[VK_CAPITAL] = PAL_KEY_CAPSLOCK;
    s_Keycodes.keycodes[VK_NUMLOCK] = PAL_KEY_NUMLOCK;
    s_Keycodes.keycodes[VK_SCROLL] = PAL_KEY_SCROLLLOCK;
    s_Keycodes.keycodes[VK_LSHIFT] = PAL_KEY_LSHIFT;
    s_Keycodes.keycodes[VK_RSHIFT] = PAL_KEY_RSHIFT;
    s_Keycodes.keycodes[VK_LCONTROL] = PAL_KEY_LCTRL;
    s_Keycodes.keycodes[VK_RCONTROL] = PAL_KEY_RCTRL;
    s_Keycodes.keycodes[VK_LMENU] = PAL_KEY_LALT;
    s_Keycodes.keycodes[VK_RMENU] = PAL_KEY_RALT;

    // Arrows
    s_Keycodes.keycodes[VK_LEFT] = PAL_KEY_LEFT;
    s_Keycodes.keycodes[VK_RIGHT] = PAL_KEY_RIGHT;
    s_Keycodes.keycodes[VK_UP] = PAL_KEY_UP;
    s_Keycodes.keycodes[VK_DOWN] = PAL_KEY_DOWN;

    // Navigation
    s_Keycodes.keycodes[VK_INSERT] = PAL_KEY_INSERT;
    s_Keycodes.keycodes[VK_DELETE] = PAL_KEY_DELETE;
    s_Keycodes.keycodes[VK_HOME] = PAL_KEY_HOME;
    s_Keycodes.keycodes[VK_END] = PAL_KEY_END;
    s_Keycodes.keycodes[VK_PRIOR] = PAL_KEY_PAGEUP;
    s_Keycodes.keycodes[VK_NEXT] = PAL_KEY_PAGEDOWN;

    // Keypad
    s_Keycodes.keycodes[VK_NUMPAD0] = PAL_KEY_KP_0;
    s_Keycodes.keycodes[VK_NUMPAD1] = PAL_KEY_KP_1;
    s_Keycodes.keycodes[VK_NUMPAD2] = PAL_KEY_KP_2;
    s_Keycodes.keycodes[VK_NUMPAD3] = PAL_KEY_KP_3;
    s_Keycodes.keycodes[VK_NUMPAD4] = PAL_KEY_KP_4;
    s_Keycodes.keycodes[VK_NUMPAD5] = PAL_KEY_KP_5;
    s_Keycodes.keycodes[VK_NUMPAD6] = PAL_KEY_KP_6;
    s_Keycodes.keycodes[VK_NUMPAD7] = PAL_KEY_KP_7;
    s_Keycodes.keycodes[VK_NUMPAD8] = PAL_KEY_KP_8;
    s_Keycodes.keycodes[VK_NUMPAD9] = PAL_KEY_KP_9;
    
    s_Keycodes.keycodes[VK_ADD] = PAL_KEY_KP_ADD;
    s_Keycodes.keycodes[VK_SUBTRACT] = PAL_KEY_KP_SUBTRACT;
    s_Keycodes.keycodes[VK_MULTIPLY] = PAL_KEY_KP_MULTIPLY;
    s_Keycodes.keycodes[VK_DIVIDE] = PAL_KEY_KP_DIVIDE;
    s_Keycodes.keycodes[VK_DECIMAL] = PAL_KEY_KP_DECIMAL;

    // Misc
    s_Keycodes.keycodes[VK_SNAPSHOT] = PAL_KEY_PRINTSCREEN;
    s_Keycodes.keycodes[VK_PAUSE] = PAL_KEY_PAUSE;    
    s_Keycodes.keycodes[VK_APPS] = PAL_KEY_MENU;
    s_Keycodes.keycodes[VK_OEM_7] = PAL_KEY_APOSTROPHE;
    s_Keycodes.keycodes[VK_OEM_5] = PAL_KEY_BACKSLASH;
    s_Keycodes.keycodes[VK_OEM_COMMA] = PAL_KEY_COMMA;
    s_Keycodes.keycodes[VK_OEM_PLUS] = PAL_KEY_EQUAL;
    s_Keycodes.keycodes[VK_OEM_3] = PAL_KEY_GRAVEACCENT;
    s_Keycodes.keycodes[VK_OEM_MINUS] = PAL_KEY_SUBTRACT;
    s_Keycodes.keycodes[VK_OEM_PERIOD] = PAL_KEY_PERIOD;
    s_Keycodes.keycodes[VK_OEM_1] = PAL_KEY_SEMICOLON;
    s_Keycodes.keycodes[VK_OEM_2] = PAL_KEY_SLASH;
    s_Keycodes.keycodes[VK_OEM_4] = PAL_KEY_LBRACKET;
    s_Keycodes.keycodes[VK_OEM_6] = PAL_KEY_RBRACKET;
    s_Keycodes.keycodes[VK_LWIN] = PAL_KEY_LSUPER;
    s_Keycodes.keycodes[VK_RWIN] = PAL_KEY_RSUPER;
}

static void createScancodeTable() {

    // Scancodes are made from OR'ed (scancode | extended)
    // Letters
    s_Keycodes.scancodes[0x01E] = PAL_SCANCODE_A;
    s_Keycodes.scancodes[0x030] = PAL_SCANCODE_B;
    s_Keycodes.scancodes[0x02E] = PAL_SCANCODE_C;
    s_Keycodes.scancodes[0x020] = PAL_SCANCODE_D;
    s_Keycodes.scancodes[0x012] = PAL_SCANCODE_E;
    s_Keycodes.scancodes[0x021] = PAL_SCANCODE_F;
    s_Keycodes.scancodes[0x022] = PAL_SCANCODE_G;
    s_Keycodes.scancodes[0x023] = PAL_SCANCODE_H;
    s_Keycodes.scancodes[0x017] = PAL_SCANCODE_I;
    s_Keycodes.scancodes[0x024] = PAL_SCANCODE_J;
    s_Keycodes.scancodes[0x025] = PAL_SCANCODE_K;
    s_Keycodes.scancodes[0x026] = PAL_SCANCODE_L;
    s_Keycodes.scancodes[0x032] = PAL_SCANCODE_M;
    s_Keycodes.scancodes[0x031] = PAL_SCANCODE_N;
    s_Keycodes.scancodes[0x018] = PAL_SCANCODE_O;
    s_Keycodes.scancodes[0x019] = PAL_SCANCODE_P;
    s_Keycodes.scancodes[0x010] = PAL_SCANCODE_Q;
    s_Keycodes.scancodes[0x013] = PAL_SCANCODE_R;
    s_Keycodes.scancodes[0x01F] = PAL_SCANCODE_S;
    s_Keycodes.scancodes[0x014] = PAL_SCANCODE_T;
    s_Keycodes.scancodes[0x016] = PAL_SCANCODE_U;
    s_Keycodes.scancodes[0x02F] = PAL_SCANCODE_V;
    s_Keycodes.scancodes[0x011] = PAL_SCANCODE_W;
    s_Keycodes.scancodes[0x02D] = PAL_SCANCODE_X;
    s_Keycodes.scancodes[0x015] = PAL_SCANCODE_Y;
    s_Keycodes.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    s_Keycodes.scancodes[0x00B] = PAL_SCANCODE_0;
    s_Keycodes.scancodes[0x002] = PAL_SCANCODE_1;
    s_Keycodes.scancodes[0x003] = PAL_SCANCODE_2;
    s_Keycodes.scancodes[0x004] = PAL_SCANCODE_3;
    s_Keycodes.scancodes[0x005] = PAL_SCANCODE_4;
    s_Keycodes.scancodes[0x006] = PAL_SCANCODE_5;
    s_Keycodes.scancodes[0x007] = PAL_SCANCODE_6;
    s_Keycodes.scancodes[0x008] = PAL_SCANCODE_7;
    s_Keycodes.scancodes[0x009] = PAL_SCANCODE_8;
    s_Keycodes.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    s_Keycodes.scancodes[0x03B] = PAL_SCANCODE_F1;
    s_Keycodes.scancodes[0x03C] = PAL_SCANCODE_F2;
    s_Keycodes.scancodes[0x03D] = PAL_SCANCODE_F3;
    s_Keycodes.scancodes[0x03E] = PAL_SCANCODE_F4;
    s_Keycodes.scancodes[0x03F] = PAL_SCANCODE_F5;
    s_Keycodes.scancodes[0x040] = PAL_SCANCODE_F6;
    s_Keycodes.scancodes[0x041] = PAL_SCANCODE_F7;
    s_Keycodes.scancodes[0x042] = PAL_SCANCODE_F8;
    s_Keycodes.scancodes[0x043] = PAL_SCANCODE_F9;
    s_Keycodes.scancodes[0x044] = PAL_SCANCODE_F10;
    s_Keycodes.scancodes[0x057] = PAL_SCANCODE_F11;
    s_Keycodes.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    s_Keycodes.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    s_Keycodes.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    s_Keycodes.scancodes[0x00F] = PAL_SCANCODE_TAB;
    s_Keycodes.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    s_Keycodes.scancodes[0x039] = PAL_SCANCODE_SPACE;
    s_Keycodes.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    s_Keycodes.scancodes[0x145] = PAL_SCANCODE_NUMLOCK;
    s_Keycodes.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    s_Keycodes.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    s_Keycodes.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    s_Keycodes.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    s_Keycodes.scancodes[0x11D] = PAL_SCANCODE_RCTRL;
    s_Keycodes.scancodes[0x038] = PAL_SCANCODE_LALT;
    s_Keycodes.scancodes[0x138] = PAL_SCANCODE_RALT;

    // Arrows
    s_Keycodes.scancodes[0x14B] = PAL_SCANCODE_LEFT;
    s_Keycodes.scancodes[0x14D] = PAL_SCANCODE_RIGHT;
    s_Keycodes.scancodes[0x148] = PAL_SCANCODE_UP;
    s_Keycodes.scancodes[0x150] = PAL_SCANCODE_DOWN;

    // Navigation
    s_Keycodes.scancodes[0x152] = PAL_SCANCODE_INSERT;
    s_Keycodes.scancodes[0x153] = PAL_SCANCODE_DELETE;
    s_Keycodes.scancodes[0x147] = PAL_SCANCODE_HOME;
    s_Keycodes.scancodes[0x14F] = PAL_SCANCODE_END;
    s_Keycodes.scancodes[0x149] = PAL_SCANCODE_PAGEUP;
    s_Keycodes.scancodes[0x151] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    s_Keycodes.scancodes[0x052] = PAL_SCANCODE_KP_0;
    s_Keycodes.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    s_Keycodes.scancodes[0x050] = PAL_SCANCODE_KP_2;
    s_Keycodes.scancodes[0x051] = PAL_SCANCODE_KP_3;
    s_Keycodes.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    s_Keycodes.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    s_Keycodes.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    s_Keycodes.scancodes[0x047] = PAL_SCANCODE_KP_7;
    s_Keycodes.scancodes[0x048] = PAL_SCANCODE_KP_8;
    s_Keycodes.scancodes[0x049] = PAL_SCANCODE_KP_9;
    s_Keycodes.scancodes[0x11C] = PAL_SCANCODE_KP_ENTER;
    s_Keycodes.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    s_Keycodes.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    s_Keycodes.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    s_Keycodes.scancodes[0x135] = PAL_SCANCODE_KP_DIVIDE;
    s_Keycodes.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;
    s_Keycodes.scancodes[0x059] = PAL_SCANCODE_KP_EQUAL;

    // Misc
    s_Keycodes.scancodes[0x137] = PAL_SCANCODE_PRINTSCREEN;
    s_Keycodes.scancodes[0x146] = PAL_SCANCODE_PAUSE;    
    s_Keycodes.scancodes[0x045] = PAL_SCANCODE_PAUSE;
    s_Keycodes.scancodes[0x15D] = PAL_SCANCODE_MENU;
    s_Keycodes.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    s_Keycodes.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    s_Keycodes.scancodes[0x033] = PAL_SCANCODE_COMMA;
    s_Keycodes.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    s_Keycodes.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    s_Keycodes.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    s_Keycodes.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    s_Keycodes.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    s_Keycodes.scancodes[0x035] = PAL_SCANCODE_SLASH;
    s_Keycodes.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    s_Keycodes.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    s_Keycodes.scancodes[0x15B] = PAL_SCANCODE_LSUPER;
    s_Keycodes.scancodes[0x15C] = PAL_SCANCODE_RSUPER;
}

static Uint64 generateDeviceID() {

    Uint32 timeStamp = (Uint32)time(nullptr);
    Uint32 local = s_Counter++;
    return ((Uint64)timeStamp << 32) | local;
}

static void handleKeyboardInput(
    RegisteredDevice* device, 
    RAWKEYBOARD* keyboard) {
    
    PalScancode scancode = PAL_SCANCODE_UNKNOWN;
    PalKey key = PAL_KEY_UNKNOWN;
    bool extended = (keyboard->Flags & RI_KEY_E0) != 0;

    if (!extended && keyboard->MakeCode == 0x045) {
        scancode = PAL_SCANCODE_NUMLOCK;

    } else {
        Uint16 index = keyboard->MakeCode | (extended << 8);
        scancode = s_Keycodes.scancodes[index];
    }

    if (extended && keyboard->VKey == VK_RETURN) {
        key = PAL_KEY_KP_ENTER;

    } else if (extended && keyboard->VKey == VK_OEM_PLUS) {
        key = PAL_KEY_KP_EQUAL;

    } else {
        key = s_Keycodes.keycodes[keyboard->VKey];
    }

    bool isKeyDown = !(keyboard->Flags & RI_KEY_BREAK);
    bool keyRepeat = device->keyboard.keyState[key];
    bool scancodeRepeat = device->keyboard.scancodeState[scancode];

    if (keyboard->Flags & RI_KEY_BREAK) {
        device->keyboard.keyState[key] = false;
        device->keyboard.scancodeState[scancode] = false;
        
    } else {
        device->keyboard.keyState[key] = true;
        device->keyboard.scancodeState[scancode] = true;  
    }
}

static void handleMouseInput(
    RegisteredDevice* device, 
    RAWMOUSE* mouse) {

    USHORT flags = mouse->usButtonFlags;
    s_Mouse.dx += mouse->lLastX;
    s_Mouse.dy += mouse->lLastY;

    // left
    if (flags & RI_MOUSE_LEFT_BUTTON_DOWN) {
        device->mouse.state[PAL_MOUSE_BUTTON_LEFT] = true;
    } else {
        device->mouse.state[PAL_MOUSE_BUTTON_LEFT] = false;
    }

    // right
    if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
        device->mouse.state[PAL_MOUSE_BUTTON_RIGHT] = true;
    } else {
        device->mouse.state[PAL_MOUSE_BUTTON_RIGHT] = false;
    }

    // middle
    if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
        device->mouse.state[PAL_MOUSE_BUTTON_MIDDLE] = true;
    } else {
        device->mouse.state[PAL_MOUSE_BUTTON_MIDDLE] = false;
    }

    // x1
    if (flags & RI_MOUSE_BUTTON_4_DOWN) {
        device->mouse.state[PAL_MOUSE_BUTTON_X1] = true;
    } else {
        device->mouse.state[PAL_MOUSE_BUTTON_X1] = false;
    }

    // x2
    if (flags & RI_MOUSE_BUTTON_5_DOWN) {
        device->mouse.state[PAL_MOUSE_BUTTON_X2] = true;
    } else {
        device->mouse.state[PAL_MOUSE_BUTTON_X2] = false;
    }

    // wheel delta
    if (flags & RI_MOUSE_WHEEL) {
        SHORT delta = (SHORT)HIWORD(mouse->usButtonData);
        device->mouse.wheel.y += (delta / WHEEL_DELTA);
    }

    if (flags & RI_MOUSE_HWHEEL) {
        SHORT delta = (SHORT)HIWORD(mouse->usButtonData);
        device->mouse.wheel.x += (delta / WHEEL_DELTA);
    }
}

LRESULT CALLBACK inputProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    if (!s_System.initialized) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

     switch (msg) {
        case WM_INPUT: {
            if (!s_DeviceCount) {
                break;
            }

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

            // find input device and check if its registered
            RAWINPUT* raw = (RAWINPUT*)buffer;
            for (Int32 i = 0; i < MAX_DEVICES; i++) {
                RegisteredDevice* device = &s_Devices[i];
                if (device->device == raw->header.hDevice) {
                    if (device->type == PAL_INPUT_DEVICE_KEYBOARD) {
                        // keyboard device
                        handleKeyboardInput(device, &raw->data.keyboard);
                    } else if (device->type == PAL_INPUT_DEVICE_MOUSE) {
                        // mouse device
                        handleMouseInput(device, &raw->data.mouse);
                    }
                }
            }
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}