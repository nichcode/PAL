
#ifndef _PAL_THREAD_H
 #define _PAL_THREAD_H

#include "pal_defines.h"

typedef int PalTLSID;

_PAPI PalTLSID _PCALL palCreateTLS();
_PAPI bool _PCALL palDestroyTLS(PalTLSID id);
_PAPI void* _PCALL palGetTLS(PalTLSID id);
_PAPI bool _PCALL palSetTLS(PalTLSID id, void* data, void (*destructor)(void*));

#endif // _PAL_THREAD_H