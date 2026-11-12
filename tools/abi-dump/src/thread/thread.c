
#include "thread.h"

bool threadStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Thread ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    return threadCreateInfoStruct();
}