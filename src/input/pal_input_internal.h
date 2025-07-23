
#ifndef _PAL_INPUT_INTERNAL_H
#define _PAL_INPUT_INTERNAL_H

#include "pal/pal_input.h"

struct PalInput_T {
    PalAllocator* allocator;
    PalEventDriver eventDriver;
    void* platformData;
};

PalResult palCreateInputData(PalAllocator* allocator, void** outData);
void palDestroyInputData(PalAllocator* allocator, void* data);

#endif // _PAL_INPUT_INTERNAL_H