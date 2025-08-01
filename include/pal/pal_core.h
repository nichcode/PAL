
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

/**
 * @file pal_core.h
 * 
 * Header file for core functions, macros, enum and structs
 * 
 * @defgroup core
 * 
 */

#ifndef _PAL_CORE_H
#define _PAL_CORE_H

#include "pal_defines.h"

/**
 * @brief A type used to identify version
 * 
 * @ingroup defines
 */
typedef struct {
   int major;         /** < Major version*/
   int minor;         /** < Minor version */
   int revision;      /** < Revision version */
} PalVersion;

/**
 * @brief Get the compiled runtime version of the library
 * 
 * @note This function is thread-safe
 * 
 * @return A copy of the version 
 */
_PAPI PalVersion _PCALL palGetVersion();

/**
 * @brief Get the compiled runtime version string of the library.
 * 
 * The format of the string will be: `1.2.3`.
 * 
 * Where `1` is the major, `2` is the minor and `3` is the revision versions respectively
 * 
 * @note This function is thread-safe
 * 
 * @return A copy of the version string
 */
_PAPI const char* _PCALL palGetVersionString();

#endif // _PAL_CORE_H