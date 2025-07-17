
#include "pal_pch.h"
#include "core/pal_console.h"

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

    // convert to wide string
    int len = MultiByteToWideChar(CP_UTF8, 0, msg, -1, PAL_NULL, 0);
    MultiByteToWideChar(CP_UTF8, 0, msg, -1, buffer, len);

    SetConsoleTextAttribute(console, levels[level]);
    DWORD written = 0;
    WriteConsoleW(console, buffer, (DWORD)len - 1, &written, 0);
    OutputDebugStringW(buffer);
}

#endif // _WIN32