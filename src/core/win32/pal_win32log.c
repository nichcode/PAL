
/**

Copyright (C) 2025 Nicholas Agbo
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

 */

#include "pal_pch.h"
#include "pal/pal_core.h"

#define LOG_SIZE 1024

static DWORD s_TlsID = 0;
static const char* s_NewlineString = "\n";

typedef struct {
    char buffer[LOG_SIZE];
    wchar_t wideBuffer[LOG_SIZE];
} LogTLSDataWin32;

static LogTLSDataWin32* getLogTlsData();

// ==================================================
// Public API
// ==================================================

void _PCALL palLog(const char* fmt, ...) {

    if (!fmt) {
        return;
    }

    LogTLSDataWin32* data = getLogTlsData();

    // get arguments
    va_list argPtr;
    va_start(argPtr, fmt);
    va_list listCopy;
#ifdef _MSC_VER
    listCopy = argPtr;
#else
    __builtin_va_copy(listCopy, argPtr);
#endif // _MSC_VER

    int len = vsnprintf(0, 0, fmt, listCopy);
    if (!len) {
        return;
    }

    vsnprintf(data->buffer, len + 1, fmt, listCopy);
    data->buffer[len] = 0;
    va_end(argPtr);

    // log to console
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    len = MultiByteToWideChar(CP_UTF8, 0, data->buffer, -1, PAL_NULL, 0);
    if (!len) {
        return;
    }

    MultiByteToWideChar(CP_UTF8, 0, data->buffer, -1, data->wideBuffer, len);
    if (console) {
        WriteConsoleW(console, data->wideBuffer, (DWORD)len - 1, NULL, 0);
        WriteConsoleA(console, s_NewlineString, 1, NULL, 0);
    } else {
        OutputDebugStringW(data->wideBuffer);
        OutputDebugStringA(s_NewlineString);
    }
}

// ==================================================
// Internal API
// ==================================================

static LogTLSDataWin32* getLogTlsData() {

    LogTLSDataWin32* data = TlsGetValue(s_TlsID);
    if (!data) {
        data = palAllocate(PAL_NULL, sizeof(LogTLSDataWin32));
        palZeroMemory(data, sizeof(LogTLSDataWin32));

        // create TLS
        s_TlsID = TlsAlloc();
        TlsSetValue(s_TlsID, data);
    }
    return data;
}