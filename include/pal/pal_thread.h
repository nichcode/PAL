
#ifndef _PAL_THREAD_H
#define _PAL_THREAD_H

#include "pal_core.h"

typedef Uint32 PalTlsId;
typedef void (*PaTlsDestructorFn)(void*);

_PAPI PalTlsId _PCALL palCreateTls(PaTlsDestructorFn destructor);
_PAPI void _PCALL palDestroyTls(PalTlsId id);
_PAPI void* _PCALL palGetTls(PalTlsId id);
_PAPI void _PCALL palSetTls(PalTlsId id, void* data);

#endif // _PAL_THREAD_H