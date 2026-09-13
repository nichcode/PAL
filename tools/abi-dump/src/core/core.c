
#include "helpers.h"

bool allocatorStruct(void);
bool versionStruct(void);
bool loggerStruct(void);

bool coreStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Core ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = allocatorStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = versionStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return loggerStruct();
}