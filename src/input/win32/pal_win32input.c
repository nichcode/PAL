
#include "pal_pch.h"
#include "input/pal_input_internal.h"
#include "pal_win32input.h"

#define XINPUT_1_4 "xinput1_4.dll"
#define XINPUT_1_3 "xinput1_3.dll"
#define XINPUT_9_1_0 "xinput9_1_0.dll"

PalResult _PCALL palUpdateInput(
    PalInput input) {

    if (!input) {
        return PAL_ERROR_NULL_POINTER;
    }

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

PalResult palCreateInputData(PalAllocator* allocator, void** outData) {

    InputDataWin32* data = palAllocate(allocator, sizeof(InputDataWin32));
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
        palFree(allocator, data);
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

    ShowWindow(data->window, SW_HIDE);
    *outData = data;
    return PAL_SUCCESS;
}

void palDestroyInputData(PalAllocator* allocator, void* data) {

    InputDataWin32* inputData = data;
    DestroyWindow(inputData->window);
    palUnregisterInputClass(inputData->instance);

    if (inputData->xInput) {
        FreeLibrary(inputData->xInput);
    }
    palFree(allocator, inputData);
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
    
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}