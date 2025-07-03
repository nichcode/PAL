
#include "pal_pch.h"
#include "pal/pal_core.h"

#define PAL_VERSION_MAJOR 1
#define PAL_VERSION_MINOR 0
#define PAL_VERSION_PATCH 0

PalVersion _PCALL palGetVersion()
{
    PalVersion version;
    version.major = PAL_VERSION_MAJOR;
    version.minor = PAL_VERSION_MINOR;
    version.patch = PAL_VERSION_PATCH;
    return version;
}