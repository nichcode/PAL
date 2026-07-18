
#include "pal/pal_graphics.h"
#include "tests.h"

static void PAL_CALL onGraphicsDebug(
    void* userData,
    PalDebugMessageSeverity severity,
    PalDebugMessageType type,
    const char* msg)
{
    palLog(nullptr, msg);
}

PalBool customBackendTest()
{
    return PAL_TRUE;
}