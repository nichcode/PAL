
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#include "pal_video_linux.h"
#include "pal_shared.h"
#include <dlfcn.h>
#include <stdlib.h>

EGL s_Egl = {0};
VideoLinux s_Video = {0};
Mouse s_Mouse = {0};
Keyboard s_Keyboard = {0};

#if PAL_HAS_X11_BACKEND == 1
PalResult xInitVideo();
void xShutdownVideo();
void xUpdateVideo();
PalResult xSetFBConfig(const int, PalFBConfigBackend);
PalResult xEnumerateMonitors(int32_t*, PalMonitor**);
PalResult xGetPrimaryMonitor(PalMonitor**);
PalResult xGetMonitorInfo(PalMonitor*, PalMonitorInfo*);
PalResult xEnumerateMonitorModes(PalMonitor*, int32_t*, PalMonitorMode*);
PalResult xGetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xSetMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xValidateMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xSetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult xCreateWindow(const PalWindowCreateInfo*, PalWindow**);
void xDestroyWindow(PalWindow*);
PalResult xMaximizeWindow(PalWindow*);
PalResult xMinimizeWindow(PalWindow*);
PalResult xRestoreWindow(PalWindow*);
PalResult xShowWindow(PalWindow*);
PalResult xHideWindow(PalWindow*);
PalResult xFlashWindow(PalWindow*, const PalFlashInfo*);
PalResult xGetWindowStyle(PalWindow*, PalWindowStyle*);
PalResult xGetWindowMonitor(PalWindow*, PalMonitor**);
PalResult xGetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
PalResult xGetWindowPos(PalWindow*, int32_t*, int32_t*);
PalResult xGetWindowSize(PalWindow*, uint32_t*, uint32_t*);
PalResult xGetWindowState(PalWindow*, PalWindowState*);
PalBool xIsWindowVisible(PalWindow*);
PalWindow* xGetFocusWindow();
PalResult xGetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
PalResult xSetWindowOpacity(PalWindow*, float);
PalResult xSetWindowStyle(PalWindow*, PalWindowStyle);
PalResult xSetWindowTitle(PalWindow*, const char*);
PalResult xSetWindowPos(PalWindow*, int32_t, int32_t);
PalResult xSetWindowSize(PalWindow*, uint32_t, uint32_t);
PalResult xSetFocusWindow(PalWindow*);

PalResult xCreateIcon(const PalIconCreateInfo*, PalIcon**);
void xDestroyIcon(PalIcon*);
PalResult xSetWindowIcon(PalWindow*, PalIcon*);

PalResult xCreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult xCreateCursorFrom(PalCursorType, PalCursor**);
void xDestroyCursor(PalCursor*);
void xShowCursor(PalBool);
PalResult xClipCursor(PalWindow*, PalBool);
PalResult xGetCursorPos(PalWindow*, int32_t*, int32_t*);
PalResult xSetCursorPos(PalWindow*, int32_t, int32_t);
PalResult xSetWindowCursor(PalWindow*, PalCursor*);

PalResult xAttachWindow(void*, PalWindow**);
PalResult xDetachWindow(PalWindow*, void**);

static Backend s_XBackend = {
    .shutdownVideo = xShutdownVideo,
    .updateVideo = xUpdateVideo,
    .setFBConfig = xSetFBConfig,
    .enumerateMonitors = xEnumerateMonitors,
    .getMonitorInfo = xGetMonitorInfo,
    .getPrimaryMonitor = xGetPrimaryMonitor,
    .enumerateMonitorModes = xEnumerateMonitorModes,
    .getCurrentMonitorMode = xGetCurrentMonitorMode,
    .setMonitorMode = xSetMonitorMode,
    .validateMonitorMode = xValidateMonitorMode,
    .setMonitorOrientation = xSetMonitorOrientation,

    .createWindow = xCreateWindow,
    .destroyWindow = xDestroyWindow,
    .maximizeWindow = xMaximizeWindow,
    .minimizeWindow = xMinimizeWindow,
    .restoreWindow = xRestoreWindow,
    .showWindow = xShowWindow,
    .hideWindow = xHideWindow,
    .flashWindow = xFlashWindow,
    .getWindowStyle = xGetWindowStyle,
    .getWindowMonitor = xGetWindowMonitor,
    .getWindowTitle = xGetWindowTitle,
    .getWindowPos = xGetWindowPos,
    .getWindowSize = xGetWindowSize,
    .getWindowState = xGetWindowState,
    .isWindowVisible = xIsWindowVisible,
    .getFocusWindow = xGetFocusWindow,
    .getWindowHandleInfo = xGetWindowHandleInfo,
    .setWindowOpacity = xSetWindowOpacity,
    .setWindowStyle = xSetWindowStyle,
    .setWindowTitle = xSetWindowTitle,
    .setWindowPos = xSetWindowPos,
    .setWindowSize = xSetWindowSize,
    .setFocusWindow = xSetFocusWindow,

    .createIcon = xCreateIcon,
    .destroyIcon = xDestroyIcon,
    .setWindowIcon = xSetWindowIcon,

    .createCursor = xCreateCursor,
    .createCursorFrom = xCreateCursorFrom,
    .destroyCursor = xDestroyCursor,
    .showCursor = xShowCursor,
    .clipCursor = xClipCursor,
    .getCursorPos = xGetCursorPos,
    .setCursorPos = xSetCursorPos,
    .setWindowCursor = xSetWindowCursor,

    .attachWindow = xAttachWindow,
    .detachWindow = xDetachWindow};

#endif // PAL_HAS_X11_BACKEND

#if PAL_HAS_WAYLAND_BACKEND == 1
PalResult wlInitVideo();
void wlShutdownVideo();
void wlUpdateVideo();
PalResult wlSetFBConfig(const int, PalFBConfigBackend);
PalResult wlEnumerateMonitors(int32_t*, PalMonitor**);
PalResult wlGetPrimaryMonitor(PalMonitor**);
PalResult wlGetMonitorInfo(PalMonitor*, PalMonitorInfo*);
PalResult wlEnumerateMonitorModes(PalMonitor*, int32_t*, PalMonitorMode*);
PalResult wlGetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlSetMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlValidateMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlSetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult wlCreateWindow(const PalWindowCreateInfo*, PalWindow**);
void wlDestroyWindow(PalWindow*);
PalResult wlMaximizeWindow(PalWindow*);
PalResult wlMinimizeWindow(PalWindow*);
PalResult wlRestoreWindow(PalWindow*);
PalResult wlShowWindow(PalWindow*);
PalResult wlHideWindow(PalWindow*);
PalResult wlFlashWindow(PalWindow*, const PalFlashInfo*);
PalResult wlGetWindowStyle(PalWindow*, PalWindowStyle*);
PalResult wlGetWindowMonitor(PalWindow*, PalMonitor**);
PalResult wlGetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
PalResult wlGetWindowPos(PalWindow*, int32_t*, int32_t*);
PalResult wlGetWindowSize(PalWindow*, uint32_t*, uint32_t*);
PalResult wlGetWindowState(PalWindow*, PalWindowState*);
PalBool wlIsWindowVisible(PalWindow*);
PalWindow* wlGetFocusWindow();
PalResult wlGetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
PalResult wlSetWindowOpacity(PalWindow*, float);
PalResult wlSetWindowStyle(PalWindow*, PalWindowStyle);
PalResult wlSetWindowTitle(PalWindow*, const char*);
PalResult wlSetWindowPos(PalWindow*, int32_t, int32_t);
PalResult wlSetWindowSize(PalWindow*, uint32_t, uint32_t);
PalResult wlSetFocusWindow(PalWindow*);

PalResult wlCreateIcon(const PalIconCreateInfo*, PalIcon**);
void wlDestroyIcon(PalIcon*);
PalResult wlSetWindowIcon(PalWindow*, PalIcon*);

PalResult wlCreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult wlCreateCursorFrom(PalCursorType, PalCursor**);
void wlDestroyCursor(PalCursor*);
void wlShowCursor(PalBool);
PalResult wlClipCursor(PalWindow*, PalBool);
PalResult wlGetCursorPos(PalWindow*, int32_t*, int32_t*);
PalResult wlSetCursorPos(PalWindow*, int32_t, int32_t);
PalResult wlSetWindowCursor(PalWindow*, PalCursor*);

PalResult wlAttachWindow(void*, PalWindow**);
PalResult wlDetachWindow(PalWindow*, void**);

static Backend s_wlBackend = {
    .shutdownVideo = wlShutdownVideo,
    .updateVideo = wlUpdateVideo,
    .setFBConfig = wlSetFBConfig,
    .enumerateMonitors = wlEnumerateMonitors,
    .getMonitorInfo = wlGetMonitorInfo,
    .getPrimaryMonitor = wlGetPrimaryMonitor,
    .enumerateMonitorModes = wlEnumerateMonitorModes,
    .getCurrentMonitorMode = wlGetCurrentMonitorMode,
    .setMonitorMode = wlSetMonitorMode,
    .validateMonitorMode = wlValidateMonitorMode,
    .setMonitorOrientation = wlSetMonitorOrientation,

    .createWindow = wlCreateWindow,
    .destroyWindow = wlDestroyWindow,
    .maximizeWindow = wlMaximizeWindow,
    .minimizeWindow = wlMinimizeWindow,
    .restoreWindow = wlRestoreWindow,
    .showWindow = wlShowWindow,
    .hideWindow = wlHideWindow,
    .flashWindow = wlFlashWindow,
    .getWindowStyle = wlGetWindowStyle,
    .getWindowMonitor = wlGetWindowMonitor,
    .getWindowTitle = wlGetWindowTitle,
    .getWindowPos = wlGetWindowPos,
    .getWindowSize = wlGetWindowSize,
    .getWindowState = wlGetWindowState,
    .isWindowVisible = wlIsWindowVisible,
    .getFocusWindow = wlGetFocusWindow,
    .getWindowHandleInfo = wlGetWindowHandleInfo,
    .setWindowOpacity = wlSetWindowOpacity,
    .setWindowStyle = wlSetWindowStyle,
    .setWindowTitle = wlSetWindowTitle,
    .setWindowPos = wlSetWindowPos,
    .setWindowSize = wlSetWindowSize,
    .setFocusWindow = wlSetFocusWindow,

    .createIcon = wlCreateIcon,
    .destroyIcon = wlDestroyIcon,
    .setWindowIcon = wlSetWindowIcon,

    .createCursor = wlCreateCursor,
    .createCursorFrom = wlCreateCursorFrom,
    .destroyCursor = wlDestroyCursor,
    .showCursor = wlShowCursor,
    .clipCursor = wlClipCursor,
    .getCursorPos = wlGetCursorPos,
    .setCursorPos = wlSetCursorPos,
    .setWindowCursor = wlSetWindowCursor,

    .attachWindow = wlAttachWindow,
    .detachWindow = wlDetachWindow};

#endif // PAL_HAS_WAYLAND_BACKEND

static int compareModes(
    const void* a,
    const void* b)
{
    const PalMonitorMode* mode1 = (const PalMonitorMode*)a;
    const PalMonitorMode* mode2 = (const PalMonitorMode*)b;

    // compare fields
    if (mode1->width != mode2->width) {
        return mode1->width - mode2->width;
    }

    if (mode1->height != mode2->height) {
        return mode1->height - mode2->height;
    }

    if (mode1->refreshRate != mode2->refreshRate) {
        return mode1->refreshRate - mode2->refreshRate;
    }

    if (mode1->bpp != mode2->bpp) {
        return mode1->bpp - mode2->bpp;
    }
}

static void createScancodeTable()
{
    // Letters
    s_Keyboard.scancodes[0x01E] = PAL_SCANCODE_A;
    s_Keyboard.scancodes[0x030] = PAL_SCANCODE_B;
    s_Keyboard.scancodes[0x02E] = PAL_SCANCODE_C;
    s_Keyboard.scancodes[0x020] = PAL_SCANCODE_D;
    s_Keyboard.scancodes[0x012] = PAL_SCANCODE_E;
    s_Keyboard.scancodes[0x021] = PAL_SCANCODE_F;
    s_Keyboard.scancodes[0x022] = PAL_SCANCODE_G;
    s_Keyboard.scancodes[0x023] = PAL_SCANCODE_H;
    s_Keyboard.scancodes[0x017] = PAL_SCANCODE_I;
    s_Keyboard.scancodes[0x024] = PAL_SCANCODE_J;
    s_Keyboard.scancodes[0x025] = PAL_SCANCODE_K;
    s_Keyboard.scancodes[0x026] = PAL_SCANCODE_L;
    s_Keyboard.scancodes[0x032] = PAL_SCANCODE_M;
    s_Keyboard.scancodes[0x031] = PAL_SCANCODE_N;
    s_Keyboard.scancodes[0x018] = PAL_SCANCODE_O;
    s_Keyboard.scancodes[0x019] = PAL_SCANCODE_P;
    s_Keyboard.scancodes[0x010] = PAL_SCANCODE_Q;
    s_Keyboard.scancodes[0x013] = PAL_SCANCODE_R;
    s_Keyboard.scancodes[0x01F] = PAL_SCANCODE_S;
    s_Keyboard.scancodes[0x014] = PAL_SCANCODE_T;
    s_Keyboard.scancodes[0x016] = PAL_SCANCODE_U;
    s_Keyboard.scancodes[0x02F] = PAL_SCANCODE_V;
    s_Keyboard.scancodes[0x011] = PAL_SCANCODE_W;
    s_Keyboard.scancodes[0x02D] = PAL_SCANCODE_X;
    s_Keyboard.scancodes[0x015] = PAL_SCANCODE_Y;
    s_Keyboard.scancodes[0x02C] = PAL_SCANCODE_Z;

    // Numbers (top row)
    s_Keyboard.scancodes[0x00B] = PAL_SCANCODE_0;
    s_Keyboard.scancodes[0x002] = PAL_SCANCODE_1;
    s_Keyboard.scancodes[0x003] = PAL_SCANCODE_2;
    s_Keyboard.scancodes[0x004] = PAL_SCANCODE_3;
    s_Keyboard.scancodes[0x005] = PAL_SCANCODE_4;
    s_Keyboard.scancodes[0x006] = PAL_SCANCODE_5;
    s_Keyboard.scancodes[0x007] = PAL_SCANCODE_6;
    s_Keyboard.scancodes[0x008] = PAL_SCANCODE_7;
    s_Keyboard.scancodes[0x009] = PAL_SCANCODE_8;
    s_Keyboard.scancodes[0x00A] = PAL_SCANCODE_9;

    // Function
    s_Keyboard.scancodes[0x03B] = PAL_SCANCODE_F1;
    s_Keyboard.scancodes[0x03C] = PAL_SCANCODE_F2;
    s_Keyboard.scancodes[0x03D] = PAL_SCANCODE_F3;
    s_Keyboard.scancodes[0x03E] = PAL_SCANCODE_F4;
    s_Keyboard.scancodes[0x03F] = PAL_SCANCODE_F5;
    s_Keyboard.scancodes[0x040] = PAL_SCANCODE_F6;
    s_Keyboard.scancodes[0x041] = PAL_SCANCODE_F7;
    s_Keyboard.scancodes[0x042] = PAL_SCANCODE_F8;
    s_Keyboard.scancodes[0x043] = PAL_SCANCODE_F9;
    s_Keyboard.scancodes[0x044] = PAL_SCANCODE_F10;
    s_Keyboard.scancodes[0x057] = PAL_SCANCODE_F11;
    s_Keyboard.scancodes[0x058] = PAL_SCANCODE_F12;

    // Control
    s_Keyboard.scancodes[0x001] = PAL_SCANCODE_ESCAPE;
    s_Keyboard.scancodes[0x01C] = PAL_SCANCODE_ENTER;
    s_Keyboard.scancodes[0x00F] = PAL_SCANCODE_TAB;
    s_Keyboard.scancodes[0x00E] = PAL_SCANCODE_BACKSPACE;
    s_Keyboard.scancodes[0x039] = PAL_SCANCODE_SPACE;
    s_Keyboard.scancodes[0x03A] = PAL_SCANCODE_CAPSLOCK;
    s_Keyboard.scancodes[0x045] = PAL_SCANCODE_NUMLOCK;
    s_Keyboard.scancodes[0x046] = PAL_SCANCODE_SCROLLLOCK;
    s_Keyboard.scancodes[0x02A] = PAL_SCANCODE_LSHIFT;
    s_Keyboard.scancodes[0x036] = PAL_SCANCODE_RSHIFT;
    s_Keyboard.scancodes[0x01D] = PAL_SCANCODE_LCTRL;
    s_Keyboard.scancodes[0x061] = PAL_SCANCODE_RCTRL;
    s_Keyboard.scancodes[0x038] = PAL_SCANCODE_LALT;
    s_Keyboard.scancodes[0x064] = PAL_SCANCODE_RALT;

    // Arrows
    s_Keyboard.scancodes[0x069] = PAL_SCANCODE_LEFT;
    s_Keyboard.scancodes[0x06A] = PAL_SCANCODE_RIGHT;
    s_Keyboard.scancodes[0x067] = PAL_SCANCODE_UP;
    s_Keyboard.scancodes[0x06C] = PAL_SCANCODE_DOWN;

    // Navigation
    s_Keyboard.scancodes[0x06E] = PAL_SCANCODE_INSERT;
    s_Keyboard.scancodes[0x06F] = PAL_SCANCODE_DELETE;
    s_Keyboard.scancodes[0x066] = PAL_SCANCODE_HOME;
    s_Keyboard.scancodes[0x067] = PAL_SCANCODE_END;
    s_Keyboard.scancodes[0x068] = PAL_SCANCODE_PAGEUP;
    s_Keyboard.scancodes[0x06D] = PAL_SCANCODE_PAGEDOWN;

    // Keypad
    s_Keyboard.scancodes[0x052] = PAL_SCANCODE_KP_0;
    s_Keyboard.scancodes[0x04F] = PAL_SCANCODE_KP_1;
    s_Keyboard.scancodes[0x050] = PAL_SCANCODE_KP_2;
    s_Keyboard.scancodes[0x051] = PAL_SCANCODE_KP_3;
    s_Keyboard.scancodes[0x04B] = PAL_SCANCODE_KP_4;
    s_Keyboard.scancodes[0x04C] = PAL_SCANCODE_KP_5;
    s_Keyboard.scancodes[0x04D] = PAL_SCANCODE_KP_6;
    s_Keyboard.scancodes[0x047] = PAL_SCANCODE_KP_7;
    s_Keyboard.scancodes[0x048] = PAL_SCANCODE_KP_8;
    s_Keyboard.scancodes[0x049] = PAL_SCANCODE_KP_9;
    s_Keyboard.scancodes[0x060] = PAL_SCANCODE_KP_ENTER;
    s_Keyboard.scancodes[0x04E] = PAL_SCANCODE_KP_ADD;
    s_Keyboard.scancodes[0x04A] = PAL_SCANCODE_KP_SUBTRACT;
    s_Keyboard.scancodes[0x037] = PAL_SCANCODE_KP_MULTIPLY;
    s_Keyboard.scancodes[0x062] = PAL_SCANCODE_KP_DIVIDE;
    s_Keyboard.scancodes[0x053] = PAL_SCANCODE_KP_DECIMAL;

    // Misc
    s_Keyboard.scancodes[0x063] = PAL_SCANCODE_PRINTSCREEN;
    s_Keyboard.scancodes[0x066] = PAL_SCANCODE_PAUSE;
    s_Keyboard.scancodes[0x07F] = PAL_SCANCODE_MENU;
    s_Keyboard.scancodes[0x028] = PAL_SCANCODE_APOSTROPHE;
    s_Keyboard.scancodes[0x02B] = PAL_SCANCODE_BACKSLASH;
    s_Keyboard.scancodes[0x033] = PAL_SCANCODE_COMMA;
    s_Keyboard.scancodes[0x00D] = PAL_SCANCODE_EQUAL;
    s_Keyboard.scancodes[0x029] = PAL_SCANCODE_GRAVEACCENT;
    s_Keyboard.scancodes[0x00C] = PAL_SCANCODE_SUBTRACT;
    s_Keyboard.scancodes[0x034] = PAL_SCANCODE_PERIOD;
    s_Keyboard.scancodes[0x027] = PAL_SCANCODE_SEMICOLON;
    s_Keyboard.scancodes[0x035] = PAL_SCANCODE_SLASH;
    s_Keyboard.scancodes[0x01A] = PAL_SCANCODE_LBRACKET;
    s_Keyboard.scancodes[0x01B] = PAL_SCANCODE_RBRACKET;
    s_Keyboard.scancodes[0x07D] = PAL_SCANCODE_LSUPER;
    s_Keyboard.scancodes[0x07E] = PAL_SCANCODE_RSUPER;
}

PalResult PAL_CALL palInitVideo(
    const PalAllocator* allocator,
    PalEventDriver* eventDriver)
{
    if (s_Video.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // get backend type
    PalBool x11 = PAL_TRUE;
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            x11 = PAL_FALSE;
        }
    }

    s_Video.maxMonitorData = 16; // initial size
    s_Video.maxWindowData = 32;  // initial size
    uint32_t windowDataSize = sizeof(WindowData) * s_Video.maxWindowData;
    uint32_t monitorDataSize = sizeof(MonitorData) * s_Video.maxMonitorData;

    s_Video.windowData = palAllocate(s_Video.allocator, windowDataSize, 0);
    s_Video.monitorData = palAllocate(s_Video.allocator, monitorDataSize, 0);
    if (!s_Video.monitorData || !s_Video.windowData) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    s_Video.className = "PAL";
    if (x11) {
#if PAL_HAS_X11_BACKEND == 1
        PalResult ret = xInitVideo();
        if (ret != PAL_RESULT_SUCCESS) {
            return ret;
        }
        s_Video.backend = &s_XBackend;
#else
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
#endif // PAL_HAS_X11_BACKEND

    } else {
#if PAL_HAS_WAYLAND_BACKEND == 1
        PalResult ret = wlInitVideo();
        if (ret != PAL_RESULT_SUCCESS) {
            return ret;
        }
        s_Video.backend = &s_wlBackend;
#else
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
#endif // PAL_HAS_WAYLAND_BACKEND
    }

    createScancodeTable();

    // we load EGL as well
    s_Egl.handle = dlopen("libEGL.so", RTLD_LAZY);
    if (s_Egl.handle) {
        eglGetProcAddressFn load = nullptr;
        load = (eglGetProcAddressFn)dlsym(s_Egl.handle, "eglGetProcAddress");

        s_Egl.eglInitialize = (eglInitializeFn)load("eglInitialize");
        s_Egl.eglTerminate = (eglTerminateFn)load("eglTerminate");
        s_Egl.eglGetDisplay = (eglGetDisplayFn)load("eglGetDisplay");
        s_Egl.eglChooseConfig = (eglChooseConfigFn)load("eglChooseConfig");
        s_Egl.eglGetError = (eglGetErrorFn)load("eglGetError");
        s_Egl.eglBindAPI = (eglBindAPIFn)load("eglBindAPI");
        s_Egl.eglGetConfigs = (eglGetConfigsFn)load("eglGetConfigs");
        s_Egl.eglGetConfigAttrib = (eglGetConfigAttribFn)load("eglGetConfigAttrib");
    }

    s_Video.allocator = allocator;
    s_Video.eventDriver = eventDriver;
    s_Video.initialized = PAL_TRUE;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownVideo()
{
    if (s_Video.initialized) {
        s_Video.backend->shutdownVideo();
        palFree(s_Video.allocator, s_Video.windowData);
        palFree(s_Video.allocator, s_Video.monitorData);

        if (s_Egl.handle) {
            dlclose(s_Egl.handle);
        }

        s_Video.platformInstance = nullptr;
        s_Video.display = nullptr;
        memset(&s_Keyboard, 0, sizeof(Keyboard));
        memset(&s_Mouse, 0, sizeof(Mouse));
        s_Video.initialized = PAL_FALSE;
    }
}

void PAL_CALL palUpdateVideo()
{
    if (s_Video.initialized) {
        s_Video.backend->updateVideo();
    }
}

PalVideoFeatures PAL_CALL palGetVideoFeatures()
{
    if (!s_Video.initialized) {
        return 0;
    }

    return s_Video.features;
}

PalResult PAL_CALL palSetFBConfig(
    const int index,
    PalFBConfigBackend backend)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // X11 and wayland can only used GLX and EGL
    if (backend == PAL_CONFIG_BACKEND_WGL) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setFBConfig(index, backend);
}

PalResult PAL_CALL palEnumerateMonitors(
    int32_t* count,
    PalMonitor** outMonitors)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!count || *count == 0 && outMonitors) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->enumerateMonitors(count, outMonitors);
}

PalResult PAL_CALL palGetPrimaryMonitor(PalMonitor** outMonitor)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!outMonitor) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getPrimaryMonitor(outMonitor);
}

PalResult PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!info) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getMonitorInfo(monitor, info);
}

PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    int32_t* count,
    PalMonitorMode* modes)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!monitor || !count || *count == 0 && modes) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    PalResult ret = s_Video.backend->enumerateMonitorModes(monitor, count, modes);
    if (ret == PAL_RESULT_SUCCESS && modes) {
        // sort the modes so that they are lowest to highest
        qsort(modes, *count, sizeof(PalMonitorMode), compareModes);
    }

    return ret;
}

PalResult PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!monitor || !mode) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getCurrentMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!monitor || !mode) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setMonitorMode(monitor, mode);
}

PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!monitor || !mode) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->validateMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!monitor) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setMonitorOrientation(monitor, orientation);
}

PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!info || !outWindow) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (info->style & PAL_WINDOW_STYLE_NO_MINIMIZEBOX) {
        return palMakeResult(
            PAL_RESULT_FEATURE_NOT_SUPPORTED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (info->style & PAL_WINDOW_STYLE_NO_MAXIMIZEBOX) {
        return palMakeResult(
            PAL_RESULT_FEATURE_NOT_SUPPORTED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->createWindow(info, outWindow);
}

void PAL_CALL palDestroyWindow(PalWindow* window)
{
    if (s_Video.initialized && window) {
        return s_Video.backend->destroyWindow(window);
    }
}

PalResult PAL_CALL palMinimizeWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->maximizeWindow(window);
}

PalResult PAL_CALL palMaximizeWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->minimizeWindow(window);
}

PalResult PAL_CALL palRestoreWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->restoreWindow(window);
}

PalResult PAL_CALL palShowWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->showWindow(window);
}

PalResult PAL_CALL palHideWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->hideWindow(window);
}

PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window || !info) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->flashWindow(window, info);
}

PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window || !outStyle) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getWindowStyle(window, outStyle);
}

PalResult PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window || !outMonitor) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getWindowMonitor(window, outMonitor);
}

PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window || !outBuffer) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getWindowTitle(window, bufferSize, outSize, outBuffer);
}

PalResult PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getWindowPos(window, x, y);
}

PalResult PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getWindowSize(window, width, height);
}

PalResult PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window || !outState) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getWindowState(window, outState);
}

const PalBool* PAL_CALL palGetKeycodeState()
{
    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Keyboard.keycodeState;
}

const PalBool* PAL_CALL palGetScancodeState()
{
    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Keyboard.scancodeState;
}

const PalBool* PAL_CALL palGetMouseState()
{
    if (!s_Video.initialized) {
        return nullptr;
    }
    return s_Mouse.state;
}

void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy)
{
    if (!s_Video.initialized) {
        return;
    }

    if (dx) {
        *dx = s_Mouse.dx;
    }

    if (dy) {
        *dy = s_Mouse.dy;
    }
}

void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy)
{
    if (!s_Video.initialized) {
        return;
    }

    if (dx) {
        *dx = (float)s_Mouse.tmpScrollX;
    }

    if (dy) {
        *dy = (float)s_Mouse.tmpScrollY;
    }
}

PalBool PAL_CALL palIsWindowVisible(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->isWindowVisible(window);
}

PalWindow* PAL_CALL palGetFocusWindow()
{
    if (!s_Video.initialized) {
        return nullptr;
    }

    return s_Video.backend->getFocusWindow();
}

PalResult PAL_CALL palGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    if (s_Video.initialized) {
        return s_Video.backend->getWindowHandleInfo(window, info);
    }

    return palMakeResult(
        PAL_RESULT_NOT_INITIALIZED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!(s_Video.features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW)) {
        return palMakeResult(
            PAL_RESULT_FEATURE_NOT_SUPPORTED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (opacity < 0.0f) {
        opacity = 0.0f;
    }

    if (opacity > 1.0f) {
        opacity = 1.0f;
    }

    return s_Video.backend->setWindowOpacity(window, opacity);
}

PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setWindowStyle(window, style);
}

PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window || !title) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setWindowTitle(window, title);
}

PalResult PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setWindowPos(window, x, y);
}

PalResult PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setWindowSize(window, width, height);
}

PalResult PAL_CALL palSetFocusWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setFocusWindow(window);
}

PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!info || !outIcon) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->createIcon(info, outIcon);
}

void PAL_CALL palDestroyIcon(PalIcon* icon)
{
    if (s_Video.initialized && icon) {
        s_Video.backend->destroyIcon(icon);
    }
}

PalResult PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setWindowIcon(window, icon);
}

PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!info || !outCursor) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->createCursor(info, outCursor);
}

PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!outCursor) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->createCursorFrom(type, outCursor);
}

void PAL_CALL palDestroyCursor(PalCursor* cursor)
{
    if (s_Video.initialized && cursor) {
        s_Video.backend->destroyCursor(cursor);
    }
}

void PAL_CALL palShowCursor(PalBool show)
{
    if (s_Video.initialized) {
        s_Video.backend->showCursor(show);
    }
}

PalResult PAL_CALL palClipCursor(
    PalWindow* window,
    PalBool clip)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->clipCursor(window, clip);
}

PalResult PAL_CALL palGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->getCursorPos(window, x, y);
}

PalResult PAL_CALL palSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setCursorPos(window, x, y);
}

PalResult PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->setWindowCursor(window, cursor);
}

void* PAL_CALL palGetInstance()
{
    if (!s_Video.initialized) {
        return nullptr;
    }

    return s_Video.display;
}

PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!windowHandle) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->attachWindow(windowHandle, outWindow);
}

PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    return s_Video.backend->detachWindow(window, outWindowHandle);
}

void PAL_CALL palSetPreferredInstance(void* instance)
{
    if (!s_Video.initialized && instance) {
        s_Video.platformInstance = instance;
    }
}

#endif // __linux__
