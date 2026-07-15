
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

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
    PalBool dumpAll = PAL_FALSE;
    PalBool dumpCore = PAL_FALSE;
    PalBool dumpEvent = PAL_FALSE;
    PalBool dumpThread = PAL_FALSE;
    PalBool dumpOpengl = PAL_FALSE;
    PalBool dumpGraphics = PAL_FALSE;
    PalBool dumpSystem = PAL_FALSE;
    PalBool dumpVideo = PAL_FALSE;
    PalBool dumpVersion = PAL_FALSE;
    PalBool dumpHelp = PAL_FALSE;
    PalBool verbose = PAL_FALSE;
    PalBool status = PAL_FALSE;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--all") == 0) {
            dumpAll = PAL_TRUE;

        } else if (strcmp(argv[i], "--core") == 0) {
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

        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = PAL_TRUE;
        }
    }

    if (dumpAll) {
        dumpCore = PAL_TRUE;
        dumpEvent = PAL_TRUE;
        dumpThread = PAL_TRUE;
        dumpOpengl = PAL_TRUE;
        dumpGraphics = PAL_TRUE;
        dumpSystem = PAL_TRUE;
        dumpVideo = PAL_TRUE;
    }

    if (dumpCore) {
        status = coreABIDump(verbose);
        if (status == PAL_FALSE) {
            return -1;
        }
    }

    if (dumpEvent) {
        // status = eventABIDump(verbose);
        // if (status == PAL_FALSE) {
        //     return -1;
        // }
    }

    if (dumpThread) {
        // status = threadABIDump(verbose);
        // if (status == PAL_FALSE) {
        //     return -1;
        // }
    }

    if (dumpSystem) {
        // status = systemABIDump(verbose);
        // if (status == PAL_FALSE) {
        //     return -1;
        // }
    }

    if (dumpVideo) {
        // status = videoABIDump(verbose);
        // if (status == PAL_FALSE) {
        //     return -1;
        // }
    }

    if (dumpOpengl) {
        // status = openglABIDump(verbose);
        // if (status == PAL_FALSE) {
        //     return -1;
        // }
    }

    if (dumpGraphics) {
        // status = graphicsABIDump(verbose);
        // if (status == PAL_FALSE) {
        //     return -1;
        // }
    }

    if (dumpVersion) {
        palLog(nullptr, "PAL ABI dump %s", VERSION);
    }

    if (dumpHelp) {
        palLog(nullptr, "USAGE: %s [options]", EXE_NAME);
        palLog(nullptr, "Options:");
        palLog(nullptr, "  --help          Display available options");
        palLog(nullptr, "  --version       Display version");
        palLog(nullptr, "  --verbose       Display ABI dump information");
        palLog(nullptr, "  --all           Check ABI for all PAL structs");
        palLog(nullptr, "  --core          Check ABI for core PAL structs");
        palLog(nullptr, "  --event         Check ABI for event PAL structs");
        palLog(nullptr, "  --graphics      Check ABI for graphics PAL structs");
        palLog(nullptr, "  --opengl        Check ABI for opengl PAL structs");
        palLog(nullptr, "  --system        Check ABI for system PAL structs");
        palLog(nullptr, "  --thread        Check ABI for thread PAL structs");
        palLog(nullptr, "  --video         Check ABI for video PAL structs");
    }

    return 0;
}
