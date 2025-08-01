
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

#define PAL_VMAJOR 1
#define PAL_VMINOR 0
#define PAL_VPATCH 0

#define PAL_VERSION_STRING "1.0.0"

// ==================================================
// Public API
// ==================================================

PalVersion _PCALL palGetVersion() { 

    return (PalVersion){
        .major = PAL_VMAJOR,
        .minor = PAL_VMINOR,
        .patch = PAL_VPATCH
    };
}

const char* _PCALL palGetVersionString() {

    return PAL_VERSION_STRING;
}