
#include "pal_win32platform.h"

PAlAllocator palPlatformGetAllocator()
{
    PAlAllocator allocator;
    allocator.alloc = malloc;
    allocator.free = free;
    allocator.alignAlloc = _aligned_malloc;
    allocator.alignFree = _aligned_free;

    return allocator;
}