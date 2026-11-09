
/**
 * @file library.h
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

#ifndef PAL_CORE_LIBRARY_H
#define PAL_CORE_LIBRARY_H

#include "types.h"

/**
 * @brief Loads a shared library.
 *
 * This function loads the specified shared library module dynamically into address
 * space using the platform's search rules. `path` must have the library extension
 * appended to it and can be absolute or relative.
 *
 * The specified module will load other modules if there is a dependency between them.
 * The library does not resolve its symbols after being loaded; a symbol is
 * resolved when @ref palGetSymbol() is called.
 *
 * Calling the function with `path` set to `nullptr` is implementation-defined
 * behavior. The returned library must be freed with @ref palFreeLibrary().
 *
 * @param[in] path Path to the library (absolute or relative, including the file extension).
 * @return The loaded library on success or `nullptr` if the library failed to load.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.2
 * @ingroup pal_core
 *
 * @sa palGetSymbol
 * @sa palFreeLibrary
 */
PAL_API PalLibrary* PAL_CALL palLoadLibrary(const char* path);

/**
 * @brief Retrieves a symbol from a library.
 *
 * This function retrieves a symbol from the specified library. `library` must be
 * loaded into address space before this call.
 *
 * Exported symbols are returned as @ref PalLibrarySymbol; a cast is required to use
 * the symbol as its declared type.
 *
 * @param[in] library Library to retrieve the symbol from.
 * @param[in] name Null-terminated UTF-8 encoded name of the symbol.
 * @return The symbol on success or `nullptr` if the symbol was not found.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.2
 * @ingroup pal_core
 *
 * @sa palLoadLibrary
 * @sa palFreeLibrary
 */
PAL_API PalLibrarySymbol PAL_CALL palGetSymbol(
    PalLibrary* library,
    const char* name);

/**
 * @brief Unloads the specified library from address space.
 *
 * This function unloads the specified library from address space and
 * invalidates all symbols retrieved from it.
 *
 * @param[in] library Library to free.
 *
 * @Thread-safety `library` must be externally synchronized.
 *
 * @since Added in version 2.2
 * @ingroup pal_core
 *
 * @sa palLoadLibrary
 * @sa palGetSymbol
 */
PAL_API void PAL_CALL palFreeLibrary(PalLibrary* library);

#endif // PAL_CORE_LIBRARY_H