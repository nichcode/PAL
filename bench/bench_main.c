
#include "bench.h"
#include "pal/pal_config.h" // for systems reflection

int main(int argc, char**) {

    PalVersion version = palGetVersion();
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

#if PAL_HAS_VIDEO
    windowBench();
    windowUpdateBench();
#endif // PAL_HAS_VIDEO

    return 0;
}