
#include "pal_pch.h"
#include "pal/pal_core.h"

// compiled version
#define PAL_VERSION_MAJOR 1
#define PAL_VERSION_MINOR 0
#define PAL_VERSION_PATCH 0

void _PCALL palGetVerion(
    PalVersion* version)
{
    if (!version) {
        return;
    }

    version->major = PAL_VERSION_MAJOR;
    version->minor = PAL_VERSION_MINOR;
    version->patch = PAL_VERSION_PATCH;
}