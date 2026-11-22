/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#if PAL_HAS_X11_BACKEND

#if PAL_HAS_EVENT_MODULE
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"
#endif // PAL_HAS_EVENT_MODULE

#include "x11_helper.h"
#include "pal2/video/window.h"
#include "video/video_egl.h"
#include "video_glx.h"

#include <X11/Xatom.h>
#include <math.h>
#include <unistd.h>

/* =========================================================================
 * Helper
 * ========================================================================= */

static XVisualInfo* glxBackend(const int fbConfigIndex)
{
    if (fbConfigIndex <= 0) {
        return nullptr;
    }

    int count = 0;
    GLXFBConfig* configs = g_VideoGlx.getFBConfigs(g_X11.display, g_X11.screen, &count);
    if (!configs || fbConfigIndex > count) {
        if (configs) {
            g_X11.free(configs);
        }
        return nullptr;
    }

    GLXFBConfig fbConfig = configs[fbConfigIndex - 1];
    g_X11.free(configs);

    if (!fbConfig) {
        return nullptr;
    }

    return g_VideoGlx.getVisualFromFBConfig(g_X11.display, fbConfig);
}

static XVisualInfo* eglXBackend(int fbConfigIndex)
{
    if (fbConfigIndex <= 0) {
        return nullptr;
    }

    EGLDisplay display = g_VideoEgl.getDisplay((EGLNativeDisplayType)g_X11.display);
    if (display == EGL_NO_DISPLAY) {
        return nullptr;
    }

    EGLint numConfigs = 0;
    if (!g_VideoEgl.getConfigs(display, nullptr, 0, &numConfigs) || fbConfigIndex > numConfigs) {
        return nullptr;
    }

    uint64_t configSize = sizeof(EGLConfig) * (uint64_t)numConfigs;
    EGLConfig* eglConfigs = palAllocate(g_X11.allocator, configSize, 0);
    if (!eglConfigs) {
        return nullptr;
    }

    g_VideoEgl.getConfigs(display, eglConfigs, numConfigs, &numConfigs);
    EGLConfig config = eglConfigs[fbConfigIndex - 1];
    palFree(g_X11.allocator, eglConfigs);

    EGLint visualID = 0;
    g_VideoEgl.getConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualID);
    if (visualID == 0) {
        return nullptr;
    }

    int numVisuals = 0;
    XVisualInfo tmp = {0};
    tmp.visualid = (VisualID)visualID;

    return g_X11.getVisualInfo(g_X11.display, VisualIDMask, &tmp, &numVisuals);
}


/* =========================================================================
 * API
 * ========================================================================= */

void x11GetWindowState(
    PalWindow* window, 
    PalWindowState* state);

void x11GetPrimaryMonitor(PalMonitor** monitor);

void x11GetMonitorInfo(
    PalMonitor*,
    PalMonitorInfo*);

PalResult x11CreateWindow(
    const PalWindowCreateInfo* info, 
    PalWindow** window)
{
    Window _window = None;
    PalMonitor* monitor = nullptr;
    PalMonitorInfo monitorInfo;

    WindowData* data = x11GetFreeWindowData();
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    Visual* visual = nullptr;
    int depth = 0;
    Colormap colormap = None;
    unsigned long bgPixel = 0;
    unsigned long borderPixel = 0;

    // user provided a config id
    if (info->fbConfigIndex) {
        PalFBConfigBackend backend = info->fbConfigBackend;
        XVisualInfo* visualInfo = nullptr;

        if (backend == PAL_FBCONFIG_BACKEND_PAL_OPENGL) {
            backend = PAL_FBCONFIG_BACKEND_EGL;
        }

        if (backend == PAL_FBCONFIG_BACKEND_EGL) {
            visualInfo = eglXBackend(info->fbConfigIndex);

        } else if (backend == PAL_FBCONFIG_BACKEND_GLX) {
            visualInfo = glxBackend(info->fbConfigIndex);

        } else {
            data->used = PAL_FALSE;
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }

        if (!visualInfo) {
            data->used = PAL_FALSE;
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }

        visual = visualInfo->visual;
        depth = visualInfo->depth;
        bgPixel = 0;
        borderPixel = 0;
        colormap = g_X11.createColormap(g_X11.display, g_X11.root, visual, AllocNone);
        g_X11.free(visualInfo);

        if (!colormap) {
            data->used = PAL_FALSE;
            return PAL_RESULT_CODE_PLATFORM_FAILURE;
        }

        data->colormap = colormap;

    } else {
        // use a default visual
        visual = DefaultVisual(g_X11.display, g_X11.screen);
        depth = DefaultDepth(g_X11.display, g_X11.screen);
        bgPixel = WhitePixel(g_X11.display, g_X11.screen);
        borderPixel = BlackPixel(g_X11.display, g_X11.screen);
        colormap = DefaultColormap(g_X11.display, g_X11.screen);
        data->colormap = None;
    }

    // get monitor
    int monitorX = 0;
    int monitorY = 0;
    uint32_t monitorW = 0;
    uint32_t monitorH = 0;
    uint32_t dpi = 0;

    if (info->monitor) {
        monitor = info->monitor;

    } else {
        // get primary monitor
        x11GetPrimaryMonitor(&monitor);
    }

    if (monitor) {
        // get monitor info
        x11GetMonitorInfo(monitor, &monitorInfo);
        monitorX = monitorInfo.x;
        monitorY = monitorInfo.y;
        monitorW = monitorInfo.width;
        monitorH = monitorInfo.height;
        dpi = monitorInfo.dpi;

    } else {
        // primary monitor is not set
        XRRScreenResources* resources = nullptr;
        resources = g_X11.getScreenResources(g_X11.display, g_X11.root);
        for (int i = 0; i < resources->noutput; ++i) {
            RROutput output = resources->outputs[i];
            XRROutputInfo* outputInfo = g_X11.getOutputInfo(g_X11.display, resources, output);

            // check if its a monitor
            if (outputInfo->connection != RR_Connected || outputInfo->crtc == None) {
                g_X11.freeOutputInfo(outputInfo);
                continue;
            }

            XRRCrtcInfo* crtc = g_X11.getCrtcInfo(g_X11.display, resources, outputInfo->crtc);
            monitorX = crtc->x;
            monitorY = crtc->y;
            monitorW = crtc->width;
            monitorH = crtc->height;

            // get DPI
            float raw = (float)crtc->width / 1920.0f;
            float steps[] = {1.0f, 1.2f, 1.5f, 1.75, 2.0f};
            float closest = steps[0];
            float minDiff = fabsf(raw - steps[0]);

            for (uint32_t i = 1; i < sizeof(steps) / sizeof(steps[0]); i++) {
                float diff = fabsf(raw - steps[i]);
                if (diff < minDiff) {
                    minDiff = diff;
                    closest = steps[i];
                }
            }

            dpi = (uint32_t)(closest * 96.0f);
            g_X11.freeCrtcInfo(crtc);
            g_X11.freeOutputInfo(outputInfo);
            break;
        }
        g_X11.freeScreenResources(resources);
    }

    int32_t x, y = 0;
    // the position and size must be scaled with the dpi before this call
    if (info->center) {
        x = monitorX + (int)(monitorW - info->width) / 2;
        y = monitorY + (int)(monitorH - info->height) / 2;

    } else {
        // we set 100 for each axix
        x = monitorX + 100;
        y = monitorY + 100;
    }

    // check and set transparency
    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        if (!(g_X11Driver.features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW)) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
        // we dont need to set any flag
    }

    long mask = StructureNotifyMask | KeyPressMask;
    mask |= KeyReleaseMask;
    mask |= ButtonPressMask;
    mask |= ButtonReleaseMask;
    mask |= PointerMotionMask;
    mask |= FocusChangeMask;
    mask |= PropertyChangeMask;

    XSetWindowAttributes attrs = {0};
    attrs.colormap = colormap;
    attrs.event_mask = mask;
    attrs.background_pixel = bgPixel;
    attrs.border_pixel = borderPixel;
    attrs.override_redirect = False;

    // create window
    _window = g_X11.createWindow(
        g_X11.display,
        g_X11.root,
        x,
        y,
        info->width,
        info->height,
        0, // border width
        depth,
        InputOutput, // class
        visual,
        CWEventMask | CWColormap | CWBorderPixel | CWBackPixel,
        &attrs);

    if (_window == None) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    // set pid property
    pid_t pid = getpid();
    g_X11.changeProperty(
        g_X11.display,
        _window,
        g_X11Atoms._NET_WM_PID,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)&pid,
        1);

    // set class property
    XClassHint* hints = g_X11.allocClassHint();
    if (hints) {
        const char* resName = info->instanceName;
        const char* resClass = info->appName;

        if (!resName || strlen(resName) == 0) {
            resName = info->title;
        }

        if (!resClass || strlen(resClass) == 0) {
            resClass = "PAL";
        }

        hints->res_name = (char*)resName;
        hints->res_class = (char*)resClass;
        g_X11.setClassHint(g_X11.display, _window, hints);
        g_X11.free(hints);
    }

    if (g_X11Atoms.unicodeTitle && info->title) {
        g_X11.changeProperty(
            g_X11.display,
            _window,
            g_X11Atoms._NET_WM_NAME,
            g_X11Atoms.UTF8_STRING,
            8, // unsigned char
            PropModeReplace,
            (const unsigned char*)info->title,
            (int)strlen(info->title));

    } else {
        if (info->title) {
            g_X11.storeName(g_X11.display, _window, info->title);
        }
    }

    // borderless
    if (info->style & PAL_WINDOW_STYLE_BORDERLESS) {
        if (!(g_X11Driver.features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW)) {
            g_X11.destroyWindow(g_X11.display, _window);
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

        g_X11.changeProperty(
            g_X11.display,
            _window,
            g_X11Atoms._NET_WM_WINDOW_TYPE,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char*)&g_X11Atoms._NET_WM_WINDOW_TYPE_SPLASH,
            1);
    }

    // tool window
    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        if (!(g_X11Driver.features & PAL_VIDEO_FEATURE_TOOL_WINDOW)) {
            g_X11.destroyWindow(g_X11.display, _window);
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

        g_X11.changeProperty(
            g_X11.display,
            _window,
            g_X11Atoms._NET_WM_WINDOW_TYPE,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char*)&g_X11Atoms._NET_WM_WINDOW_TYPE_UTILITY,
            1);
    }

    // topmost
    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        if (g_X11Atoms._NET_WM_STATE_ABOVE) {
            g_X11.changeProperty(
                g_X11.display,
                _window,
                g_X11Atoms._NET_WM_STATE,
                XA_ATOM,
                32,
                PropModeAppend,
                (unsigned char*)&g_X11Atoms._NET_WM_STATE_ABOVE,
                1);
        }
    }

    // set size hints
    XSizeHints wmHints = {0};
    wmHints.flags = PPosition | PSize;
    wmHints.x = x;
    wmHints.y = y;
    wmHints.width = (int)info->width;
    wmHints.height = (int)info->height;

    // resizable
    if (!(info->style & PAL_WINDOW_STYLE_RESIZABLE)) {
        wmHints.flags |= PMinSize;
        wmHints.flags |= PMaxSize;
        wmHints.min_width = wmHints.max_width = (int)info->width;
        wmHints.min_height = wmHints.max_height = (int)info->height;
    }

    // show window
    g_X11.setWMNormalHints(g_X11.display, _window, &wmHints);
    if (info->show) {
        g_X11.mapWindow(g_X11.display, _window);
        g_X11.flush(g_X11.display);
    }

    // check if the window has been mapped
    // we use this to minimize or maximize
    XWindowAttributes attr;
    g_X11.getWindowAttributes(g_X11.display, _window, &attr);
    PalBool windowMapped = (attr.map_state == IsViewable);

    // maximize
    if (info->state == PAL_WINDOW_STATE_MAXIMIZED) {
        if (!(g_X11Driver.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
            g_X11.destroyWindow(g_X11.display, _window);
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

        // if the window is not mapped, we wait till its mapped
        if (!windowMapped) {
            // wait till the window is mapped
            for (;;) {
                XEvent event;
                g_X11.nextEvent(g_X11.display, &event);
                if (event.type == MapNotify && event.xmap.window == _window) {
                    windowMapped = PAL_TRUE;
                    break;
                }
            }
        }

        x11SendWMEvent(
            _window,
            g_X11Atoms._NET_WM_STATE,
            (long)g_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
            (long)g_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
            1,
            0,
            PAL_TRUE); // _NET_WM_STATE_ADD
    }

    // minimize
    if (info->state == PAL_WINDOW_STATE_MINIMIZED) {
        if (!(g_X11Driver.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
            g_X11.destroyWindow(g_X11.display, _window);
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

        // if the window is not mapped, we wait till its mapped
        if (!windowMapped) {
            // wait till the window is mapped
            for (;;) {
                XEvent event;
                g_X11.nextEvent(g_X11.display, &event);
                if (event.type == MapNotify && event.xmap.window == _window) {
                    windowMapped = PAL_TRUE;
                    break;
                }
            }
        }
        g_X11.iconifyWindow(g_X11.display, _window, g_X11.screen);
    }

    g_X11.setWMProtocols(g_X11.display, _window, &g_X11Atoms.WM_DELETE_WINDOW, 1);
    g_X11.flush(g_X11.display);

    // attach the window data to the window
    data->skipConfigure = PAL_TRUE;
    data->skipState = PAL_TRUE;
    data->isAttached = PAL_FALSE; // PAL_TRUE for attached windows
    data->dpi = dpi;              // the current window monitor
    data->window = TO_PAL_HANDLE(PalWindow, _window);
    g_X11.saveContext(g_X11.display, _window, g_X11.dataID, (XPointer)data);

    // create an input context
    data->ic = g_X11.createIC(
        g_X11.im,
        XNInputStyle,
        XIMPreeditNothing | XIMStatusNothing,
        XNClientWindow,
        _window,
        XNFocusWindow,
        _window,
        nullptr);

    if (!data->ic) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    *window = TO_PAL_HANDLE(PalWindow, _window);
    return PAL_RESULT_SUCCESS;
}

void x11DestroyWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    WindowData* data = nullptr;
    g_X11.findContext(g_X11.display, xWin, g_X11.dataID, (XPointer*)&data);

    // PAL does not destroy an attached window
    if (data->isAttached) {
        return;
    }

    g_X11.destroyIC(data->ic);
    g_X11.destroyWindow(g_X11.display, xWin);

    if (data->colormap != None) {
        g_X11.freeColormap(g_X11.display, data->colormap);
    }

    data->used = PAL_FALSE;
}

void x11MaximizeWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);

    x11SendWMEvent(
        xWin,
        g_X11Atoms._NET_WM_STATE,
        (long)g_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
        (long)g_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
        1,
        0,
        PAL_TRUE); // _NET_WM_STATE_ADD
}

void x11MinimizeWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    g_X11.iconifyWindow(g_X11.display, xWin, g_X11.screen);
}

void x11RestoreWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);

    // since we have no fixed way to restore the window
    // we just restore from minimized and maximized state
    x11SendWMEvent(
        xWin,
        g_X11Atoms._NET_WM_STATE,
        (long)g_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
        (long)g_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
        1,
        0,
        PAL_FALSE); // _NET_WM_STATE_REMOVE

    g_X11.mapRaised(g_X11.display, xWin);
}

void x11ShowWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    g_X11.mapWindow(g_X11.display, xWin);
}

void x11HideWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    g_X11.unmapWindow(g_X11.display, xWin);
}

void x11FlashWindow(
    PalWindow* window, 
    const PalFlashInfo* info)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    PalBool add = (info->flags & PAL_FLASH_FLAG_TRAY) ? PAL_TRUE : PAL_FALSE;

    if (g_X11Atoms._NET_WM_STATE_DEMANDS_ATTENTION) {
        x11SendWMEvent(
            xWin,
            g_X11Atoms._NET_WM_STATE,
            (long)g_X11Atoms._NET_WM_STATE_DEMANDS_ATTENTION,
            0,
            0,
            0,
            add);

    } else {
        XWMHints* hints = g_X11.getWMHints(g_X11.display, xWin);
        if (!hints) {
            hints = g_X11.allocWMHints();
            if (!hints) {
                return;
            }
        }

        if (add) {
            hints->flags |= XUrgencyHint;
        } else {
            hints->flags &= ~XUrgencyHint;
        }
        g_X11.setWMHints(g_X11.display, xWin, hints);
        g_X11.free(hints);
    }

    g_X11.flush(g_X11.display);
}

PalResult x11AttachWindow(
    void* windowHandle,
    PalWindow** window)
{
    Window xWin = FROM_PAL_HANDLE(Window, windowHandle);
    XWindowAttributes attr;
    if (!g_X11.getWindowAttributes(g_X11.display, xWin, &attr)) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // get a free slot and set the window handle to it
    // we also set a flag to make sure we know this is an attached window
    WindowData* data = x11GetFreeWindowData();
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    PalWindow* _window = TO_PAL_HANDLE(PalWindow, xWin);
    // we assume the window was just created, since there is
    // no official way to get the DPI
    data->isAttached = PAL_TRUE;
    data->dpi = 96; // if this is not the DPI, a dpi event will be triggered

    // If the window manager has not mapped the window yet,
    // we dont need the initial Size / Move events
    data->skipConfigure = PAL_TRUE;
    data->skipState = PAL_TRUE;
    data->skipIfAttached = PAL_TRUE;
    data->window = _window;
    data->w = (uint32_t)attr.width;
    data->h = (uint32_t)attr.height;
    data->x = attr.x;
    data->y = attr.y;

    // get the current window state
    // we dont check the return code because we know the window is valid
    x11GetWindowState(_window, &data->state);

    // listen to the events we support
    long mask = StructureNotifyMask | KeyPressMask;
    mask |= KeyReleaseMask;
    mask |= ButtonPressMask;
    mask |= ButtonReleaseMask;
    mask |= PointerMotionMask;
    mask |= FocusChangeMask;
    mask |= PropertyChangeMask;
    g_X11.selectInput(g_X11.display, xWin, mask);

    // listen to window close event
    g_X11.setWMProtocols(g_X11.display, xWin, &g_X11Atoms.WM_DELETE_WINDOW, 1);

    g_X11.saveContext(g_X11.display, xWin, g_X11.dataID, (XPointer)data);
    g_X11.flush(g_X11.display);

    *window = _window;
    return PAL_RESULT_SUCCESS;
}

PalResult x11DetachWindow(
    PalWindow* window,
    void** windowHandle)
{
    // we check is the window is really detachable
    Window xWin = FROM_PAL_HANDLE(Window, window);
    WindowData* data = nullptr;
    g_X11.findContext(g_X11.display, xWin, g_X11.dataID, (XPointer*)&data);
    if (!data) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    if (data->isAttached == PAL_FALSE) {
        // window was created by PAL
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // detach the window
    data->used = PAL_FALSE;
    long mask = 0;
    g_X11.selectInput(g_X11.display, xWin, mask);
    g_X11.setWMProtocols(g_X11.display, xWin, nullptr, 0);

    if (windowHandle) {
        *windowHandle = (void*)window;
    }

    return PAL_RESULT_SUCCESS;
}
    
void x11GetWindowStyle(
    PalWindow* window, 
    PalWindowStyle* style)
{
    (void)window;
    (void)style;
}

void x11GetWindowMonitor(
    PalWindow* window, 
    PalMonitor** monitor)
{
    (void)window;
    (void)monitor;
}

void x11GetWindowTitle(
    PalWindow* window, 
    size_t bufferSize, 
    size_t* size, 
    char* buffer)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);

    if (g_X11Atoms.unicodeTitle) {
        Atom type;
        int format;
        unsigned long count = 0;
        unsigned long bytesAfter = 0;
        unsigned char* prop = nullptr;

        g_X11.getWindowProperty(
            g_X11.display,
            xWin,
            g_X11Atoms._NET_WM_NAME,
            0,
            (~0L),
            False,
            g_X11Atoms.UTF8_STRING,
            &type,
            &format,
            &count,
            &bytesAfter,
            &prop);

        if (size) {
            *size = prop ? (uint64_t)(count + 1) : 0;
        }

        if (prop && buffer && bufferSize > 0) {
            uint64_t copyLen = (count < bufferSize - 1) ? count : (bufferSize - 1);
            memcpy(buffer, prop, (size_t)copyLen);
            buffer[copyLen] = '\0';
        }

        if (prop) {
            g_X11.free(prop);
        }

    } else {
        XTextProperty text;
        if (!g_X11.getWMName(g_X11.display, xWin, &text) || !text.value) {
            if (size) {
                *size = 0;
            }
            return;
        }

        if (size) {
            *size = (uint64_t)(text.nitems + 1);
        }

        if (buffer && bufferSize > 0) {
            uint64_t copyLen = (text.nitems < bufferSize - 1) ? text.nitems : (bufferSize - 1);
            memcpy(buffer, text.value, (size_t)copyLen);
            buffer[copyLen] = '\0';
        }

        g_X11.free(text.value);
    }
}

void x11GetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    Window child = None;
    int winX = 0;
    int winY = 0;

    if (!g_X11.translateCoordinates(g_X11.display, xWin, g_X11.root, 0, 0, &winX, &winY, &child)) {
        return;
    }

    if (x) {
        *x = winX;
    }

    if (y) {
        *y = winY;
    }
}

void x11GetWindowSize(
    PalWindow* window, 
    uint32_t* width, 
    uint32_t* height)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!g_X11.getWindowAttributes(g_X11.display, xWin, &attr)) {
        return;
    }

    if (width) {
        *width = (uint32_t)attr.width;
    }

    if (height) {
        *height = (uint32_t)attr.height;
    }
}

void x11GetWindowState(
    PalWindow* window, 
    PalWindowState* state)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    Atom type;
    int format;
    unsigned long count = 0;
    unsigned long bytesAfter = 0;
    Atom* props = nullptr;

    g_X11.getWindowProperty(
        g_X11.display,
        xWin,
        g_X11Atoms._NET_WM_STATE,
        0,
        (~0L),
        False,
        XA_ATOM,
        &type,
        &format,
        &count,
        &bytesAfter,
        (unsigned char**)&props);

    PalWindowState _state = PAL_WINDOW_STATE_RESTORED;
    if (props) {
        for (unsigned long i = 0; i < count; ++i) {
            if (props[i] == g_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ ||
                props[i] == g_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT) {
                _state = PAL_WINDOW_STATE_MAXIMIZED;
            }

            if (props[i] == g_X11Atoms._NET_WM_STATE_HIDDEN) {
                _state = PAL_WINDOW_STATE_MINIMIZED;
            }
        }

        g_X11.free(props);
    }

    *state = _state;
}

PalBool x11IsWindowVisible(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!g_X11.getWindowAttributes(g_X11.display, xWin, &attr)) {
        return PAL_FALSE;
    }

    return attr.map_state == IsViewable;
}

PalWindow* x11GetFocusWindow(void)
{
    Window window;
    int tmp;

    g_X11.getInputFocus(g_X11.display, &window, &tmp);
    Window xWin = FROM_PAL_HANDLE(Window, window);
    if (xWin == g_X11.root) {
        return nullptr;
    }
    return TO_PAL_HANDLE(PalWindow, window);
}

void x11GetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    info->nativeInstance = (void*)g_X11.display;
    info->nativeWindow = (void*)window;
    info->nativeHandle1 = nullptr;
    info->nativeHandle2 = nullptr;
    info->nativeHandle3 = nullptr;
}

void x11SetWindowOpacity(
    PalWindow* window, 
    float opacity)
{
    uint32_t value = (uint32_t)(opacity * (float)0xFFFFFFFFUL + 0.5f);

    g_X11.changeProperty(
        g_X11.display,
        FROM_PAL_HANDLE(Window, window),
        g_X11Atoms._NET_WM_WINDOW_OPACITY,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)&value,
        1);

    g_X11.flush(g_X11.display);
}

void x11SetWindowStyle(
    PalWindow* window, 
    PalWindowStyle style)
{
    (void)window;
    (void)style;
}

void x11SetWindowTitle(
    PalWindow* window, 
    const char* title)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    
    if (g_X11Atoms.unicodeTitle) {
        g_X11.changeProperty(
            g_X11.display,
            xWin,
            g_X11Atoms._NET_WM_NAME,
            g_X11Atoms.UTF8_STRING,
            8,
            PropModeReplace,
            (const unsigned char*)title,
            (int)strlen(title));

    } else {
        g_X11.storeName(g_X11.display, xWin, title);
    }

    g_X11.flush(g_X11.display);
}

void x11SetWindowPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    g_X11.moveWindow(g_X11.display, xWin, x, y);
    g_X11.flush(g_X11.display);
}

void x11SetWindowSize(
    PalWindow* window, 
    uint32_t width, 
    uint32_t height)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XSizeHints hints = {0};
    long tmp = 0;

    g_X11.getWMNormalHints(g_X11.display, xWin, &hints, &tmp);
    if ((hints.flags & PMinSize) && (hints.flags & PMaxSize)) {
        hints.flags &= ~(PMinSize | PMaxSize);
        g_X11.setWMNormalHints(g_X11.display, xWin, &hints);
        g_X11.resizeWindow(g_X11.display, xWin, width, height);
        g_X11.flush(g_X11.display);

        // revert
        hints.flags |= (PMinSize | PMaxSize);
        hints.min_width = hints.max_width = (int)width;
        hints.min_height = hints.max_height = (int)height;
        g_X11.setWMNormalHints(g_X11.display, xWin, &hints);

    } else {
        g_X11.resizeWindow(g_X11.display, xWin, width, height);
    }

    g_X11.flush(g_X11.display);
}

void x11SetFocusWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    if (g_X11Atoms._NET_ACTIVE_WINDOW) {
        x11SendWMEvent(xWin, g_X11Atoms._NET_ACTIVE_WINDOW, CurrentTime, 0, 0, 0, PAL_TRUE);

    } else {
        g_X11.setInputFocus(g_X11.display, xWin, RevertToParent, CurrentTime);
    }
}

#endif // PAL_HAS_X11_BACKEND