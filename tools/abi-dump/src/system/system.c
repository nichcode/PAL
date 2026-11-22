
#include "system.h"

PalBool systemStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "System ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    PalBool status = platformInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return cpuInfoStruct();
}