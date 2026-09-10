
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "shared.h"
#include "pal2/pal_core.h"

#if PLATFORM_POSIX
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>

#if defined(__APPLE__)
#define EXT ".dylib"
#else
#define EXT ".so"
#endif // EXT

/** We use a union to avoid GCC and Clang warnings. This is needed since
 * we treat warnings as errors.
 */
typedef union Symbol
{
    void* raw;
    PalLibrarySymbol symbol;
} Symbol;

PalLibrary* PAL_CALL palLoadLibrary(const char* path)
{
    char buffer[PAL_MAX_PATH];
    if (strlen(path) + strlen(EXT) >= PAL_MAX_PATH) {
        return nullptr;
    }

    snprintf(buffer, PAL_MAX_PATH, "%s%s", path, EXT);
    return (PalLibrary*)dlopen(buffer, RTLD_LAZY);
}

PalLibrarySymbol PAL_CALL palGetSymbol(
    PalLibrary* library, 
    const char* name)
{
    Symbol sym;
    sym.raw = dlsym((void*)library, name);
    return sym.symbol;
}

void PAL_CALL palFreeLibrary(PalLibrary* library)
{
    dlclose((void*)library);
}

#endif // PLATFORM_POSIX