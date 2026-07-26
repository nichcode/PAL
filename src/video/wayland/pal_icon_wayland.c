
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_WAYLAND_BACKEND == 1
#include "pal_wayland.h"

PalResult wlCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

void wlDestroyIcon(PalIcon* icon)
{
    return;
}

#endif // PAL_HAS_WAYLAND_BACKEND