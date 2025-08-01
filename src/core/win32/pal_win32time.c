
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

// ==================================================
// Public API
// ==================================================

PalTimer _PCALL palGetSystemTimer() {

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
