
#include "pal_pch.h"
#include "pal/pal_core.h"

const char* _PCALL palResultToString(PalResult result) {

    switch (result) {
        case PAL_SUCCESS:
        return "Success";

        case PAL_ERROR_NULL_POINTER:
        return "Null pointer";

        case PAL_ERROR_INVALID_ARGUMENT:
        return "Invalid argument";

        case PAL_ERROR_INVALID_ALLOCATOR:
        return "invalid allocator";

        case PAL_ERROR_OUT_OF_MEMORY:
        return "Out of memory";

        case PAL_ERROR_FEATURE_UNSUPPORTED:
        return "Feature not supported";

        case PAL_ERROR_RESOURCE_IN_USE:
        return "Resource in use";

        case PAL_ERROR_TIMEOUT:
        return "Operation timeout";

        case PAL_ERROR_MODULE_NOT_FOUND:
        return "Module or dll not found";

        case PAL_ERROR_DEVICE_NOT_FOUND:
        return "Device not found";

        case PAL_ERROR_INVALID_DISPLAY:
        return "Invalid display";
    }

}