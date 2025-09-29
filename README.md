
# PAL (Platform Abstraction Layer)

## Overview

PAL is a lightweight, low-level, cross-platform abstraction layer in 
**C** designed to be **explicit**, 
and as close to the **OS** as possible — similar in philosophy to Vulkan.

## Philosophy

- Minimal overhead (close to raw OS calls)  
- Explicit API (no hidden behavior or magic defaults)  
- Event system supporting both polling and callbacks.
- Written in C for easy integration  
- Extendable: users can plug in their own backends (e.g. event queue, allocator)
- Stateless: Objects like windows, threads and monitors are represented by opaque handles. You choose when to query or cache their state. PAL does not do internal caching, all calls goes to the underlying Os.
- No lowest common denominator: PAL does not “dumb down” APIs to the weakest platform. Instead, it exposes capabilities explicitly, and you decide what to use. PAL exposes functionality and capabilities as features that must be used to ensure correctness across platforms.
- Modular: PAL opengl system can be used with a window from another library. (eg. SDL, GLFW).
- Explicit Build: PAL supports modular builds. You can build only the video system at compile time.

### Supported Platforms

- Windows (Vista+)

### Dependencies

- Standard C library
- Platform SDKs (Win32, X11, Cocoa, etc.)
- [Make for Windows](https://www.gnu.org/software/make/) (If not using Visual Studio)

### Compilers

- GCC
- Clang
- MSVC

## Build

PAL written in **C99**. 
PAL supports modular builds and uses premake as its build system. Use [pal_config.lua](https://github.com/nichcode/PAL/blob/master/pal_config.lua) to choose which modules to build.
To build PAL tests, set `PAL_BUILD_TESTS = true` in the file above. See [pal_config.h](https://github.com/nichcode/PAL/blob/master/include/pal_config.h) to see the reflection of modules that will be built.

Run `premake\premake5.exe gmake2` from the root direcory to generate the project with makefile. This uses the config file (above) to choose the modules to build. If --compiler is not set, **GCC** will be used. To use clang, run `premake\premake5.exe gmake2 --compiler=clang`.

Run `premake\premake5.exe vs2022` from the root direcory to generate the project with msvc (Visual studio). This uses the config file (above) to choose the modules to build. if --compiler is not set, **MSVC** will be used. To use clang, run `premake\premake5.exe vs2022 --compiler=clang`.

## Getting Started

See [tests](https://github.com/nichcode/PAL/blob/master/tests) for more information.

### Core

```C
#include "pal/pal_core.h"

typedef struct {
    Uint64 frequency;
    Uint64 startTime;
} MyTimer;

void PAL_CALL onLog(void* userData, const char* msg) 
{
    palLog(nullptr, "Using Default Logger");
}

inline double getTime(MyTimer* timer)
{
    Uint64 now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

int main (int argc, char** argv) 
{
    PalLogger logger;
    logger.callback = onLog;
    logger.userData = nullptr;

    palLog(nullptr, "Default logger");
    palLog(&logger, "Custom logger");

    MyTimer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    // get the start time normalize by timer.startTime
    double lastTime = getTime(&timer);

    // get version
    PalVersion version = palGetVersion();
    palLog(nullptr, "%s: %s", "PAL Version", palGetVersionString());

    return 0;
}
```

### Event

```C
#include "pal/pal_event.h"

void PAL_CALL onEvent(void* userData, const PalEvent* event)
{
    
}

int main (int argc, char** argv) 
{
    PalResult result;
    PalEventDriver* driver = nullptr;
    PalEventDriverCreateInfo createInfo = {0};
    createInfo.callback = onEvent;
    createInfo.allocator = nullptr; // default allocator
    createInfo.queue = nullptr; // default event queue

    // create the event driver
    result = palCreateEventDriver(&createInfo, &driver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver %s", error);
        return -1;
    }

    // set dispatch mode for all event types
    // PAL_DISPATCH_CALLBACK - for callback.
    // PAL_DISPATCH_POLL - for polling.
    for (Uint32 e = 0; e < PAL_EVENT_MAX; e++) {
        palSetEventDispatchMode(driver, e, PAL_DISPATCH_POLL);
    }

    bool pushed = false;
    while (!pushed) {
        // push an event
        PalEvent event = {0};
        event.type = PAL_EVENT_USER;
        event.userId = 1200;
        palPushEvent(driver, &event);

        // poll event
        while (palPollEvent(driver, &event)) {
            pushed = true;
        }
    }

    // destroy the event driver
    palDestroyEventDriver(driver);

    return 0;
}
```

### Opengl

```C
#include "pal/pal_opengl.h"
#include "pal/pal_video.h"

static const char* g_BoolsToSting[2] = {"False", "True"};

// opengl typedefs
typedef void(PAL_GL_APIENTRY* PFNGLCLEARCOLORPROC)(
    float red,
    float green,
    float blue,
    float alpha);

typedef void(PAL_GL_APIENTRY* PFNGLCLEARPROC)(
    Uint32 mask);

int main (int argc, char** argv) 
{
    // initialize the opengl system
    PalResult result = palInitGL(nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize opengl: %s", error);
        return -1;
    }

    PalWindow* window = nullptr;
    PalGLContext* context = nullptr;
    PalWindowCreateInfo createInfo = {0};
    PalGLContextCreateInfo contextCreateInfo = {0};
    Int32 fbCount;
    bool running = false;

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver %s", error);
        return -1;
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video: %s", error);
        return -1;
    }

    createInfo.monitor = nullptr; // use primary monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "Pal Opengl Context Window";

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window %s", error);
        return -1;
    }

    // we set window close to poll
    palSetEventDispatchMode(
        eventDriver,
        PAL_EVENT_WINDOW_CLOSE,
        PAL_DISPATCH_POLL);

    // get window handle. You can use any window from any library
    // so long as you can get the window handle and display (if on X11, wayland)
    // If pal video system will not be used, there is no need to initialize it
    PalWindowHandleInfo windowHandleInfo;
    windowHandleInfo = palGetWindowHandleInfo(window);

    // PalGLWindow is just a struct to hold native handles
    PalGLWindow glWindow = {0};
    // needed when using X11 or wayland
    glWindow.display = windowHandleInfo.nativeDisplay;
    glWindow.window = windowHandleInfo.nativeWindow;

    // use the gl window to query supported FBconfigs
    result = palEnumerateGLFBConfigs(&glWindow, &fbCount, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query GL FBConfigs %s", error);
        return -1;
    }

    palLog(nullptr, "GL FBConfig count: %d", fbCount);
    if (fbCount == 0) {
        palLog(nullptr, "No supported FBConfig found");
        return -1;
    }

    PalGLFBConfig* fbConfigs = nullptr;
    fbConfigs = palAllocate(nullptr, sizeof(PalGLFBConfig) * fbCount, 0);
    if (!fbConfigs) {
        palLog(nullptr, "Failed to allocate memory");
        return -1;
    }

    result = palEnumerateGLFBConfigs(&glWindow, &fbCount, fbConfigs);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to query GL FBConfigs %s", error);
        palFree(nullptr, fbConfigs);
        return -1;
    }

    // we desire a FB config and see what is closest the driver will give us
    PalGLFBConfig desired = {0};
    desired.redBits = 8;
    desired.greenBits = 8;
    desired.blueBits = 8;
    desired.alphaBits = 8;
    desired.alphaBits = 8;
    desired.depthBits = 24;
    desired.stencilBits = 8;
    desired.samples = 2;

    desired.stereo = false; // not widely supported
    desired.sRGB = true;
    desired.doubleBuffer = true;

    // get the closest
    const PalGLFBConfig* closest = nullptr;
    closest = palGetClosestGLFBConfig(fbConfigs, fbCount, &desired);

    // log the closest GL FBConfig
    palLog(nullptr, "Closest GL FBConfig:");
    palLog(nullptr, " Index: %d", closest->index);
    palLog(nullptr, " Red Bits: %d", closest->redBits);
    palLog(nullptr, " Green Bits: %d", closest->greenBits);
    palLog(nullptr, " Blue Bits: %d", closest->blueBits);
    palLog(nullptr, " Alpha Bits: %d", closest->alphaBits);
    palLog(nullptr, " Depth Bits: %d", closest->depthBits);
    palLog(nullptr, " Stencil Bits: %d", closest->stencilBits);

    palLog(nullptr, " Samples: %d", closest->samples);
    palLog(nullptr, " DoubleBuffer: %s", g_BoolsToSting[closest->doubleBuffer]);
    palLog(nullptr, " Stereo: %s", g_BoolsToSting[closest->stereo]);
    palLog(nullptr, " sRGB: %s", g_BoolsToSting[closest->sRGB]);
    palLog(nullptr, "");

    // get opengl info
    const PalGLInfo* info = palGetGLInfo();

    // fill the context create info with the closest FBConfig
    contextCreateInfo.debug = true;        // debug context
    contextCreateInfo.fbConfig = closest;  // we use the closest to what we want
    contextCreateInfo.major = info->major; // context major
    contextCreateInfo.minor = info->minor; // context minor
    contextCreateInfo.noError = false;     // check PAL_GL_EXTENSION_NO_ERROR

    // check PAL_GL_EXTENSION_FLUSH_CONTROL
    contextCreateInfo.release = PAL_GL_RELEASE_BEHAVIOR_NONE;

    // check PAL_GL_EXTENSION_ROBUSTNESS
    contextCreateInfo.reset = PAL_GL_CONTEXT_RESET_NONE;

    contextCreateInfo.shareContext = nullptr; // no shared context
    contextCreateInfo.window = &glWindow;

    if (info->extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        contextCreateInfo.forward = true;
    }

    if (info->extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        contextCreateInfo.profile = PAL_GL_PROFILE_CORE;
    }

    // create the opengl context with the context create info
    result = palCreateGLContext(&contextCreateInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create opengl context: %s", error);
        palFree(nullptr, fbConfigs);
        return -1;
    }

    // make the context current and optionally set vsync if supported
    result = palMakeContextCurrent(&glWindow, context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to make opengl context current: %s", error);
        palFree(nullptr, fbConfigs);
        return -1;
    }

    if (info->extensions & PAL_GL_EXTENSION_SWAP_CONTROL) {
        // vsync is supported. This is set for the current context
        palSetSwapInterval(1);
    }

    // load function procs
    PFNGLCLEARCOLORPROC glClearColor = nullptr;
    PFNGLCLEARPROC glClear = nullptr;
    glClearColor = (PFNGLCLEARCOLORPROC)palGLGetProcAddress("glClearColor");
    glClear = (PFNGLCLEARPROC)palGLGetProcAddress("glClear");

    // set clear color
    glClearColor(.2f, .2f, .2f, .2f);

    running = true;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = false;
                    break;
                }
            }
        }

        // clear the buffer
        glClear(0x00004000); // GL_COLOR_BUFFER_BIT

        // swap buffers
        result = palSwapBuffers(&glWindow, context);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to swap buffers: %s", error);
            palFree(nullptr, fbConfigs);
            return -1;
        }
    }

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    // destroy the opengl context
    palDestroyGLContext(context);

    // shutdown the opengl system
    palShutdownGL();

    // free the framebuffer configs
    palFree(nullptr, fbConfigs);

    return 0;
}
```

### System

```C
#include "pal/pal_system.h"
#include <string.h> // for strcat

inline const char* platformToString(PalPlatformType type)
{
    switch (type) {
        case PAL_PLATFORM_WINDOWS:
            return "Windows";

        case PAL_PLATFORM_LINUX:
            return "Linux";

        case PAL_PLATFORM_MACOS:
            return "MacOs";

        case PAL_PLATFORM_ANDROID:
            return "Android";

        case PAL_PLATFORM_IOS:
            return "Ios";
    }
    return nullptr;
}

inline const char* platformApiToString(PalPlatformApiType type)
{
    switch (type) {
        case PAL_PLATFORM_API_WIN32:
            return "Win32";

        case PAL_PLATFORM_API_X11:
            return "X11";

        case PAL_PLATFORM_API_WAYLAND:
            return "Wayland";
    }
    return nullptr;
}

inline const char* cpuArchToString(PalCpuArch arch)
{
    switch (arch) {
        case PAL_CPU_ARCH_UNKNOWN:
            return "Unknown";

        case PAL_CPU_ARCH_X86:
            return "X86";

        case PAL_CPU_ARCH_X86_64:
            return "X64";

        case PAL_CPU_ARCH_ARM:
            return "Arm";

        case PAL_CPU_ARCH_ARM64:
            return "Arm64";
    }
    return nullptr;
}

int main (int argc, char** argv) 
{
    PalResult result;
    PalCPUInfo cpuInfo;
    PalPlatformInfo platformInfo;

    // get the platform info. Users must cache this
    result = palGetPlatformInfo(&platformInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get platform info: %s", error);
        return -1;
    }

    // user defined allocator, set to override the default one or nullptr for default
    result = palGetCPUInfo(nullptr, &cpuInfo);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to get Cpu info %s", error);
        return -1;
    }

    // log platform information
    palLog(nullptr, "Platform: %s", platformToString(platformInfo.type));
    palLog(nullptr, " Name: %s", platformInfo.name);
    palLog(nullptr, " API: %s", platformApiToString(platformInfo.apiType));
    palLog(nullptr, " Total RAM: %llu MB", platformInfo.totalRAM);
    palLog(nullptr, " Total Memory: %llu GB", platformInfo.totalMemory);

    Uint16 major, minor, build;
    major = platformInfo.version.major;
    minor = platformInfo.version.minor;
    build = platformInfo.version.build;
    palLog(nullptr, " Version: (%d.%d.%d)", major, minor, build);

    // log cpu information
    const char* archString = cpuArchToString(cpuInfo.architecture);
    Int32 processors = cpuInfo.numLogicalProcessors;

    palLog(nullptr, " Cpu: %s", cpuInfo.model);
    palLog(nullptr, "  Vendor: %s", cpuInfo.vendor);
    palLog(nullptr, "  Architecture: %s", archString);
    palLog(nullptr, "  Number Of Cores: %d", cpuInfo.numCores);
    palLog(nullptr, "  Number Of Processors: %d", processors);
    palLog(nullptr, "  L1 Cache KB: %d", cpuInfo.cacheL1);
    palLog(nullptr, "  L2 Cache KB: %d", cpuInfo.cacheL2);
    palLog(nullptr, "  L3 Cache KB: %d", cpuInfo.cacheL3);

    // get instruction sets
    char instructionSets[256] = {0};
    if (cpuInfo.features & PAL_CPU_FEATURE_SSE) {
        strcat(instructionSets, "SSE");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSE2) {
        strcat(instructionSets, " | SSE2");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSE3) {
        strcat(instructionSets, " | SSE3");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSSE3) {
        strcat(instructionSets, " | SSSE3");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSE41) {
        strcat(instructionSets, " | SSE4.1");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_SSE42) {
        strcat(instructionSets, " | SSE4.2");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_AVX) {
        strcat(instructionSets, " | AVX");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_AVX2) {
        strcat(instructionSets, "| AVX2");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_AVX512F) {
        strcat(instructionSets, "| AVX-512F");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_FMA3) {
        strcat(instructionSets, " | FMA3");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_BMI1) {
        strcat(instructionSets, " | BMI1");
    }

    if (cpuInfo.features & PAL_CPU_FEATURE_BMI2) {
        strcat(instructionSets, " | BMI2");
    }

    palLog(nullptr, "  Instructions Sets: %s", instructionSets);

    return 0;
}
```

### Thread

```C
#include "pal/pal_core.h"
#include "pal/pal_thread.h"

#define THREAD_TIME 1000
#define THREAD_COUNT 4

static const char* g_ThreadNames[THREAD_COUNT] = {
    "Thread1", 
    "Thread2", 
    "Thread3", 
    "Thread4"};

void* PAL_CALL worker(void* arg)
{
    // palLog is thread safe so there should'nt be any race conditions
    Int32 id = (Int32)(IntPtr)arg;
    palLog(nullptr, "Thread %d: started", id);

    palSleep(THREAD_TIME * id);
    palLog(nullptr, "Thread %d: finished", id);
    return nullptr;
}

int main (int argc, char** argv)
{
    PalResult result;
    PalThread* threads[THREAD_COUNT];

    // fill the thread creation struct
    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;      // will be the same for all threads
    createInfo.stackSize = 0;       // same for all threads
    createInfo.allocator = nullptr; // default

    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        createInfo.arg = (void*)((IntPtr)i + 1);

        // create thread
        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create thread: %s", error);
            return -1;
        }
    }

    // join threads
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        // we dont need the return value
        result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to join threads: %s", error);
            return -1;
        }
    }

    // check if we support setting thread names and set for each created thread
    PalThreadFeatures features = palGetThreadFeatures();
    if (features & PAL_THREAD_FEATURE_NAME) {
        for (Int32 i = 0; i < THREAD_COUNT; i++) {
            result = palSetThreadName(threads[i], g_ThreadNames[i]);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to set thread name %s", error);
                return -1;
            }
        }
    }

    if (features & PAL_THREAD_FEATURE_NAME) {
        char buffer[64] = {0};
        for (Int32 i = 0; i < THREAD_COUNT; i++) {
            palGetThreadName(threads[i], 64, nullptr, buffer);
            palLog(
                nullptr,
                "Thread %d: (%s) finnished successfully",
                i + 1,
                buffer);
        }
    }

    palLog(nullptr, "All threads finished successfully");

    // detach threads
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        palDetachThread(threads[i]);
        palLog(nullptr, "Thread %d: detached", i + 1);
    }

    return 0;
}
```

### Video

```C
#include "pal/pal_video.h"

int main (int argc, char** argv)
{
    PalResult result;
    PalWindow* window = nullptr;
    PalWindowCreateInfo createInfo = {0};
    PalVideoFeatures features;
    bool running = false;

    // event driver
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo = {0};

    // fill the event driver create info
    eventDriverCreateInfo.allocator = nullptr; // default allocator
    eventDriverCreateInfo.callback = nullptr;  // for callback dispatch
    eventDriverCreateInfo.queue = nullptr;     // default queue
    eventDriverCreateInfo.userData = nullptr;  // null

    // create the event driver
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create event driver %s", error);
        return -1;
    }

    // initialize the video system. We pass the event driver to recieve video
    // related events the video system does not copy the event driver, it must
    // be valid till the video system is shutdown
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to initialize video %s", error);
        return -1;
    }

    // get video system features
    features = palGetVideoFeatures();

    // fill the create info struct
    createInfo.monitor = nullptr; // use primary monitor
    createInfo.height = 480;
    createInfo.width = 640;
    createInfo.show = true;
    createInfo.style = PAL_WINDOW_STYLE_RESIZABLE;
    createInfo.title = "PAL Test Window Unicode - àà";

    // create the window with the create info struct
    result = palCreateWindow(&createInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create window %s", error);
        return -1;
    }

    // set dispatch mode for all window related events.
    for (Uint32 e = 0; e < PAL_EVENT_KEYDOWN; e++) {
        palSetEventDispatchMode(eventDriver, e, PAL_DISPATCH_POLL);
    }

    running = true;
    while (running) {
        // update the video system to push video events
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = false;
                    break;
                }

                case PAL_EVENT_WINDOW_SIZE: {
                    onWindowResize(&event);
                    break;
                }

                case PAL_EVENT_WINDOW_MOVE: {
                    onWindowMove(&event);
                    break;
                }

                case PAL_EVENT_WINDOW_VISIBILITY: {
                    onWindowVisibility(&event);
                    break;
                }

                case PAL_EVENT_WINDOW_STATE: {
                    onWindowState(&event);
                    break;
                }

                case PAL_EVENT_WINDOW_FOCUS: {
                    onWindowFocus(&event);
                    break;
                }

                case PAL_EVENT_MONITOR_DPI_CHANGED: {
                    onMonitorDPI(&event);
                    break;
                }

                case PAL_EVENT_MONITOR_LIST_CHANGED: {
                    onMonitorList(&event);
                    break;
                }
            }
        }

        // update/render
    }

    // destroy the window
    palDestroyWindow(window);

    // shutdown the video system
    palShutdownVideo();

    // destroy the event driver
    palDestroyEventDriver(eventDriver);

    return 0;
}
```

## Modules

- `pal_core` - memory, log, time, version
- `pal_video` - windows, monitors, mouse, keyboard
- `pal_event` - event queue, event callback
- `pal_thread` - threads, synchronization
- `pal_opengl` - Framebuffer configs, context

## Planned Modules:
- `pal_graphics` - Vulkan, D3D12, Metal, Custom
- `pal_network`
- `pal_audio`
- `pal_hid`
- `pal_filesystem`

## Documentation

PAL uses [Doxygen](https://www.doxygen.nl/) for generating API documentation.  
Run `doxygen` in the `docs/` folder to generate HTML API documentation.

## Contributing

Contributions are welcome! Please open an issue or pull request.

## License

Zlib License