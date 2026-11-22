
#include "event.h"

PalBool eventStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Event ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    PalBool status = eventStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = eventQueueStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return eventDriverCreateInfoStruct();
}