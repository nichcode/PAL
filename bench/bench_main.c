
#include "bench.h"
#include "pal/pal_config.h" // for systems reflection

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

#if PAL_HAS_VIDEO
    windowBench();
    windowBenchPlatform();
#endif // PAL_HAS_VIDEO

    return 0;
}