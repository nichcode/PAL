
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#if PAL_HAS_WAYLAND_BACKEND == 1

#include "pal_wayland.h"
#include "pal_shared.h"

PalResult wlCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

void wlDestroyIcon(PalIcon* icon)
{
    return;
}

PalResult wlSetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // __linux__