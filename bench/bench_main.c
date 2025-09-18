
#include "bench.h"
#include "pal/pal_config.h" // for systems reflection

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    PalVersion version = palGetVersion();
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

#if PAL_HAS_VIDEO
    windowBench();
    windowUpdateBench();
#endif // PAL_HAS_VIDEO

    return 0;
}