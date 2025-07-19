
#include "pal_pch.h"
#include "pal/pal_core.h"

#ifdef _WIN32

PalTimer _PCALL palGetSysTimer() {

    LARGE_INTEGER freq, startTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&startTime);

    return (PalTimer) {
        .frequency = freq.QuadPart,
        .startTime = startTime.QuadPart
    };
}

double _PCALL palGetTime(PalTimer* timer) {
    if (!timer) {
        return 0.0;
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - timer->startTime) / (double)timer->frequency;
}

Uint64 _PCALL palGetPerformanceCounter() {

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (Uint64)counter.QuadPart;
}

Uint64 _PCALL palGetPerformanceFrequency() {

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return (Uint64)frequency.QuadPart;
}

#endif // _WIN32