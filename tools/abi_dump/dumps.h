
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _DUMPS_H
#define _DUMPS_H

#include "pal/pal_core.h"
#include <stddef.h>

static const char* s_FailedString = "FAILED";
static const char* s_PassedString = "PASSED";

#ifdef _MSC_VER
#define PAL_ALIGNOF(type) __alignof(type)
#else
#define PAL_ALIGNOF(type) __alignof__(type)
#endif // _MSC_VER

void coreABIDump(PalBool verbose);
void eventABIDump(PalBool verbose);
void threadABIDump(PalBool verbose);
void systemABIDump(PalBool verbose);
void videoABIDump(PalBool verbose);
void openglABIDump(PalBool verbose);

#endif // _DUMPS_H