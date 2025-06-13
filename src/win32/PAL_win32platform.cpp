
#include "PAL_pch.h"
#include "PAL_win32platform.h"

PAL_Allocator PAL_PlatformGetAllocator()
{
    PAL_Allocator allocator;
    allocator.alloc = ::operator new;
    allocator.free = ::operator delete;

    return allocator;
}