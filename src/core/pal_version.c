
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