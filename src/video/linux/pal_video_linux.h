
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_VIDEO_LINUX_H
#define _PAL_VIDEO_LINUX_H
#ifdef __linux__
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#include "pal/pal_video.h"
#include <errno.h>

#define NULL_BUTTON_SERIAL 0xffffffffU
#define MAX_SPAN_MONITORS 4

#define TO_PAL_HANDLE(type, val) ((type*)(uintptr_t)(val))
#define FROM_PAL_HANDLE(type, handle) ((type)(uintptr_t)(handle))

#define EGL_CAST(type, value) ((type)(value))
#define EGL_OPENGL_API 0x30A2
#define EGL_OPENGL_BIT 0x0008
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES_API 0x30A0
#define EGL_NO_CONTEXT EGL_CAST(EGLContext, 0)
#define EGL_NO_DISPLAY EGL_CAST(EGLDisplay, 0)
#define EGL_NO_SURFACE EGL_CAST(EGLSurface, 0)
#define EGL_NATIVE_VISUAL_ID 0x302E

typedef void* EGLConfig;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLNativeDisplayType;

typedef int32_t EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;

typedef void* (*eglGetProcAddressFn)(const char*);

typedef EGLBoolean (*eglInitializeFn)(
    EGLDisplay,
    EGLint*,
    EGLint*);

typedef EGLBoolean (*eglTerminateFn)(EGLDisplay);

typedef EGLDisplay (*eglGetDisplayFn)(void*);

typedef EGLBoolean (*eglChooseConfigFn)(
    EGLDisplay,
    const EGLint*,
    EGLConfig*,
    EGLint,
    EGLint*);

typedef EGLBoolean (*eglGetConfigAttribFn)(
    EGLDisplay,
    EGLConfig,
    EGLint,
    EGLint*);

typedef EGLint (*eglGetErrorFn)(void);

typedef EGLBoolean (*eglBindAPIFn)(EGLenum);

typedef EGLBoolean (*eglGetConfigsFn)(
    EGLDisplay,
    EGLConfig*,
    EGLint,
    EGLint*);

typedef struct {
    PalBool pendingScroll;
    int32_t lastX;
    int32_t lastY;
    int32_t dx;
    int32_t dy;
    int32_t WheelX;
    int32_t WheelY;
    float WheelXf;
    float WheelYf;
    PalBool state[PAL_MOUSE_BUTTON_MAX];
    double tmpScrollX;
    double tmpScrollY;
    double accumScrollX;
    double accumScrollY;
} Mouse;

typedef struct {
    PalBool scancodeState[PAL_SCANCODE_MAX];
    PalBool keycodeState[PAL_KEYCODE_MAX];
    int repeatRate;
    int repeatDelay;
    int repeatKey;
    int repeatScancode;
    int scancodes[512];
    int keycodes[256];
    uint64_t timer;
    uint64_t frequency;
} Keyboard;

typedef struct {
    void* handle;
    eglInitializeFn eglInitialize;
    eglTerminateFn eglTerminate;
    eglGetDisplayFn eglGetDisplay;
    eglChooseConfigFn eglChooseConfig;
    eglGetConfigAttribFn eglGetConfigAttrib;
    eglGetErrorFn eglGetError;
    eglBindAPIFn eglBindAPI;
    eglGetConfigsFn eglGetConfigs;
} EGL;

typedef struct {
    // clang-format off
    void (*shutdownVideo)();
    void (*updateVideo)();
    PalResult (*setFBConfig)(const int, PalFBConfigBackend);
    PalResult (*enumerateMonitors)(int32_t*, PalMonitor**);
    PalResult (*getPrimaryMonitor)(PalMonitor**);
    PalResult (*getMonitorInfo)(PalMonitor*, PalMonitorInfo*);
    PalResult (*enumerateMonitorModes)(PalMonitor*, int32_t*, PalMonitorMode*);
    PalResult (*getCurrentMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*validateMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorOrientation)(PalMonitor*, PalOrientation);

    PalResult (*createWindow)(const PalWindowCreateInfo*, PalWindow**);
    void (*destroyWindow)(PalWindow*);
    PalResult (*maximizeWindow)(PalWindow*);
    PalResult (*minimizeWindow)(PalWindow*);
    PalResult (*restoreWindow)(PalWindow*);
    PalResult (*showWindow)(PalWindow*);
    PalResult (*hideWindow)(PalWindow*);
    PalResult (*flashWindow)(PalWindow*, const PalFlashInfo*);
    PalResult (*getWindowStyle)(PalWindow*, PalWindowStyle*);
    PalResult (*getWindowMonitor)(PalWindow*, PalMonitor**);
    PalResult (*getWindowTitle)(PalWindow*, uint64_t, uint64_t*, char*);
    PalResult (*getWindowPos)(PalWindow*, int32_t*, int32_t*);
    PalResult (*getWindowSize)(PalWindow*, uint32_t*, uint32_t*);
    PalResult (*getWindowState)(PalWindow*, PalWindowState*);
    PalBool (*isWindowVisible)(PalWindow*);
    PalWindow* (*getFocusWindow)();
    PalResult (*getWindowHandleInfo)(PalWindow*, PalWindowHandleInfo*);
    PalResult (*setWindowOpacity)(PalWindow*, float);
    PalResult (*setWindowStyle)(PalWindow*, PalWindowStyle);
    PalResult (*setWindowTitle)(PalWindow*, const char*);
    PalResult (*setWindowPos)(PalWindow*, int32_t, int32_t);
    PalResult (*setWindowSize)(PalWindow*, uint32_t, uint32_t);
    PalResult (*setFocusWindow)(PalWindow*);

    PalResult (*createIcon)(const PalIconCreateInfo*, PalIcon**);
    void (*destroyIcon)(PalIcon*);
    PalResult (*setWindowIcon)(PalWindow*, PalIcon*);

    PalResult (*createCursor)(const PalCursorCreateInfo*, PalCursor**);
    PalResult (*createCursorFrom)(PalCursorType, PalCursor**);
    void (*destroyCursor)(PalCursor*);
    void (*showCursor)(PalBool);
    PalResult (*clipCursor)(PalWindow*, PalBool);
    PalResult (*getCursorPos)(PalWindow*, int32_t*, int32_t*);
    PalResult (*setCursorPos)(PalWindow*, int32_t, int32_t);
    PalResult (*setWindowCursor)(PalWindow*, PalCursor*);

    PalResult (*attachWindow)(void*, PalWindow**);
    PalResult (*detachWindow)(PalWindow*, void**);
    // clang-format on
} Backend;

typedef struct {
    void* monitor;
    int dpi;
} SpanMonitor;

typedef struct {
    PalBool skipConfigure;
    PalBool skipState;
    PalBool used;
    PalBool isAttached;
    PalBool skipIfAttached;
    PalBool focused;
    PalBool pushConfigureEvent;
    PalBool pushStateEvent;
    int x;
    int y;
    uint32_t w;
    int dpi;
    int monitorCount;
    uint32_t h;
    PalWindowState state;
    PalWindow* window;

    // X11 only
    void* ic;
    unsigned long colormap;

    // Wayland only
    void* xdgSurface;
    void* xdgToplevel;
    void* buffer;
    void* decoration;
    void* cursor;
    void* eglWindow;
    SpanMonitor monitors[MAX_SPAN_MONITORS];
} WindowData;

typedef struct {
    PalBool used;
    int dpi;
    int x;
    int y;
    uint32_t w;
    uint32_t h;
    uint32_t refreshRate;
    uint32_t wlName;
    PalOrientation orientation;
    PalMonitor* monitor;
    PalMonitorMode mode; // wayland only sends current
    char name[32];
} MonitorData;

typedef struct {
    PalBool initialized;
    int32_t maxWindowData;
    int32_t maxMonitorData;
    int32_t pixelFormat;
    PalVideoFeatures features;
    const PalAllocator* allocator;
    PalEventDriver* eventDriver;
    const Backend* backend;
    WindowData* windowData;
    MonitorData* monitorData;
    const char* className;
    void* display;
} VideoLinux;

extern VideoLinux s_Video;
extern Mouse s_Mouse;
extern Keyboard s_Keyboard;
extern EGL s_Egl;

static WindowData* getFreeWindowData()
{
    for (int i = 0; i < s_Video.maxWindowData; ++i) {
        if (!s_Video.windowData[i].used) {
            s_Video.windowData[i].used = PAL_TRUE;
            return &s_Video.windowData[i];
        }
    }

    // resize the data array
    // It is rare for a user to create and manage
    // 32 windows at the same time
    WindowData* data = nullptr;
    int count = s_Video.maxWindowData * 2; // double the size
    int freeIndex = s_Video.maxWindowData + 1;
    data = palAllocate(s_Video.allocator, sizeof(WindowData) * count, 0);
    if (data) {
        memcpy(data, s_Video.windowData, s_Video.maxWindowData * sizeof(WindowData));

        palFree(s_Video.allocator, s_Video.windowData);
        s_Video.windowData = data;
        s_Video.maxWindowData = count;

        s_Video.windowData[freeIndex].used = PAL_TRUE;
        return &s_Video.windowData[freeIndex];
    }
    return nullptr;
}

static WindowData* findWindowData(PalWindow* window)
{
    for (int i = 0; i < s_Video.maxWindowData; ++i) {
        if (s_Video.windowData[i].used && s_Video.windowData[i].window == window) {
            return &s_Video.windowData[i];
        }
    }
    return nullptr;
}

static void resetMonitorData()
{
    memset(s_Video.monitorData, 0, s_Video.maxMonitorData * sizeof(MonitorData));
}

static MonitorData* getFreeMonitorData()
{
    for (int i = 0; i < s_Video.maxMonitorData; ++i) {
        if (!s_Video.monitorData[i].used) {
            s_Video.monitorData[i].used = PAL_TRUE;
            return &s_Video.monitorData[i];
        }
    }

    // resize the data array
    // this will almost not reach here since most setups are 1-4 monitors
    MonitorData* data = nullptr;
    int count = s_Video.maxMonitorData * 2; // double the size
    int freeIndex = s_Video.maxMonitorData + 1;
    data = palAllocate(s_Video.allocator, sizeof(MonitorData) * count, 0);
    if (data) {
        memcpy(data, s_Video.monitorData, s_Video.maxMonitorData * sizeof(MonitorData));

        palFree(s_Video.allocator, s_Video.monitorData);
        s_Video.monitorData = data;
        s_Video.maxWindowData = count;

        s_Video.monitorData[freeIndex].used = PAL_TRUE;
        return &s_Video.monitorData[freeIndex];
    }
    return nullptr;
}

static MonitorData* findMonitorData(PalMonitor* monitor)
{
    for (int i = 0; i < s_Video.maxMonitorData; ++i) {
        if (s_Video.monitorData[i].used && s_Video.monitorData[i].monitor == monitor) {
            return &s_Video.monitorData[i];
        }
    }
    return nullptr;
}

static void freeMonitorData(PalMonitor* monitor)
{
    for (int i = 0; i < s_Video.maxMonitorData; ++i) {
        if (s_Video.monitorData[i].used && s_Video.monitorData[i].monitor == monitor) {
            s_Video.monitorData[i].used = PAL_FALSE;
        }
    }
}

static inline uint64_t getCurrentTime()
{
    uint64_t now = palGetPerformanceCounter();
    return (now * 1000) / s_Keyboard.frequency;
}

#endif // __linux__
#endif // _PAL_VIDEO_LINUX_H