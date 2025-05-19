
#pragma once

#include "PAL_pch.h"
#include "PAL/PAL_platform.h"
#include "PAL_wgl_context.h"

void PAL_Init()
{
    s_Instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbClsExtra = 0;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursorW(s_Instance, IDC_ARROW);
    wc.hIcon = LoadIconW(s_Instance, IDI_APPLICATION);
    wc.hIconSm = LoadIconW(s_Instance, IDI_APPLICATION);
    wc.hInstance = s_Instance;
    wc.lpfnWndProc = PAL_Win32Proc;
    wc.lpszClassName = s_ClassName;
    wc.lpszMenuName = NULL;
    wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

    ATOM success = RegisterClassExW(&wc);
    if (!success) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to register win32 window class");
        return;
    }

    s_Data.gdi = LoadLibraryA("gdi32.dll");
    if (!s_Data.gdi) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to load gdi32.dll"); return;
    }
    s_Data.opengl = LoadLibraryA("opengl32.dll");
    if (!s_Data.opengl) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to load opengl.dll"); return;
    }

    PAL_WGLCreateDummyContext();
    PAL_InitInput();
    s_Data.init = true;

    PAL_LOG_INFO("PAL Platform Initialized");
}

void PAL_Terminate()
{
    UnregisterClassW(s_ClassName, s_Instance);

    if (s_Data.gdi) {
        FreeLibrary((HMODULE)s_Data.gdi);
    }
    if (s_Data.opengl) {
        FreeLibrary((HMODULE)s_Data.opengl);
    }
}

char* PAL_ToString(const wchar_t* wide_string)
{
    int len = WideCharToMultiByte(CP_UTF8, 0, wide_string, -1, nullptr, 0, 0, 0);
    if (!len) {
        PAL_ERROR(PAL_INVALID_POINTER, "Wide string is null or empty");
        return nullptr;
    }

    char* string = new char[len + 1];
    if (!string) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for the string");
        return nullptr;
    }
    
    WideCharToMultiByte(CP_UTF8, 0, wide_string, -1, string, len, 0, 0);
    return string;
}

wchar_t* PAL_ToWideString(const char* string)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);
    if (!len) {
        PAL_ERROR(PAL_INVALID_POINTER, "String is null or empty");
        return nullptr;
    }

    wchar_t* wstring = new wchar_t[sizeof(wchar_t) + len];
    if (!string) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for the wide string");
        return nullptr;
    }

    MultiByteToWideChar(CP_UTF8, 0, string, -1, wstring, len);
    return wstring;
}

char* PAL_Format(const char* fmt, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    char* result = PAL_FormatArgs(fmt, arg_ptr);
    va_end(arg_ptr);
    return result;
}

char* PAL_FormatArgs(const char* fmt, va_list args_list)
{
    va_list list_copy;
    __builtin_va_copy(list_copy, args_list);
    
    i32 len = vsnprintf(0, 0, fmt, list_copy);
    va_end(list_copy);

    char* string = new char[len + 1];
    if (!string) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for the string");
        return nullptr;
    }

    vsnprintf(string, len + 1, fmt, args_list);
    string[len] = 0;
    return string;
}

void PAL_WriteConsole(u32 log_level, const char* msg)
{
    b8 error = log_level > PAL_LOG_LEVEL_WARN;
    HANDLE console = NULL;
    static u8 levels[4] = { 8, 2, 6, 4 };

    if (error) {
        console = GetStdHandle(STD_ERROR_HANDLE);
    }
    else {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    SetConsoleTextAttribute(console, levels[log_level]);
    wchar_t* wstring = PAL_ToWideString(msg);
    auto len = wcslen(wstring);
    DWORD number_written = 0;

    WriteConsoleW(console, wstring, (DWORD)len, &number_written, 0);
    SetConsoleTextAttribute(console, 15);
    delete[] wstring;
}

void* PAL_LoadLibrary(const char* dll)
{
    void* library = LoadLibraryA(dll);
    if (!library) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to load library. Check library path");
        return nullptr;
    }
    return library;
}

void* PAL_GetProcAddress(void* dll, const char* func_name)
{
    if (!dll) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Library is null");
        return nullptr;
    }

    HMODULE library = (HMODULE)dll;
    FARPROC proc = GetProcAddress((HMODULE)library, func_name);
    if (!proc) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to get addrress of %s", func_name);
        return nullptr;
    }

    return (void*)proc;
}

void PAL_FreeLibrary(void* dll)
{
    if (!dll) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Library is null");
        return;
    }

    HMODULE library = (HMODULE)dll;
    FreeLibrary(library);
}