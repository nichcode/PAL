
#include "pal_pch.h"
#include "pal/pal_core.h"

static PalTLSID s_LogTLSID = 0;
static PalLogCallbackFn s_Callback;

typedef struct LogTLSData
{
    char fmtBuffer[1024];
    char buffer[1024];
    bool isLogging;

} LogTLSData;

void formatArgs(const char* fmt, va_list argsList, char* buffer);
void format(char* buffer, const char* fmt, ...);
void consoleWrite(PalLogLevel level, const char* msg);

void _PCALL palLog(PalLogLevel level, const char* fmt, ...)
{
    LogTLSData* data = palGetTLS(s_LogTLSID);
    if (!data) {
        data = palAlloc(sizeof(LogTLSData));
        palZeroMemory(data, sizeof(LogTLSData));
    }

    data->isLogging = PAL_FALSE;
    if (data->isLogging) {
        // prevent recursive calls
        return; 
    }

    data->isLogging = PAL_TRUE;
    va_list argPtr;
    va_start(argPtr, fmt);
    formatArgs(fmt, argPtr, data->fmtBuffer);
    va_end(argPtr);
    format(data->buffer, "%s\n", data->fmtBuffer);

    if (s_Callback) {
        s_Callback(level, data->buffer);

    } else {
        consoleWrite(level, data->buffer);
    }

    data->isLogging = PAL_FALSE;
    palSetTLS(s_LogTLSID, data);
}

void _PCALL palLogConsole(PalLogLevel level, const char* fmt, ...)
{
    LogTLSData* data = palGetTLS(s_LogTLSID);
    if (!data) {
        data = palAlloc(sizeof(LogTLSData));
        palZeroMemory(data, sizeof(LogTLSData));
    }

    va_list argPtr;
    va_start(argPtr, fmt);
    formatArgs(fmt, argPtr, data->fmtBuffer);
    va_end(argPtr);
    format(data->buffer, "%s\n", data->fmtBuffer);

    consoleWrite(level, data->buffer);
    palSetTLS(s_LogTLSID, data);
}

void _PCALL palSetLogCallback(PalLogCallbackFn callback)
{
    s_Callback = callback;
}

PalLogCallbackFn _PCALL palGetLogCallback()
{
    return s_Callback; 
}

const char* _PCALL palGetLastLog()
{
    LogTLSData* data = palGetTLS(s_LogTLSID);
    if (!data) {
        return PAL_NULL;
    }
    return data->buffer;
}

void formatArgs(const char* fmt, va_list argsList, char* buffer)
{
    va_list listCopy;
#ifdef _MSC_VER
        listCopy = argsList;
#elif defined(P_PLATFORM_APPLE)
        listCopy = argsList;
#else
        __builtin_va_copy(listCopy, argsList);
#endif // _MSC_VER

    int len = vsnprintf(0, 0, fmt, listCopy);
    vsnprintf(buffer, len + 1, fmt, listCopy);
    buffer[len] = 0;
}

void format(char* buffer, const char* fmt, ...)
{
    va_list argPtr;
    va_start(argPtr, fmt);
    formatArgs(fmt, argPtr, buffer);
    va_end(argPtr);
}

void consoleWrite(PalLogLevel level, const char* msg)
{
    HANDLE console = PAL_NULL;
    bool error = level > PAL_LOG_LEVEL_WARN;
    static int levels[4] = { 8, 2, 6, 4 };
    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    SetConsoleTextAttribute(console, levels[level]);
    Uint64 len = strlen(msg);
    DWORD written = 0;
    WriteConsoleA(console, msg, (DWORD)len, &written, 0);
    OutputDebugStringA(msg);
}