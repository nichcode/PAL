
#ifndef ABI_DUMP_H
#define ABI_DUMP_H

#include "helpers.h"

/**
 * We add a high level function that calls its system dump functions. This is
 * here to avoid bloating the main.c with dumps (eg. allocator dump, etc)
*/
bool coreStructs(void);
bool eventStructs(void);
bool threadStructs(void);
bool systemStructs(void);
bool videoStructs(void);
bool openglStructs(void);
bool graphicsStructs(void);

#endif // ABI_DUMP_H