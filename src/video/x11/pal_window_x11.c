
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_X11_BACKEND == 1
#include "pal_x11.h"
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

static int xErrorHandler(
    Display*,
    XErrorEvent* e)
{
    // this is use for simple success and failure
    s_X11.error = PAL_TRUE;
    return 0;
}

static XVisualInfo* glxBackend(const int fbConfigIndex)
{
    int count = 0;
    GLXFBConfig* configs = s_X11.glxGetFBConfigs(s_X11.display, s_X11.screen, &count);
    GLXFBConfig fbConfig = configs[fbConfigIndex];
    if (!fbConfig) {
        return nullptr;
    }

    // get a matching visual
    XVisualInfo* visualInfo = s_X11.glxGetVisualFromFBConfig(s_X11.display, fbConfig);
    if (!visualInfo) {
        return nullptr;
    }

    return visualInfo;
}

static XVisualInfo* eglXBackend(int fbConfigIndex)
{
    EGLDisplay display = EGL_NO_DISPLAY;
    display = s_VideoEgl.getDisplay((EGLNativeDisplayType)s_X11.display);
    if (display == EGL_NO_DISPLAY) {
        return nullptr;
    }

    EGLint numConfigs = 0;
    if (!s_VideoEgl.getConfigs(display, nullptr, 0, &numConfigs)) {
        return nullptr;
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_X11.allocator, configSize, 0);
    if (!eglConfigs) {
        return nullptr;
    }

    s_VideoEgl.getConfigs(display, eglConfigs, numConfigs, &numConfigs);
    EGLConfig config = eglConfigs[fbConfigIndex];

    // we get a visual info from the config
    EGLint visualID;
    s_VideoEgl.getConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &visualID);
    if (visualID == 0) {
        return nullptr;
    }

    int numVisuals = 0;
    XVisualInfo tmp;
    tmp.visualid = visualID;

    // get a matching visual info
    XVisualInfo* visualInfo = s_X11.getVisualInfo(s_X11.display, VisualIDMask, &tmp, &numVisuals);
    if (!visualInfo) {
        return nullptr;
    }

    palFree(s_X11.allocator, eglConfigs);
    return visualInfo;
}

void xGetMonitorInfo(PalMonitor*, PalMonitorInfo*);

PalResult xCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    Window window = None;
    PalMonitor* monitor = nullptr;
    PalMonitorInfo monitorInfo;

    WindowData* data = xGetFreeWindowData();
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
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }

        // create a colormap from the visual info
        visual = visualInfo->visual;
        depth = visualInfo->depth;
        bgPixel = 0;
        borderPixel = 0;
        colormap = s_X11.createColormap(s_X11.display, s_X11.root, visual, AllocNone);
        if (!colormap) {
            return PAL_RESULT_CODE_PLATFORM_FAILURE;
        }

        data->colormap = colormap;

    } else {
        // use a default visual
        visual = DefaultVisual(s_X11.display, s_X11.screen);
        depth = DefaultDepth(s_X11.display, s_X11.screen);
        bgPixel = WhitePixel(s_X11.display, s_X11.screen);
        borderPixel = BlackPixel(s_X11.display, s_X11.screen);
        colormap = DefaultColormap(s_X11.display, s_X11.screen);
        data->colormap = None;
    }

    // get monitor
    int monitorX = 0;
    int monitorY = 0;
    uint32_t monitorW = 0;
    uint32_t monitorH = 0;
    int dpi = 0;
    if (info->monitor) {
        monitor = info->monitor;

    } else {
        // get primary monitor
        xGetPrimaryMonitor(&monitor);
    }

    if (monitor) {
        // get monitor info
        xGetMonitorInfo(monitor, &monitorInfo);
        monitorX = monitorInfo.x;
        monitorY = monitorInfo.y;
        monitorW = monitorInfo.width;
        monitorH = monitorInfo.height;
        dpi = monitorInfo.dpi;

    } else {
        // primary monitor is not set
        XRRScreenResources* resources = nullptr;
        resources = s_X11.getScreenResources(s_X11.display, s_X11.root);
        for (int i = 0; i < resources->noutput; ++i) {
            RROutput output = resources->outputs[i];
            XRROutputInfo* outputInfo = s_X11.getOutputInfo(s_X11.display, resources, output);

            // check if its a monitor
            if (outputInfo->connection != RR_Connected || outputInfo->crtc == None) {
                s_X11.freeOutputInfo(outputInfo);
                continue;
            }

            XRRCrtcInfo* crtc = s_X11.getCrtcInfo(s_X11.display, resources, outputInfo->crtc);
            monitorX = crtc->x;
            monitorY = crtc->y;
            monitorW = crtc->width;
            monitorH = crtc->height;

            // get DPI
            float raw = crtc->width / 1920.0f;
            float steps[] = {1.0f, 1.2f, 1.5f, 1.75, 2.0f};
            float closest = steps[0];
            float minDiff = fabsf(raw - steps[0]);

            for (int i = 1; i < sizeof(steps) / sizeof(steps[0]); i++) {
                float diff = fabsf(raw - steps[i]);
                if (diff < minDiff) {
                    minDiff = diff;
                    closest = steps[i];
                }
            }

            dpi = (uint32_t)(closest * 96.0f);
            s_X11.freeCrtcInfo(crtc);
            s_X11.freeOutputInfo(outputInfo);
            break;
        }
        s_X11.freeScreenResources(resources);
    }

    int32_t x, y = 0;
    // the position and size must be scaled with the dpi before this call
    if (info->center) {
        x = monitorX + (monitorW - info->width) / 2;
        y = monitorY + (monitorH - info->height) / 2;

    } else {
        // we set 100 for each axix
        x = monitorX + 100;
        y = monitorY + 100;
    }

    // check and set transparency
    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        if (!(s_X11.features & PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW)) {
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
    window = s_X11.createWindow(
        s_X11.display,
        s_X11.root,
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

    if (window == None) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    // set pid property
    pid_t pid = getpid();
    s_X11.changeProperty(
        s_X11.display,
        window,
        s_X11Atoms._NET_WM_PID,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)&pid,
        1);

    // set class property
    XClassHint* hints = s_X11.allocClassHint();
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
        s_X11.setClassHint(s_X11.display, window, hints);
        s_X11.free(hints);
    }

    if (s_X11Atoms.unicodeTitle && info->title) {
        s_X11.changeProperty(
            s_X11.display,
            window,
            s_X11Atoms._NET_WM_NAME,
            s_X11Atoms.UTF8_STRING,
            8, // unsigned char
            PropModeReplace,
            info->title,
            strlen(info->title));

    } else {
        if (info->title) {
            s_X11.storeName(s_X11.display, window, info->title);
        }
    }

    // borderless
    if (info->style & PAL_WINDOW_STYLE_BORDERLESS) {
        if (!(s_X11.features & PAL_VIDEO_FEATURE_BORDERLESS_WINDOW)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

        s_X11.changeProperty(
            s_X11.display,
            window,
            s_X11Atoms._NET_WM_WINDOW_TYPE,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char*)&s_X11Atoms._NET_WM_WINDOW_TYPE_SPLASH,
            1);
    }

    // tool window
    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        if (!(s_X11.features & PAL_VIDEO_FEATURE_TOOL_WINDOW)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

        s_X11.changeProperty(
            s_X11.display,
            window,
            s_X11Atoms._NET_WM_WINDOW_TYPE,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char*)&s_X11Atoms._NET_WM_WINDOW_TYPE_UTILITY,
            1);
    }

    // topmost
    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        if (s_X11Atoms._NET_WM_STATE_ABOVE) {
            s_X11.changeProperty(
                s_X11.display,
                window,
                s_X11Atoms._NET_WM_STATE,
                XA_ATOM,
                32,
                PropModeAppend,
                (unsigned char*)&s_X11Atoms._NET_WM_STATE_ABOVE,
                1);
        }
    }

    // set size hints
    XSizeHints wmHints = {0};
    wmHints.flags = PPosition | PSize;
    wmHints.x = x;
    wmHints.y = y;
    wmHints.width = info->width;
    wmHints.height = info->height;

    // resizable
    if (!(info->style & PAL_WINDOW_STYLE_RESIZABLE)) {
        wmHints.flags |= PMinSize;
        wmHints.flags |= PMaxSize;
        wmHints.min_width = wmHints.max_width = info->width;
        wmHints.min_height = wmHints.max_height = info->height;
    }

    // show window
    s_X11.setWMNormalHints(s_X11.display, window, &wmHints);
    if (info->show) {
        s_X11.mapWindow(s_X11.display, window);
        s_X11.flush(s_X11.display);
    }

    // check if the window has been mapped
    // we use this to minimize or maximize
    XWindowAttributes attr;
    s_X11.getWindowAttributes(s_X11.display, window, &attr);
    PalBool windowMapped = attr.map_state = IsViewable;

    // maximize
    if (info->state == PAL_WINDOW_STATE_MAXIMIZED) {
        if (!(s_X11.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

        // if the window is not mapped, we wait till its mapped
        if (!windowMapped) {
            // wait till the window is mapped
            for (;;) {
                XEvent event;
                s_X11.nextEvent(s_X11.display, &event);
                if (event.type == MapNotify && event.xmap.window == window) {
                    windowMapped = PAL_TRUE;
                    break;
                }
            }
        }

        sendWMEvent(
            window,
            s_X11Atoms._NET_WM_STATE,
            s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
            s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
            1,
            0,
            PAL_TRUE); // _NET_WM_STATE_ADD
    }

    // minimize
    if (info->state == PAL_WINDOW_STATE_MINIMIZED) {
        if (!(s_X11.features & PAL_VIDEO_FEATURE_WINDOW_SET_STATE)) {
            s_X11.destroyWindow(s_X11.display, window);
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }

        // if the window is not mapped, we wait till its mapped
        if (!windowMapped) {
            // wait till the window is mapped
            for (;;) {
                XEvent event;
                s_X11.nextEvent(s_X11.display, &event);
                if (event.type == MapNotify && event.xmap.window == window) {
                    windowMapped = PAL_TRUE;
                    break;
                }
            }
        }
        s_X11.iconifyWindow(s_X11.display, window, s_X11.screen);
    }

    s_X11.setWMProtocols(s_X11.display, window, &s_X11Atoms.WM_DELETE_WINDOW, 1);
    s_X11.flush(s_X11.display);

    // attach the window data to the window
    data->skipConfigure = PAL_TRUE;
    data->skipState = PAL_TRUE;
    data->isAttached = PAL_FALSE; // PAL_TRUE for attached windows
    data->dpi = dpi;          // the current window monitor
    data->window = TO_PAL_HANDLE(PalWindow, window);
    s_X11.saveContext(s_X11.display, window, s_X11.dataID, (XPointer)data);

    // create an input context
    data->ic = s_X11.createIC(
        s_X11.im,
        XNInputStyle,
        XIMPreeditNothing | XIMStatusNothing,
        XNClientWindow,
        window,
        XNFocusWindow,
        window,
        nullptr);

    if (!data->ic) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    *outWindow = TO_PAL_HANDLE(PalWindow, window);
    return PAL_RESULT_SUCCESS;
}

void xDestroyWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    WindowData* data = nullptr;
    s_X11.findContext(s_X11.display, xWin, s_X11.dataID, (XPointer*)&data);

    // PAL does not destroy an attached window
    if (data->isAttached) {
        return;
    }

    s_X11.destroyIC(data->ic);
    s_X11.destroyWindow(s_X11.display, xWin);

    if (data->colormap != None) {
        s_X11.freeColormap(s_X11.display, data->colormap);
    }

    data->used = PAL_FALSE;
}

void xMinimizeWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    s_X11.iconifyWindow(s_X11.display, xWin, s_X11.screen);
}

void xMaximizeWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    sendWMEvent(
        xWin,
        s_X11Atoms._NET_WM_STATE,
        s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
        s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
        1,
        0,
        PAL_TRUE); // _NET_WM_STATE_ADD
}

void xRestoreWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    // since we have no fixed way to restore the window
    // we just restore from minimized and maximized state
    sendWMEvent(
        xWin,
        s_X11Atoms._NET_WM_STATE,
        s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT,
        s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ,
        1,
        0,
        PAL_FALSE); // _NET_WM_STATE_REMOVE

    s_X11.mapRaised(s_X11.display, xWin);
}

void xShowWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    s_X11.mapWindow(s_X11.display, xWin);
}

void xHideWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    s_X11.unmapWindow(s_X11.display, xWin);
}

void xFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    PalBool add = PAL_FALSE;
    if (info->flags & PAL_FLASH_FLAG_TRAY) {
        add = PAL_TRUE;
    }

    // check if modern flashing is supported
    if (s_X11Atoms._NET_WM_STATE_DEMANDS_ATTENTIONS) {
        sendWMEvent(
            xWin,
            s_X11Atoms._NET_WM_STATE,
            s_X11Atoms._NET_WM_STATE_DEMANDS_ATTENTIONS,
            0,
            0,
            0,
            add); // _NET_WM_STATE_ADD

    } else {
        // legacy mode
        XWMHints* hints = s_X11.getWMHints(s_X11.display, xWin);
        if (!hints) {
            hints = s_X11.allocWMHints();
            if (!hints) {
                return;
            }

            if (add) {
                hints->flags |= XUrgencyHint;
            } else {
                hints->flags &= ~XUrgencyHint;
            }
            s_X11.setWMHints(s_X11.display, xWin, hints);
            s_X11.free(hints);
        }
    }
}

void xGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    if (s_X11Atoms.unicodeTitle) {
        Atom type;
        int format;
        unsigned long count, bytesAfter;
        unsigned char* prop = nullptr;
        s_X11.getWindowProperty(
            s_X11.display,
            xWin,
            s_X11Atoms._NET_WM_NAME,
            0,
            (~0L),
            False,
            s_X11Atoms.UTF8_STRING,
            &type,
            &format,
            &count,
            &bytesAfter,
            &prop);

        if (bufferSize >= count) {
            strcpy(outBuffer, (const char*)prop);
        } else {
            // copy up to the limiit of the supplied buffer
            strncpy(outBuffer, (const char*)prop, bufferSize);
        }
        s_X11.free(prop);

    } else {
        XTextProperty text;
        if (!s_X11.getWMName(s_X11.display, xWin, &text)) {
            return;
        }

        if (bufferSize >= text.nitems) {
            strcpy(outBuffer, (const char*)text.value);
        } else {
            // copy up to the limiit of the supplied buffer
            strncpy(outBuffer, (const char*)text.value, bufferSize);
        }
        s_X11.free(text.value);
    }
}

void xGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return;
    }

    if (x) {
        *x = attr.x;
    }

    if (y) {
        *y = attr.y;
    }
}

void xGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return;
    }

    if (width) {
        *width = attr.width;
    }

    if (height) {
        *height = attr.height;
    }
}

void xGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    Atom type;
    int format;
    unsigned long count, bytesAfter;
    unsigned char* props = nullptr;
    s_X11.getWindowProperty(
        s_X11.display,
        xWin,
        s_X11Atoms._NET_WM_STATE,
        0,
        (~0L),
        False,
        XA_ATOM,
        &type,
        &format,
        &count,
        &bytesAfter,
        &props);

    PalWindowState state = PAL_WINDOW_STATE_RESTORED;
    for (unsigned long i = 0; i < count; ++i) {
        if (props[i] == s_X11Atoms._NET_WM_STATE_MAXIMIZED_HORZ) {
            state = PAL_WINDOW_STATE_MAXIMIZED;
        }

        if (props[i] == s_X11Atoms._NET_WM_STATE_MAXIMIZED_VERT) {
            state = PAL_WINDOW_STATE_MAXIMIZED;
        }

        if (props[i] == s_X11Atoms._NET_WM_STATE_HIDDEN) {
            state = PAL_WINDOW_STATE_MINIMIZED;
        }
    }

    s_X11.free(props);
    *outState = state;
}

PalBool xIsWindowVisible(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_FALSE;
    }

    return attr.map_state == IsViewable;
}

PalWindow* xGetFocusWindow()
{
    Window window;
    int tmp;
    s_X11.getInputFocus(s_X11.display, &window, &tmp);
    Window xWin = FROM_PAL_HANDLE(Window, window);
    if (xWin == s_X11.root) {
        return nullptr;
    }
    return TO_PAL_HANDLE(PalWindow, window);
}

void xGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    info->nativeInstance = (void*)s_X11.display;
    info->nativeWindow = (void*)window;
    info->nativeHandle1 = nullptr;
    info->nativeHandle2 = nullptr;
    info->nativeHandle3 = nullptr;
}

void xSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    unsigned long value = (unsigned long)(opacity * 0xFFFFFFFFUL + 0.5f);
    s_X11.changeProperty(
        s_X11.display,
        FROM_PAL_HANDLE(Window, window),
        s_X11Atoms._NET_WM_WINDOW_OPACITY,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)&value,
        1);

    s_X11.flush(s_X11.display);
}

void xSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    if (s_X11Atoms.unicodeTitle) {
        s_X11.changeProperty(
            s_X11.display,
            xWin,
            s_X11Atoms._NET_WM_NAME,
            s_X11Atoms.UTF8_STRING,
            8, // unsigned char
            PropModeReplace,
            title,
            strlen(title));

    } else {
        s_X11.storeName(s_X11.display, xWin, title);
    }

    s_X11.flush(s_X11.display);
}

void xSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    s_X11.moveWindow(s_X11.display, xWin, x, y);
    s_X11.flush(s_X11.display);
}

void xSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    // X11 does not allow users resize programaticaly
    // if the window is not resizable.
    // so we hack it by making the window resizable and resizing
    // then revert back.
    XSizeHints hints;
    long tmp;
    s_X11.getWMNormalHints(s_X11.display, xWin, &hints, &tmp);
    if ((hints.flags & PMinSize) && (hints.flags & PMaxSize)) {
        hints.flags &= ~PMinSize;
        hints.flags &= ~PMaxSize;
        s_X11.resizeWindow(s_X11.display, xWin, width, height);
        s_X11.flush(s_X11.display);

        // revert
        hints.flags |= PMinSize;
        hints.flags |= PMaxSize;
        hints.min_width = hints.max_width = width;
        hints.min_height = hints.max_height = height;

    } else {
        // window is already resizable
        s_X11.resizeWindow(s_X11.display, xWin, width, height);
    }

    s_X11.flush(s_X11.display);
}

void xSetFocusWindow(PalWindow* window)
{
    Window xWin = FROM_PAL_HANDLE(Window, window);
    if (s_X11Atoms._NET_ACTIVE_WINDOW) {
        sendWMEvent(xWin, s_X11Atoms._NET_ACTIVE_WINDOW, CurrentTime, 0, 0, 0, PAL_TRUE); // 1
    } else {
        s_X11.setInputFocus(s_X11.display, xWin, RevertToParent, CurrentTime);
    }
}

PalResult xAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    Window xWin = FROM_PAL_HANDLE(Window, windowHandle);
    XWindowAttributes attr;
    if (!s_X11.getWindowAttributes(s_X11.display, xWin, &attr)) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // get a free slot and set the window handle to it
    // we also set a flag to make sure we know this is an attached window
    WindowData* data = xGetFreeWindowData();
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    PalWindow* window = TO_PAL_HANDLE(PalWindow, xWin);
    // we assume the window was just created, since there is
    // no official way to get the DPI
    data->isAttached = PAL_TRUE;
    data->dpi = 96; // if this is not the DPI, a dpi event will be triggered

    // If the window manager has not mapped the window yet,
    // we dont need the initial Size / Move events
    data->skipConfigure = PAL_TRUE;
    data->skipState = PAL_TRUE;
    data->skipIfAttached = PAL_TRUE;
    data->window = window;
    data->w = attr.width;
    data->h = attr.height;
    data->x = attr.x;
    data->y = attr.y;

    // get the current window state
    // we dont check the return code because we know the window is valid
    xGetWindowState(window, &data->state);

    // listen to the events we support
    long mask = StructureNotifyMask | KeyPressMask;
    mask |= KeyReleaseMask;
    mask |= ButtonPressMask;
    mask |= ButtonReleaseMask;
    mask |= PointerMotionMask;
    mask |= FocusChangeMask;
    mask |= PropertyChangeMask;
    s_X11.selectInput(s_X11.display, xWin, mask);

    // listen to window close event
    s_X11.setWMProtocols(s_X11.display, xWin, &s_X11Atoms.WM_DELETE_WINDOW, 1);

    s_X11.saveContext(s_X11.display, xWin, s_X11.dataID, (XPointer)data);
    s_X11.flush(s_X11.display);

    *outWindow = window;
    return PAL_RESULT_SUCCESS;
}

PalResult xDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    // we check is the window is really detachable
    Window xWin = FROM_PAL_HANDLE(Window, window);
    WindowData* data = nullptr;
    s_X11.findContext(s_X11.display, xWin, s_X11.dataID, (XPointer*)&data);
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
    s_X11.selectInput(s_X11.display, xWin, mask);
    s_X11.setWMProtocols(s_X11.display, xWin, nullptr, 0);

    if (outWindowHandle) {
        *outWindowHandle = (void*)window;
    }

    return PAL_RESULT_SUCCESS;
}

#endif // PAL_HAS_X11_BACKEND