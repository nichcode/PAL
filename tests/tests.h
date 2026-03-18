
#ifndef _TESTS_H
#define _TESTS_H

#include "pal/pal_core.h"

typedef bool (*TestFn)();

void registerTest(TestFn func);
void runTests();

// core tests
bool loggerTest();
bool timeTest();
bool userEventTest();
bool eventTest();

// system tests
bool systemTest();

// system tests
bool threadTest();
bool tlsTest();
bool mutexTest();
bool condvarTest();

// video test
bool videoTest();
bool monitorTest();
bool monitorModeTest();
bool windowTest();
bool iconTest();
bool cursorTest();
bool inputWindowTest();
bool systemCursorTest();
bool attachWindowTest();
bool charEventTest();
bool nativeIntegrationTest();
bool nativeInstanceTest();
bool customDecorationTest();

// opengl test
bool openglTest();

// opengl and video test
bool openglFBConfigTest();
bool openglContextTest();
bool openglMultiContextTest();

// opengl, video and thread
bool multiThreadOpenGlTest();

// graphics
bool graphicsTest();
bool computeTest();
bool rayTracingTest();

// graphics and video
bool clearColorTest();
bool triangleTest();
bool meshTest();
bool textureTest();

#endif // _TESTS_H
