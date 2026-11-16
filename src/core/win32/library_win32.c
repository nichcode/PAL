/**
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

#include "core/platform.h"
#include "pal2/core/library.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

/** We use a union to avoid GCC and Clang warnings. This is needed since
 * we treat warnings as errors.
 */
typedef union Symbol
{
    FARPROC raw;
    PalLibrarySymbol symbol;
} Symbol;

PalLibrary* platformLoadLibrary(const char* path)
{
    if (!path) {
        return (PalLibrary*)LoadLibraryW(nullptr);
    }

    int len = MultiByteToWideChar(CP_UTF8, 0, path, -1, nullptr, 0);
    if (len <= 0) {
        return nullptr;
    }

    wchar_t widePath[MAX_PATH];
    if (len > MAX_PATH) {
        return nullptr;
    }

    MultiByteToWideChar(CP_UTF8, 0, path, -1, widePath, len);
    return (PalLibrary*)LoadLibraryW(widePath);
}

PalLibrarySymbol platformGetSymbol(
    PalLibrary* library, 
    const char* name)
{
    Symbol sym;
    sym.raw = GetProcAddress((HMODULE)library, name);
    return sym.symbol;
}

void* platformGetGlobal(PalLibrary* library, const char* name)
{
    return (void*)GetProcAddress((HMODULE)library, name);
}

void platformFreeLibrary(PalLibrary* library)
{
    FreeLibrary((HMODULE)library);
}

#endif // _WIN32