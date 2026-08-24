
#ifndef _TESTS_H
#define _TESTS_H

#include "pal2/pal_video.h"
#include <stdio.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

typedef PalBool (*TestFn)();

// a simple timer object to hold frequency and start time
typedef struct {
    uint64_t frequency;
    uint64_t startTime;
} MyTimer;

void registerTest(
    TestFn func,
    const char* name);
void runTests();

static inline void logResult(
    PalResult result,
    const char* msg)
{
    char buffer[256];
    palFormatResult(result, 256, buffer);
    palLog(nullptr, "%s \n %s", msg, buffer);
}

// get the time in seconds
static inline double getTime(MyTimer* timer)
{
    uint64_t now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

static PalBool readFile(
    const char* filename,
    void* buffer,
    uint32_t* size)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return PAL_FALSE;
    }

    fseek(file, 0, SEEK_END);
    uint32_t tmpSize = (uint32_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    if (buffer) {
        tmpSize = *size;
        size_t read = fread(buffer, 1, tmpSize, file);
        if ((uint32_t)read != tmpSize) {
            return PAL_FALSE;
        }
    }

    fclose(file);
    *size = tmpSize;
    return PAL_TRUE;
}

static void writePPM(
    FILE* file, 
    uint32_t width, 
    uint32_t height, 
    void* ptr)
{
    fprintf(file, "P6\n%d %d\n255\n", width, height);
    float* pixels = (float*)ptr;
    for (int y = 0; y < height; y++) {
        int row = height - 1 - y; // flip y
        for (int x = 0; x < width; x++) {
            int index = row * width + x;
            uint8_t rgb[3];

            rgb[0] = pixels[index * 4 + 0] > 0.5f ? 255 : 0;
            rgb[1] = pixels[index * 4 + 1] > 0.5f ? 255 : 0;
            rgb[2] = pixels[index * 4 + 2] > 0.5f ? 255 : 0;
            fwrite(rgb, 1, 3, file);
        }
    }
}

PalEventDriver* helperCreateEventDriver(
    uint32_t count,
    PalDispatchMode mode,
    PalEventType* types);

#endif // _TESTS_H
