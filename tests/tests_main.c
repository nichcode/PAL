
#include "pal/pal_config.h" // for systems reflection
#include "tests.h"

// clang-format off
int main(int argc, char** argv)
{
    // clang-format on
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    // core
    // registerTest(loggerTest, "Logger Test");
    // registerTest(timeTest, "Time Test");

    // event
    // registerTest(eventTest, "Event test");
    // registerTest(userEventTest, "User Event Test");

#if PAL_HAS_SYSTEM
    // registerTest(systemTest, "System Test");
#endif // PAL_HAS_SYSTEM

#if PAL_HAS_THREAD
    // registerTest(threadTest, "Thread Test");
    // registerTest(tlsTest, "TLS Test");
    // registerTest(mutexTest, "Mutex Test");
    // registerTest(condvarTest, "Condvar Test");
#endif // PAL_HAS_THREAD

#if PAL_HAS_VIDEO
    // registerTest(videoTest, "Video Test");
    // registerTest(monitorTest, "Monitor Test");
    // registerTest(monitorModeTest, "Monitor Mode Test");
    // registerTest(windowTest, "Window Test");
    // registerTest(iconTest, "Icon Test");
    // registerTest(cursorTest, "Cursor Test");
    // registerTest(inputWindowTest, "Input Window Test");
    // registerTest(systemCursorTest, "System Cursor Test");
    // registerTest(attachWindowTest, "Attach Window Test");
    // registerTest(charEventTest, "Char Event Test");
    // registerTest(nativeIntegrationTest, "Native Integration Test");
    // registerTest(nativeInstanceTest, "Native Instance Test");
    // registerTest(customDecorationTest, "Custom Decoration Test");
#endif // PAL_HAS_VIDEO

    // This test can run without video system so long as your have a valid
    // window
#if PAL_HAS_OPENGL && PAL_HAS_VIDEO
    // registerTest(openglTest, "Opengl Test");
    // registerTest(openglFBConfigTest, "Opengl FBConfig Test");
    // registerTest(openglContextTest, "Context Test");
    // registerTest(openglMultiContextTest, "Opengl Multi Context Test");
#endif // PAL_HAS_OPENGL

#if PAL_HAS_OPENGL && PAL_HAS_VIDEO && PAL_HAS_THREAD
    // registerTest(multiThreadOpenGlTest, "Multi Thread Opengl Test");
#endif

#if PAL_HAS_GRAPHICS
    // registerTest(graphicsTest, "Graphics Test");
    // registerTest(computeTest, "Compute Test");
    // registerTest(rayTracingTest, "Ray Tracing Test");
    // registerTest(multiDescriptorSetTest, "Multi Descriptor Set Test");
#endif // PAL_HAS_GRAPHICS

#if PAL_HAS_GRAPHICS && PAL_HAS_VIDEO
    // registerTest(clearColorTest, "Clear Color Test");
    // registerTest(triangleTest, "Triangle Test");
    // registerTest(meshTest, "Mesh Test");
    // registerTest(textureTest, "Texture Test");
    // registerTest(geometryTest, "Geometry Test");
    registerTest(indirectDrawTest, "Indirect Draw Test");
#endif //

    runTests();
    return 0;
}
