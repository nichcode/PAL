
#include "tests.h"

#define NAME_MAX 32

bool coreTest()
{
    PalVersion version = palGetVersion();
    palLogInfo("PAL Version (%i.%i.%i)", version.major, version.minor, version.patch);
    palLogInfo(palGetPlatformString());

    PalTLSID tlsID;
    tlsID = palCreateTLS();
    char* name = palAllocate(NAME_MAX); // + 1 to use 32 characters
    palZeroMemory(name, NAME_MAX);

    palCopyString(name, "TinyLocalStorage");
    if (!name) {
        return PAL_FALSE;
    }

    palSetTLS(tlsID, name);

    char* tlsName = palGetTLS(tlsID);
    palLogInfo(tlsName);
    palDestroyTLS(tlsID);
    palFree(name);

    return PAL_FALSE;
}