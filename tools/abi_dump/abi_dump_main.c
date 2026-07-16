
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"

#define VERSION "1.0"

#ifdef _WIN32
#define EXE_NAME "abi-dump.exe"
#else
#define EXE_NAME "abi-dump"
#endif // _WIN32

#define DUMP_FLAG_CORE (1u << 0)
#define DUMP_FLAG_EVENT (1u << 1)
#define DUMP_FLAG_THREAD (1u << 2)
#define DUMP_FLAG_OPENGL (1u << 3)
#define DUMP_FLAG_GRAPHICS (1u << 4)
#define DUMP_FLAG_SYSTEM (1u << 5)
#define DUMP_FLAG_VIDEO (1u << 6)
#define DUMP_FLAG_VERSION (1u << 7)
#define DUMP_FLAG_HELP (1u << 8)
#define DUMP_FLAG_ALL 0x7F

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    PalBool verbose = PAL_FALSE;
    PalBool status = PAL_FALSE;
    uint32_t dumpFlags = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--core") == 0) {
            dumpFlags |= DUMP_FLAG_CORE;

        } else if (strcmp(argv[i], "--event") == 0) {
            dumpFlags |= DUMP_FLAG_EVENT;

        } else if (strcmp(argv[i], "--graphics") == 0) {
            dumpFlags |= DUMP_FLAG_GRAPHICS;

        } else if (strcmp(argv[i], "--opengl") == 0) {
            dumpFlags |= DUMP_FLAG_OPENGL;

        } else if (strcmp(argv[i], "--system") == 0) {
            dumpFlags |= DUMP_FLAG_SYSTEM;

        } else if (strcmp(argv[i], "--thread") == 0) {
            dumpFlags |= DUMP_FLAG_THREAD;

        } else if (strcmp(argv[i], "--video") == 0) {
            dumpFlags |= DUMP_FLAG_VIDEO;

        } else if (strcmp(argv[i], "--version") == 0) {
            dumpFlags |= DUMP_FLAG_VERSION;

        } else if (strcmp(argv[i], "--help") == 0) {
            dumpFlags |= DUMP_FLAG_HELP;

        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = PAL_TRUE;
        }
    }

    if (dumpFlags == 0) {
        dumpFlags |= DUMP_FLAG_ALL;
    }

    if (dumpFlags & DUMP_FLAG_CORE) {
        status = coreABIDump(verbose);
        if (status == PAL_FALSE) {
            return -1;
        }
    }

    if (dumpFlags & DUMP_FLAG_EVENT) {
        status = eventABIDump(verbose);
        if (status == PAL_FALSE) {
            return -1;
        }
    }

    if (dumpFlags & DUMP_FLAG_THREAD) {
        status = threadABIDump(verbose);
        if (status == PAL_FALSE) {
            return -1;
        }
    }

    if (dumpFlags & DUMP_FLAG_SYSTEM) {
        status = systemABIDump(verbose);
        if (status == PAL_FALSE) {
            return -1;
        }
    }

    if (dumpFlags & DUMP_FLAG_VIDEO) {
        status = videoABIDump(verbose);
        if (status == PAL_FALSE) {
            return -1;
        }
    }

    if (dumpFlags & DUMP_FLAG_OPENGL) {
        status = openglABIDump(verbose);
        if (status == PAL_FALSE) {
            return -1;
        }
    }

    if (dumpFlags & DUMP_FLAG_GRAPHICS) {
        // status = graphicsABIDump(verbose);
        // if (status == PAL_FALSE) {
        //     return -1;
        // }
    }

    if (dumpFlags & DUMP_FLAG_VERSION) {
        palLog(nullptr, "PAL ABI dump %s", VERSION);
    }

    if (dumpFlags & DUMP_FLAG_HELP) {
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
