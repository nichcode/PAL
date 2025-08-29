
// ==================================================
// Includes
// ==================================================

#include "pal/pal_core.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <windows.h>
#endif // _WIN32

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#endif // _MSC_VER

// ==================================================
// Typedefs, enums and structs
// ==================================================

#define DEFAULT_ALIGNMENT 16

// ==================================================
// Internal API
// ==================================================

static inline void* alignedAlloc(
    Uint64 size,
    Uint64 alignment) { 

#if defined(_MSC_VER) || defined(__MINGW32__)
    return _aligned_malloc(size, alignment);
#elif defined(_ISOC11_SOURCE) || defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    return aligned_alloc(alignment, size);
#else
    void* ptr = nullptr;
    posix_memalign(&ptr, alignment);
    return ptr;
#endif // _MSC_VER
}

static inline void alignedFree(void* ptr) { 

#if defined(_MSC_VER) || defined(__MINGW32__)
    _aligned_free(ptr);
#else
    free(ptr);
#endif // _MSC_VER
}

// ==================================================
// Public API
// ==================================================

void* _PCALL palAllocate(
    const PalAllocator* allocator,
    Uint64 size,
    Uint64 alignment) {
    
    Uint64 align = alignment;
    if (align == 0) {
        align = DEFAULT_ALIGNMENT;
    }

    if (allocator && allocator->allocate) {
        return allocator->allocate(allocator->userData, size, align);
    }
    return alignedAlloc(size, alignment);
}

void _PCALL palFree(
    const PalAllocator* allocator,
    void* ptr) {

    if (allocator && allocator->free && ptr) {
        allocator->free(allocator->userData, ptr);

    } else {
        alignedFree(ptr);
    }
}