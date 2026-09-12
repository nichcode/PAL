
#include "pal2/pal_core.h"

#ifdef _WIN32
#define LIBRARY_NAME "ucrtbase.dll"
#define SYMBOL_NAME "puts"
#else
#define LIBRARY_NAME "libc.so.6"
#define SYMBOL_NAME "printf"
#endif // _WIN32

typedef int (*PrintfFn)(const char* restrict fmt, ...);

int main(void)
{
    palLog(nullptr, "Running library test...");

    PalLibrary* library = palLoadLibrary(LIBRARY_NAME);
    if (!library) {
        palLog(nullptr, "Failed to load library");
        return -1;
    }

    PrintfFn print = (PrintfFn)palGetSymbol(library, SYMBOL_NAME);
    if (!print) {
        palLog(nullptr, "Failed to get function symbol");
        palFreeLibrary(library);
        return -1;
    }

    palFreeLibrary(library);

    return 0;
}