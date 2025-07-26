
#include "pal_pch.h"
#include "input/pal_input_internal.h"
#include "pal_win32input.h"

#define XINPUT_1_4 "xinput1_4.dll"
#define XINPUT_1_3 "xinput1_3.dll"
#define XINPUT_9_1_0 "xinput9_1_0.dll"

// ==================================================
// Public API
// ==================================================

PalResult _PCALL palUpdateInput(
    PalInput input) {

    if (!input) {
        return PAL_ERROR_NULL_POINTER;
    }

    // reset
    input->mouseDX = 0;
    input->mouseDY = 0;
    input->mouseWheelDeltaX = 0;
    input->mouseWheelDeltaY = 0;

    InputDataWin32* data = input->platformData;
    MSG msg;
    while (PeekMessageA(&msg, data->window, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return PAL_SUCCESS;
}

PalResult _PCALL palEnumerateInputDevices(
    PalInput input,
    int* count,
    PalInputDevice* inputDevices) {

    if (!input || !count) {
        return PAL_ERROR_NULL_POINTER;
    }

    InputDataWin32* data = input->platformData;
    Uint32 deviceCount = 0;
    Uint32 maxDeviceCount = inputDevices ? *count : 0;

    // load function pointers
    if (!g_HidData.loaded) {
        g_HidData.getAttributes = (GetAttributesFn)GetProcAddress(
            data->hid,
            "HidD_GetAttributes"
        );

         g_HidData.getManufacturerString = (GetManufacturerStringFn)GetProcAddress(
            data->hid,
            "HidD_GetManufacturerString"
        );

        g_HidData.getProductString = (GetProductStringFn)GetProcAddress(
            data->hid,
            "HidD_GetProductString"
        );

        g_HidData.loaded = PAL_TRUE;

    }

    // get count
    GetRawInputDeviceList(
        PAL_NULL, 
        &deviceCount, 
        sizeof(RAWINPUTDEVICELIST)
    );

    // if user supplied an array
    if (inputDevices) {
        RAWINPUTDEVICELIST* devices = palAllocate(
            input->allocator, 
            sizeof(RAWINPUTDEVICELIST) * deviceCount
        );

        if (!devices) {
            return PAL_ERROR_OUT_OF_MEMORY;
        }

        GetRawInputDeviceList(
            devices, 
            &deviceCount, 
            sizeof(RAWINPUTDEVICELIST)
        );

        for (UINT i = 0; i < deviceCount; ++i) {
            inputDevices[i] = (PalInputDevice)devices[i].hDevice;
        }
        palFree(input->allocator, devices);
    }

    // xinput
    if (data->XinputSupport) {
        XINPUT_STATE state;
        for (DWORD i = 0; i < XINPUT_COUNT; i++) {
            if (data->getXinputState(i, &state) == ERROR_SUCCESS) {
                if (inputDevices) {
                    if (deviceCount < maxDeviceCount) {
                        PalInputDevice handle = palMakeXinputHandle(i);
                        inputDevices[deviceCount] = handle;
                    }
                    deviceCount++;
                }
            }
        }
    }

    *count = deviceCount;
    return PAL_SUCCESS;
}

PalResult _PCALL palGetInputDeviceInfo(
    PalInputDevice inputDevice, 
    PalInputDeviceInfo* info) {

    if (!inputDevice || !info) {
        return PAL_ERROR_NULL_POINTER;
    }

    if (palIsXinputHandle(inputDevice)) {
        if (!palGetXinputDeviceInfo(inputDevice, info)) {
            return PAL_ERROR_INVALID_INPUT_DEVICE;
        }

    } else {
        if (!palGetRawDeviceInfo(inputDevice, info)) {
            return PAL_ERROR_INVALID_INPUT_DEVICE;
        }
    }
    return PAL_SUCCESS;
}

PalResult _PCALL palRegisterInputDevice(
    PalInput input,
    PalInputDevice inputDevice) {

    if (!input || !inputDevice) {
        return PAL_ERROR_NULL_POINTER;
    }

    InputDataWin32* data = input->platformData;
    if (!data) {
        return PAL_ERROR_NULL_POINTER;
    }

    if (palIsXinputHandle(inputDevice)) {
        // TODO: Xinput

    } else {
        RID_DEVICE_INFO deviceInfo = {};
        deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
        Uint32 size = sizeof(RID_DEVICE_INFO);
        if (GetRawInputDeviceInfoA(
            (HANDLE)inputDevice,
            RIDI_DEVICEINFO,
            &deviceInfo,
            &size) == (UINT)-1) {
            return PAL_ERROR_INVALID_INPUT_DEVICE;
        }

        switch (deviceInfo.dwType) {
            case RIM_TYPEKEYBOARD: {
                RAWINPUTDEVICE rid = {};
                rid.dwFlags = RIDEV_INPUTSINK;
                rid.hwndTarget = data->window;
                rid.usUsage = 0x02; 
                rid.usUsagePage = 0x01;

                if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
                    return PAL_ERROR_DEVICE_NOT_FOUND;
                }
                return PAL_SUCCESS;
            }

            case RIM_TYPEMOUSE: {
                RAWINPUTDEVICE rid = {};
                rid.dwFlags = RIDEV_INPUTSINK;
                rid.hwndTarget = data->window;
                rid.usUsage = 0x06; 
                rid.usUsagePage = 0x01;

                if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
                    return PAL_ERROR_DEVICE_NOT_FOUND;
                }
                return PAL_SUCCESS;
            }

            case RIM_TYPEHID: {
                // TODO: HID

                return PAL_ERROR_DEVICE_NOT_FOUND;
            }

        }
    }
}

// ==================================================
// Internal API
// ==================================================

PalResult palCreateInputData(PalInput input) {

    InputDataWin32* data = palAllocate(input->allocator, sizeof(InputDataWin32));
    if (!data) {
        return PAL_ERROR_OUT_OF_MEMORY;
    }

    palZeroMemory(data, sizeof(InputDataWin32));
    data->instance = GetModuleHandleW(PAL_NULL);
    palRegisterInputClass(data->instance);
    data->window = CreateWindowExW(
        0,
        WIN32_INPUT_CLASS,
        L"",
        0,
        0,
        0,
        1,
        1,
        PAL_NULL,
        PAL_NULL,
        data->instance,
        PAL_NULL
    );

    if (!data->window) {
        palFree(input->allocator, data);
        return PAL_ERROR_DEVICE_NOT_FOUND;
    }

    // xinput
    data->xInput = LoadLibraryA(XINPUT_1_4);
    if (!data->xInput) {
        // load a lower version 
        data->xInput = LoadLibraryA(XINPUT_1_3);
        if (!data->xInput) {
            data->xInput = LoadLibraryA(XINPUT_9_1_0);
            if (!data->xInput) {
                data->XinputSupport = PAL_FALSE;

            }
        }
    }
    data->XinputSupport = PAL_TRUE;

    // load function pointers
    if (data->XinputSupport) {
        data->getXinputState = (XInputGetStateFn)GetProcAddress(
            data->xInput,
            "XInputGetState"
        );

        data->setXinputState = (XInputSetStateFn)GetProcAddress(
            data->xInput,
            "XInputSetState"
        );
    }

    // hid
    data->hid = LoadLibraryA("hid.dll");

    ShowWindow(data->window, SW_HIDE);
    input->platformData = data;
    SetPropW(data->window, WIN32_INPUT_PROP, input);
    return PAL_SUCCESS;
}

void palDestroyInputData(PalInput input) {

    InputDataWin32* inputData = input->platformData;
    DestroyWindow(inputData->window);
    palUnregisterInputClass(inputData->instance);

    if (inputData->xInput) {
        FreeLibrary(inputData->xInput);
    }

     if (inputData->hid) {
        FreeLibrary(inputData->hid);
    }
    palFree(input->allocator, inputData);
}

bool palRegisterInputClass(HINSTANCE instance) {

    WNDCLASSEXW wcCheck = {};
    wcCheck.cbSize = sizeof(WNDCLASSEXW);
    if (GetClassInfoExW(instance, WIN32_INPUT_CLASS, &wcCheck)) {
        return PAL_TRUE;
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = palInputProc;
    wc.hInstance = instance;
    wc.lpszClassName = WIN32_INPUT_CLASS;

    if (!RegisterClassW(&wc)) {
        return PAL_FALSE;
    }
    return PAL_TRUE;
}

void palUnregisterInputClass(HINSTANCE instance) {

    WNDCLASSEXW wcCheck = {};
    wcCheck.cbSize = sizeof(WNDCLASSEXW);
    if (GetClassInfoExW(instance, WIN32_INPUT_CLASS, &wcCheck)) {
        UnregisterClassW(WIN32_INPUT_CLASS, instance);
    }
}

LRESULT CALLBACK palInputProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    PalInput input = GetPropW(hwnd, WIN32_INPUT_PROP);
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
            UINT ret = GetRawInputData(
                (HRAWINPUT)lParam, 
                RID_INPUT, 
                buffer, 
                &size, 
                sizeof(RAWINPUTHEADER)
            );

            if (ret != size) {
                break;
            }

            RAWINPUT* raw = (RAWINPUT*)buffer;
            // keyboard
            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                PalScancode scancode = PAL_SCANCODE_UNKNOWN;
                PalKey key = PAL_KEY_UNKNOWN;
                RAWKEYBOARD* keyboard = &raw->data.keyboard;
                bool extended = (keyboard->Flags & RI_KEY_E0) != 0;

                if (!extended && keyboard->MakeCode == 0x045) {
                    scancode = PAL_SCANCODE_NUMLOCK;

                } else {
                    Uint16 index = keyboard->MakeCode | (extended << 8);
                    scancode = input->scancodes[index];
                }

                if (extended && keyboard->VKey == VK_RETURN) {
                    key = PAL_KEY_KP_ENTER;

                } else if (extended && keyboard->VKey == VK_OEM_PLUS) {
                    key = PAL_KEY_KP_EQUAL;

                } else {
                    key = input->keycodes[keyboard->VKey];
                }

                if (keyboard->Flags & RI_KEY_BREAK) {
                    input->keyState[key] = PAL_FALSE;
                    input->scancodeState[scancode] = PAL_FALSE;
                } else {
                    input->keyState[key] = PAL_TRUE;
                    input->scancodeState[scancode] = PAL_TRUE;
                }
            }

            // mouse
            if (raw->header.dwType == RIM_TYPEMOUSE) {
                RAWMOUSE* mouse = &raw->data.mouse;
                USHORT flags = mouse->usButtonFlags;
                input->mouseDX += mouse->lLastX;
                input->mouseDX += mouse->lLastY;

                // left
                if (flags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_LEFT] = PAL_TRUE;

                } else {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_LEFT] = PAL_FALSE;
                }

                // right
                if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_RIGHT] = PAL_TRUE;

                } else {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_RIGHT] = PAL_FALSE;
                }

                // middle
                if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_MIDDLE] = PAL_TRUE;

                } else {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_MIDDLE] = PAL_FALSE;
                }

                // x1
                if (flags & RI_MOUSE_BUTTON_4_DOWN) {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_X1] = PAL_TRUE;

                } else {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_X1] = PAL_FALSE;
                }

                // x2
                if (flags & RI_MOUSE_BUTTON_5_DOWN) {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_X2] = PAL_TRUE;

                } else {
                    input->mouseButtonState[PAL_MOUSE_BUTTON_X2] = PAL_FALSE;
                }

                // wheel delta
                if (flags & RI_MOUSE_WHEEL) {
                    SHORT delta = (SHORT)HIWORD(mouse->usButtonData);
                    input->mouseWheelDeltaY += (delta / WHEEL_DELTA);
                }

                if (flags & RI_MOUSE_HWHEEL) {
                    SHORT delta = (SHORT)HIWORD(mouse->usButtonData);
                    input->mouseWheelDeltaX += (delta / WHEEL_DELTA);
                }
            }
            
            break;
        }
    }
    
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}