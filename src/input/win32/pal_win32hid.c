
#include "pal_pch.h"
#include "pal_win32input.h"
#include "platform/pal_platform.h"

HidDataWin32 g_HidData;

PalInputDeviceType getHidDeviceType(int vid, int pid, bool gamepad);

void palGetHidProperties(
    PalInputDeviceInfo* info,
    bool isGamepad) {

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

        if (isGamepad) {
            info->type = PAL_INPUT_DEVICE_GAMEPAD;

        } else {
            info->type = PAL_INPUT_DEVICE_HID;
        }
        return;
    }

    WCHAR manufacturer[64] = {};
    WCHAR product[64] = {};
    bool sucessManufacturer = g_HidData.getManufacturerString(
        file,
        manufacturer,
        sizeof(manufacturer)
    );

    bool sucessproduct = g_HidData.getProductString(
        file,
        product,
        sizeof(product)
    );

    if (sucessManufacturer && sucessproduct) {
        WCHAR combined[128] = {};
        _snwprintf(combined, 128, L"%s %s", manufacturer, product);
        palStringToWideString(combined, info->name);

    } else if (sucessManufacturer) {
        palStringToWideString(manufacturer, info->name);

    } else if (sucessproduct) {
        palStringToWideString(product, info->name);

    } else {
        if (isGamepad) {
            strcpy(info->name, "Unknown Gamepad");

        } else {
            strcpy(info->name, "Unknown HID");
        }
    }

    HIDD_ATTRIBUTES attrs = {};
    attrs.Size = sizeof(HIDD_ATTRIBUTES);
    if (!g_HidData.getAttributes(file, &attrs)) {
        info->productID = 0;
        info->vendorID = 0;

        if (isGamepad) {
            info->type = PAL_INPUT_DEVICE_GAMEPAD;

        } else {
            info->type = PAL_INPUT_DEVICE_HID;
        }

        CloseHandle(file);
        return;
    }

    info->productID = attrs.ProductID;
    info->vendorID = attrs.VendorID;
    info->type = getHidDeviceType(info->vendorID, info->productID, isGamepad);
    CloseHandle(file);
}

PalInputDeviceType getHidDeviceType(int vid, int pid, bool gamepad) {

    if (vid == 0x054C) {
        // sony
        switch (pid) {
            case 0x05C4:
            return PAL_INPUT_DEVICE_DUALSHOCK4;

            case 0x0CE6:
            return PAL_INPUT_DEVICE_DUALSENSE;
        }

    } else if (vid == 0x057E) {
        // nintendo
        switch (pid) {
            case 0x2009:
            return PAL_INPUT_DEVICE_SWITCH_PRO;
        }

    } else {
        if (gamepad) {
            return PAL_INPUT_DEVICE_GAMEPAD;

        } else {
            return PAL_INPUT_DEVICE_HID;
        }
    }
}