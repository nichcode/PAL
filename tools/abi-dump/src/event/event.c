
#include "event.h"

bool eventStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Event ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = eventStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = eventQueueStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return eventDriverCreateInfoStruct();
}