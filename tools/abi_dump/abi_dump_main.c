
#include "dumps.h"

#define VERSION "1.0"

#ifdef _WIN32
#define EXE_NAME "pal-abi-dump.exe"
#else
#define EXE_NAME "pal-abi-dump"
#endif // _WIN32

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    PalBool dumpCore = PAL_FALSE;
    PalBool dumpEvent = PAL_FALSE;
    PalBool dumpThread = PAL_FALSE;
    PalBool dumpOpengl = PAL_FALSE;
    PalBool dumpGraphics = PAL_FALSE;
    PalBool dumpSystem = PAL_FALSE;
    PalBool dumpVideo = PAL_FALSE;
    PalBool dumpVersion = PAL_FALSE;
    PalBool dumpHelp = PAL_FALSE;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--core") == 0) {
            dumpCore = PAL_TRUE;

        } else if (strcmp(argv[i], "--event") == 0) {
            dumpEvent = PAL_TRUE;

        } else if (strcmp(argv[i], "--graphics") == 0) {
            dumpGraphics = PAL_TRUE;

        } else if (strcmp(argv[i], "--opengl") == 0) {
            dumpOpengl = PAL_TRUE;

        } else if (strcmp(argv[i], "--system") == 0) {
            dumpSystem = PAL_TRUE;

        } else if (strcmp(argv[i], "--thread") == 0) {
            dumpThread = PAL_TRUE;

        } else if (strcmp(argv[i], "--video") == 0) {
            dumpVideo = PAL_TRUE;

        } else if (strcmp(argv[i], "--version") == 0) {
            dumpVersion = PAL_TRUE;

        } else if (strcmp(argv[i], "--help") == 0) {
            dumpHelp = PAL_TRUE;
        }
    }

    if (dumpCore) {
        dumpCoreABI();
    }

    if (dumpEvent) {
        dumpEventABI();
    }

    if (dumpVersion) {
        palLog(nullptr, "PAL ABI dump %s", VERSION);
    }

    if (dumpHelp) {
        palLog(nullptr, "USAGE: %s [options]", EXE_NAME);
        palLog(nullptr, "Options:"); // 10 spaces
        palLog(nullptr, "  --help          Display available options");
        palLog(nullptr, "  --version       Display ABI dump version information");
        palLog(nullptr, "  --core          Display PAL core system structs ABI information");
    }

    return 0;
}
