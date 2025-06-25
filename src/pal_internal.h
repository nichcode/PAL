
#ifndef _PAL_INTERNAL_H
 #define _PAL_INTERNAL_H

#include "pal/pal.h"

#define _PAL_MSG_SIZE 1024
#define PAL_VMAJOR 1
#define PAL_VMINOR 0
#define PAL_VPATCH 0

typedef struct PalLibrary
{
    // read only 
    PalVersion version;
    PAlAllocator tmpAllocator;
    PAlAllocator* allocator;
    bool initialized, videoInitialized;

} PalLibrary;

static PalLibrary s_PAL;

void _palFormatArgs(const char* fmt, va_list argsList, char* buffer);
void _palFormat(char* buffer, const char* fmt, ...);

// platform
void _palPlatformWriteConsole(Uint32 level, const char* msg);

#endif // _PAL_INTERNAL_H