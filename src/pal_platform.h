
#ifndef PAL_PLATFORM_H
    #define PAL_PLATFORM_H

#include "pal/pal.h"

void* palPlatformAlloc(Uint64 size);
void palPlatformMemZero(void* memory, Uint64 size);
void palPlatformMemSet(void* memory, int value, Uint64 size);

void palPlatformMemCpy(void* destMemory, void* srcMemory, Uint64 size);
void palPlatformFree(void* memory);

void palWriteConsole(PAL_LogLevel level, const char* msg);

#endif // PAL_PLATFORM_H