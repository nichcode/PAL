
/**
 * @file library.h
 * @brief This is the header file for PAL Library API.
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

#ifndef LIBRARY_H
#define LIBRARY_H

#include "defines.h"

/**
 * @brief Opaque handle to a library.
 * 
 * @since Added in version 2.2
 * @ingroup pal_core
 * 
 * @sa palLoadLibrary
 */
typedef struct PalLibrary PalLibrary;

/**
 * @brief A generic library symbol for exported functions.
 * 
 * @since Added in version 2.2
 * @ingroup pal_core
 * 
 * @sa palGetSymbol
 */
typedef void (PAL_CALL *PalLibrarySymbol)(void);

/**
 * @brief Loads the specified shared library module dynamically into 
 * address space.
 * 
 * The specified module will load other modules is there is a dependency
 * between them. `path` will be searched in the systems default
 * module directories. The library does not validate and resolves
 * its function symbols after creation, the symbol is resolved when
 * `palGetSymbol()` is called.
 * 
 * Calling the function with `path` set to `nullptr` is 
 * implementation-defined. An implementation might return the
 * handle to the main program. Another implementation might fail.
 * 
 * The returned library must be freed with `palFreeLibrary()` when no longer
 * needed.
 * 
 * @param[in] path The path to the library. This can be absolute or relative. 
 * The path must have the extension appended to it.
 *
 * @return The loaded library on success or nullptr on failure.
 *
 * @Thread-safety The entry function must be thread-safe.
 *
 * @since Added in version 2.2
 * @ingroup pal_core
 * @sa palGetSymbol
 * @sa palFreeLibrary
 */
PAL_API PalLibrary* PAL_CALL palLoadLibrary(const char* path);

/**
 * @brief Retrieves the address or symbol of an exported function or variable
 * from the specified library.
 *
 * @param[in] library The library. Must not be `nullptr`.
 * @param[in] name The name of the exported function or variable.
 * Must not be `nullptr`.
 * 
 * @return the symbol or address of the exported funtion or variable on success
 * or `nullptr` on failure. Exported functions are returned as 
 * `PalLibrarySymbol`. Therefore they must be casted to the required type.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.2
 * @ingroup pal_core
 * @sa palLoadLibrary
 * @sa palFreeLibrary
 */
PAL_API PalLibrarySymbol PAL_CALL palGetSymbol(
    PalLibrary* library, 
    const char* name);

/**
 * @brief Unloads the specified library from address space.
 * 
 * This function invalidates all the symbols loaded from it after this call.
 * 
 * @param[in] library The library to free. Must not be `nullptr`.
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

#endif // LIBRARY_H