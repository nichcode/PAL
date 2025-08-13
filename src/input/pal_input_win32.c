
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


void getHidProperties(PalInputDeviceInfo* info);

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

// ==================================================
// Internal API
// ==================================================

void getHidProperties(PalInputDeviceInfo* info) {

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

LRESULT CALLBACK inputProc(
    HWND hwnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam) {

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}