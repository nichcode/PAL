
#ifndef _PAL_PLATFORM_H
 #define _PAL_PLATFORM_H

#include "pal/pal_core.h"

PAlAllocator palPlatformGetAllocator();
void palWriteConsole(PalLogLevel level, const char* msg);

#endif // _PAL_PLATFORM_H