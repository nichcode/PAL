
#ifndef _PAL_INPUT_INTERNAL_H
#define _PAL_INPUT_INTERNAL_H

#include "pal/pal_input.h"

struct PalInput_T {
    PalAllocator* allocator;
    PalEventDriver eventDriver;
    PalScancode scancodes[512];
    const char* scancodeNames[PAL_SCANCODE_MAX];
    void* platformData;
};

PalResult palCreateInputData(PalInput input);
void palDestroyInputData(PalInput input);

void palMapScancodes(PalInput input);
void palMapScancodeNames(PalInput input);

#endif // _PAL_INPUT_INTERNAL_H