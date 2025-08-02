
/**

Copyright (C) 2025 Nicholas Agbo
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

#include "pal_pch.h"
#include "pal/pal_core.h"

#define PAL_DEFAULT_ALIGNMENT 16

// ==================================================
// Public API
// ==================================================

void* _PCALL palAllocate(
    PalAllocator* allocator, 
    Uint64 size,
    Uint64 alignment) {

    Uint64 align = alignment;
    if (align == 0) {
        align = PAL_DEFAULT_ALIGNMENT;
    }
    
    if (allocator && allocator->allocate) {
        return allocator->allocate(allocator->userData, size, align);
    }

#ifdef _WIN32
    return _aligned_malloc(size, align);
#endif // _WIN32
}

void _PCALL palFree(
    PalAllocator* allocator, 
    void* ptr) {

    if (allocator && allocator->free && ptr) {
        allocator->free(allocator->userData, ptr);
        return;
    }

#ifdef _WIN32
    _aligned_free(ptr);
#endif // _WIN32
}