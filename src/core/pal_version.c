
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal2/pal_core.h"

#define PAL_VERSION_MAJOR 2
#define PAL_VERSION_MINOR 1
#define PAL_VERSION_BUILD 0
#define PAL_VERSION_STRING "2.1.0"

void PAL_CALL palGetVersion(PalVersion* version)
{
    version->major = PAL_VERSION_MAJOR;
    version->minor = PAL_VERSION_MINOR;
    version->build = PAL_VERSION_BUILD;
}

const char* PAL_CALL palGetVersionString()
{
    return PAL_VERSION_STRING;
}