
#include "core.h"

PalBool coreStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Core ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    PalBool status = allocatorStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = versionStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return loggerStruct();
}