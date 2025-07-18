
#ifndef _PAL_PLATFORM_H
#define _PAL_PLATFORM_H

#include "pal/pal_core.h"

bool palGetOsVersionWin32(PalVersion* version);
bool palIsOsVersionWin32(PalVersion* osVersion, PalVersion* version);
int palStringToWideString(wchar_t* buffer, const char* string);

typedef struct palVersionsWin32 {
    PalVersion windowXP;
    PalVersion windowVista;
    PalVersion window7;
    PalVersion window8;
    PalVersion window8_1;
    PalVersion window10;
    PalVersion window11;
} palVersionsWin32;

extern palVersionsWin32 g_VersionsWin32;

#endif // _PAL_PLATFORM_H