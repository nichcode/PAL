
#include "pal_pch.h"
#include "core/pal_console.h"
#include "platform/pal_platform.h"

#ifdef _WIN32

void palWriteConsole(PalLogLevel level, wchar_t* buffer, const char* msg) {

    HANDLE console = PAL_NULL;
    bool error = level > PAL_LOG_LEVEL_WARN;
    static int levels[4] = { 8, 2, 6, 4 };
    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    int len = palStringToWideString(buffer, msg);
    SetConsoleTextAttribute(console, levels[level]);
    DWORD written = 0;
    WriteConsoleW(console, buffer, (DWORD)len - 1, &written, 0);
    OutputDebugStringW(buffer);
}

#endif // _WIN32