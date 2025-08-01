
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

// ==================================================
// Public API
// ==================================================

void* _PCALL palAllocate(
    PalAllocator* allocator, 
    Uint64 size) {

    if (allocator && allocator->allocate) {
        return allocator->allocate(allocator->userData, size);
    }
    return malloc(size);
}

void _PCALL palFree(
    PalAllocator* allocator, 
    void* ptr) {

    if (allocator && allocator->free && ptr) {
        allocator->free(allocator->userData, ptr);
        return;
    }
    free(ptr);
}

void _PCALL palSetMemory(
    void* ptr, 
    int value, 
    Uint64 size) {

    if (ptr) {
        memset(ptr, value, size);
    }
}

void _PCALL palZeroMemory(
    void* ptr, 
    Uint64 size) {

    if (ptr) {
        memset(ptr, 0, size);
    }
}

void _PCALL palCopyMemory(
    void* dest, 
    const void* src, 
    Uint64 size) {

    if (dest && src) {
        memcpy(dest, src, size);
    }
}

void _PCALL palMoveMemory(
    void* dest, 
    const void* src, 
    Uint64 size) {

    if (dest && src) {
        memmove(dest, src, size);
    }
}
