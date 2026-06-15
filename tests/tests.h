
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal_core.h"
#include <stdio.h>

typedef PalBool (*TestFn)();

void registerTest(TestFn func, const char* name);
void runTests();

static PalBool readFile(
    const char* filename,
    void* buffer,
    uint64_t* size)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return PAL_FALSE;
    }

    fseek(file, 0, SEEK_END);
    uint64_t tmpSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (buffer) {
        tmpSize = *size;
        size_t read = fread(buffer, 1, tmpSize, file);
        if (read != tmpSize) {
            return PAL_FALSE;
        }
    }

    fclose(file);
    *size = tmpSize;
    return PAL_TRUE;
}

static inline void logResult(PalResult result, const char* msg)
{
    char buffer[256];
    palFormatResult(result, 256, buffer);
    palLog(nullptr, "%s \n %s", msg, buffer);
}

// core tests
PalBool loggerTest();
PalBool timeTest();
PalBool userEventTest();
PalBool eventTest();

// system tests
PalBool systemTest();

// system tests
PalBool threadTest();
PalBool tlsTest();
PalBool mutexTest();
PalBool condvarTest();

// video test
PalBool videoTest();
PalBool monitorTest();
PalBool monitorModeTest();
PalBool windowTest();
PalBool iconTest();
PalBool cursorTest();
PalBool inputWindowTest();
PalBool systemCursorTest();
PalBool attachWindowTest();
PalBool charEventTest();
PalBool nativeIntegrationTest();
PalBool nativeInstanceTest();
PalBool customDecorationTest();

// opengl test
PalBool openglTest();

// opengl and video test
PalBool openglFBConfigTest();
PalBool openglContextTest();
PalBool openglMultiContextTest();

// opengl, video and thread
PalBool multiThreadOpenGlTest();

// graphics
PalBool graphicsTest();
PalBool computeTest();
PalBool rayTracingTest();
PalBool multiDescriptorSetTest();

// graphics and video
PalBool clearColorTest();
PalBool triangleTest();
PalBool meshTest();
PalBool textureTest();
PalBool geometryTest();
PalBool indirectDrawTest();
PalBool descriptorIndexingTest();

#endif // _TESTS_H
