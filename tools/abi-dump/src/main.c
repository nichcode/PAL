
#include "helpers.h"

#define VERSION "1.0"
#define LOG_NAME "PAL ABI Dump"

#ifdef _WIN32
#define EXE_NAME "abi-dump.exe"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DLL_HANDLE HMODULE
#define LOAD_DLL LoadLibraryA("PAL2.dll")
#define FREE_DLL(handle) FreeLibrary(handle)
#define GET_FUNC(handle, name) GetProcAddress(handle, name)

#else
#define EXE_NAME "abi-dump"
#include <dlfcn.h>

#define DLL_HANDLE void*
#define LOAD_DLL dlopen("libPAL2.so", RTLD_LAZY)
#define FREE_DLL(handle) dlclose(handle)
#define GET_FUNC(handle, name) dlsym(handle, name)
#endif // _WIN32

typedef void(PAL_CALL* GetVersionFn)(PalVersion* version);

static int logDumpStatus(bool status)
{
    int ret = -1;
    const char* str = "FAILED";
    if (status) {
        str = "PASSED";
        ret = 0;
    }

    palLog(nullptr, "%s %s: %s", LOG_NAME, "Status", str);
    return ret;
}

static bool checkPALVersion()
{
    GetVersionFn getVersion = nullptr;
    DLL_HANDLE handle = LOAD_DLL;
    if (!handle) {
        palLog(nullptr, "Failed to load PAL library");
        return PAL_FALSE;
    }

    getVersion = (GetVersionFn)GET_FUNC(handle, "palGetVersion");
    if (!getVersion) {
        palLog(nullptr, "Failed to load palGetVersion in PAL library");
        FREE_DLL(handle);
        return PAL_FALSE;
    }

    PalVersion version;
    getVersion(&version);
    FREE_DLL(handle);

    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "PAL ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    if (strcmp(VERSION, "1.0") == 0) {
        // ABI Dump v1.0 == PAL v2
        if (version.major == 2) {
            if (g_DumpFlags & ABI_DUMP_VERBOSE) {
                palLog(nullptr, "Expected PAL ABI Generation: 2");
                palLog(nullptr, "Actual PAL ABI Generation:: %d", version.major);
                palLog(nullptr, "Target Library: PAL2");
            }
            return PAL_TRUE;
        }
    }

    return PAL_FALSE;
}

int main(int argc, char** argv)
{
    PalBool status = PAL_FALSE;
    PalBool dumpVersion = PAL_FALSE;
    PalBool dumpHelp = PAL_FALSE;
    uint32_t passed = 0;
    uint32_t dumps = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--core") == 0) {
            dumps |= ABI_DUMP_CORE;

        } else if (strcmp(argv[i], "--event") == 0) {
            dumps |= ABI_DUMP_EVENT;

        } else if (strcmp(argv[i], "--graphics") == 0) {
            dumps |= ABI_DUMP_GRAPHICS;

        } else if (strcmp(argv[i], "--opengl") == 0) {
            dumps |= ABI_DUMP_OPENGL;

        } else if (strcmp(argv[i], "--system") == 0) {
            dumps |= ABI_DUMP_SYSTEM;

        } else if (strcmp(argv[i], "--thread") == 0) {
            dumps |= ABI_DUMP_THREAD;

        } else if (strcmp(argv[i], "--video") == 0) {
            dumps |= ABI_DUMP_VIDEO;

        } else if (strcmp(argv[i], "--version") == 0) {
            dumpVersion = PAL_TRUE;

        } else if (strcmp(argv[i], "--help") == 0) {
            dumpHelp = PAL_TRUE;

        } else if (strcmp(argv[i], "--verbose") == 0) {
            g_DumpFlags &= ~ABI_DUMP_QUICK;
            g_DumpFlags |= ABI_DUMP_VERBOSE;

        } else if (strcmp(argv[i], "--quick") == 0) {
            g_DumpFlags &= ~ABI_DUMP_VERBOSE;
            g_DumpFlags |= ABI_DUMP_QUICK;
        }
    }

    if (dumps == 0 && dumpHelp == PAL_FALSE && dumpVersion == PAL_FALSE) {
        dumps |= ABI_DUMP_ALL;
    }

    status = checkPALVersion();
    if (!status) {
        if (g_DumpFlags & ABI_DUMP_QUICK) {
            return logDumpStatus(status);
        } else {
            return -1;
        }
    }

    if (dumps & ABI_DUMP_CORE) {
        status = coreStructs();
        if (status) {
            passed |= ABI_DUMP_CORE;
        } else {
            if (g_DumpFlags & ABI_DUMP_QUICK) {
                return logDumpStatus(status);
            } else {
                return -1;
            }
        }
    }

    if (dumps & ABI_DUMP_EVENT) {
        status = eventStructs();
        if (status) {
            passed |= ABI_DUMP_EVENT;
        } else {
            if (g_DumpFlags & ABI_DUMP_QUICK) {
                return logDumpStatus(status);
            } else {
                return -1;
            }
        }
    }

    if (dumps & ABI_DUMP_THREAD) {
        status = threadStructs();
        if (status) {
            passed |= ABI_DUMP_THREAD;
        } else {
            if (g_DumpFlags & ABI_DUMP_QUICK) {
                return logDumpStatus(status);
            } else {
                return -1;
            }
        }
    }

    if (dumps & ABI_DUMP_SYSTEM) {
        status = systemStructs();
        if (status) {
            passed |= ABI_DUMP_SYSTEM;
        } else {
            if (g_DumpFlags & ABI_DUMP_QUICK) {
                return logDumpStatus(status);
            } else {
                return -1;
            }
        }
    }

    if (dumps & ABI_DUMP_VIDEO) {
        status = videoStructs();
        if (status) {
            passed |= ABI_DUMP_VIDEO;
        } else {
            if (g_DumpFlags & ABI_DUMP_QUICK) {
                return logDumpStatus(status);
            } else {
                return -1;
            }
        }
    }

    if (dumps & ABI_DUMP_OPENGL) {
        if (status) {
            status = openglStructs();
            passed |= ABI_DUMP_OPENGL;
        } else {
            if (g_DumpFlags & ABI_DUMP_QUICK) {
                return logDumpStatus(status);
            } else {
                return -1;
            }
        }
    }

    if (dumps & ABI_DUMP_GRAPHICS) {
        status = graphicsStructs();
        if (status) {
            passed |= ABI_DUMP_GRAPHICS;
        } else {
            if (g_DumpFlags & ABI_DUMP_QUICK) {
                return logDumpStatus(status);
            } else {
                return -1;
            }
        }
    }

    if (dumpVersion) {
        palLog(nullptr, "%s %s", LOG_NAME, VERSION);
    }

    if (dumpHelp) {
        palLog(nullptr, "USAGE: %s [options]", EXE_NAME);
        palLog(nullptr, "Options:");
        palLog(nullptr, "  --help          Display available options");
        palLog(nullptr, "  --version       Display version");
        palLog(nullptr, "  --verbose       Display Detailed ABI dump information");
        palLog(nullptr, "  --quick         Display only the final ABI status");
        palLog(nullptr, "  --core          Check ABI for core PAL structs");
        palLog(nullptr, "  --event         Check ABI for event PAL structs");
        palLog(nullptr, "  --graphics      Check ABI for graphics PAL structs");
        palLog(nullptr, "  --opengl        Check ABI for opengl PAL structs");
        palLog(nullptr, "  --system        Check ABI for system PAL structs");
        palLog(nullptr, "  --thread        Check ABI for thread PAL structs");
        palLog(nullptr, "  --video         Check ABI for video PAL structs");
    }

    if (g_DumpFlags & ABI_DUMP_QUICK) {
        if (dumps == 0) {
            return 0;
        }

        if (dumps == passed) {
            return logDumpStatus(PAL_TRUE);
        } else {
            return logDumpStatus(PAL_FALSE);
        }
    }

    return 0;
}