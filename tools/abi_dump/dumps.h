
#ifndef _DUMPS_H
#define _DUMPS_H

#include "pal/pal_core.h"

#ifdef _MSC_VER
#define PAL_ALIGNOF(type) __alignof(type)
#else
#define PAL_ALIGNOF(type) __alignof__(type)
#endif // _MSC_VER

void dumpCoreABI();

#endif // _DUMPS_H