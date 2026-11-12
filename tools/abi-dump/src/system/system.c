
#include "system.h"

bool systemStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "System ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = platformInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return cpuInfoStruct();
}