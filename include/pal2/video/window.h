
/**
 * @file window.h
 * 
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

#ifndef PAL_VIDEO_WINDOW_H
#define PAL_VIDEO_WINDOW_H

#include "types.h"

/**
 * @defgroup window_styles Window Styles
 * @ingroup pal_video
 * @{
 */
#define PAL_WINDOW_STYLE_RESIZABLE (1U << 0) /**< Window is resizable. */
#define PAL_WINDOW_STYLE_TRANSPARENT (1U << 1) /**< Window is transparent. */
#define PAL_WINDOW_STYLE_TOPMOST (1U << 2) /**< Window is topmost. */
#define PAL_WINDOW_STYLE_NO_MINIMIZEBOX (1U << 3) /**< Window has no minimize button. */
#define PAL_WINDOW_STYLE_NO_MAXIMIZEBOX (1U << 4) /**< Window has no maximize button. */
#define PAL_WINDOW_STYLE_TOOL (1U << 5) /**< Window has no taskbar icon. */
#define PAL_WINDOW_STYLE_BORDERLESS (1U << 6) /**< Window has no decorations. */
/** @} */

/**
 * @defgroup window_states Window States
 * @ingroup pal_video
 * @{
 */
#define PAL_WINDOW_STATE_NORMAL 0 /**< Windowed mode. */
#define PAL_WINDOW_STATE_MAXIMIZED 1 /**< Maximized window. */
#define PAL_WINDOW_STATE_MINIMIZED 2 /**< Minimized window. */
#define PAL_WINDOW_STATE_RESTORED 3 /**< Window restored from a minimized or maximized state. */
#define PAL_WINDOW_STATE_COUNT 4 /**< Number of window states. */
/** @} */

/**
 * @defgroup flash_flags Window Flash Flags
 * @ingroup pal_video
 * @{
 */
#define PAL_FLASH_FLAG_STOP 0 /**< Stop all flash operations. */
#define PAL_FLASH_FLAG_CAPTION (1U << 0) /**< Flash the window title bar. */
#define PAL_FLASH_FLAG_TRAY (1U << 1) /**< Flash the window taskbar icon. */
/** @} */

/**
 * @defgroup fbconfig_backends Window Framebuffer Configuration Backends
 * @ingroup pal_video
 * @{
 */
#define PAL_FBCONFIG_BACKEND_PAL_OPENGL 0 /**< PAL OpenGL backend. */
#define PAL_FBCONFIG_BACKEND_EGL 1 /**< EGL backend. */
#define PAL_FBCONFIG_BACKEND_GLX 2 /**< GLX backend. */
#define PAL_FBCONFIG_BACKEND_WGL 3 /**< WGL backend. */
#define PAL_FBCONFIG_BACKEND_COUNT 4 /**< Number of framebuffer configuration backends. */
/** @} */

/**
 * @struct PalFlashInfo
 * @brief Flash information.
 *
 * Passed to @ref palFlashWindow().
 *
 * `interval` greater than `0` requires the @ref PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL
 * feature to be supported.
 *
 * The following flash flags require:
 *
 * - @ref PAL_FLASH_FLAG_CAPTION - @ref PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION feature to be supported
 *
 * - @ref PAL_FLASH_FLAG_TRAY - @ref PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY feature to be supported
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalFlashInfo {
    PalFlashFlags flags; /**< Bitmask of flash flags. */
    uint32_t interval; /**< Flash interval in ms, or `0`. */
    uint32_t count; /**< Number of times to flash, or `0`. */
} PalFlashInfo;

/**
 * @struct PalWindowHandleInfo
 * @brief Window handle information.
 *
 * Filled in by @ref palGetWindowHandleInfo().
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalWindowHandleInfo {
    void* nativeInstance; /**< Native instance of the window or display. */
    void* nativeWindow; /**< Native window handle. */
    void* nativeHandle1; /**< Additional native window handle, or `nullptr`. */
    void* nativeHandle2; /**< Additional native window handle, or `nullptr`. */
    void* nativeHandle3; /**< Additional native window handle, or `nullptr`. */
} PalWindowHandleInfo;

/**
 * @struct PalWindowCreateInfo
 * @brief Window creation parameters.
 *
 * This struct is used only during @ref palCreateWindow() and may be
 * discarded after the function returns.
 *
 * `center` only works for windowed mode. It is ignored if `monitor` is `nullptr`
 * and the initial state of the window is not @ref PAL_WINDOW_STATE_NORMAL.
 *
 * The following window styles require:
 *
 * - @ref PAL_WINDOW_STYLE_TRANSPARENT - @ref PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW feature to be supported
 *
 * - @ref PAL_WINDOW_STYLE_TOPMOST - @ref PAL_VIDEO_FEATURE_TOPMOST_WINDOW feature to be supported
 *
 * - @ref PAL_WINDOW_STYLE_NO_MINIMIZEBOX - @ref PAL_VIDEO_FEATURE_NO_MINIMIZEBOX feature to be supported
 *
 * - @ref PAL_WINDOW_STYLE_NO_MAXIMIZEBOX - @ref PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX feature to be supported
 *
 * - @ref PAL_WINDOW_STYLE_TOOL - @ref PAL_VIDEO_FEATURE_TOOL_WINDOW feature to be supported
 *
 * - @ref PAL_WINDOW_STYLE_BORDERLESS - @ref PAL_VIDEO_FEATURE_BORDERLESS_WINDOW feature to be supported
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalWindowCreateInfo {
    const char* title; /**< Window title (NULL-terminated, UTF-8 encoded). */
    PalMonitor* monitor; /**< Monitor to use, or `nullptr` for windowed mode. */
    const char* appName; /**< Application name (NULL-terminated, UTF-8 encoded), or `nullptr` to use `PAL`. */
    const char* instanceName; /**< Instance name (NULL-terminated, UTF-8 encoded), or `nullptr` to use `title`. */
    PalFBConfigBackend fbConfigBackend; /**< Framebuffer configuration backend. */
    int32_t fbConfigIndex; /**< Framebuffer configuration index, or `0` for no configuration. */
    uint32_t width; /**< Window width in screen coordinates. */
    uint32_t height; /**< Window height in screen coordinates. */
    PalBool show; /**< Whether the window is visible after creation. */
    PalWindowStyle style; /**< Bitmask of window styles. */
    PalWindowState state; /**< Initial window state. */
    PalBool center; /**< Whether to center the window after creation. */
} PalWindowCreateInfo;

/**
 * @brief Creates a window.
 *
 * The created window must be destroyed using @ref palDestroyWindow().
 *
 * This function creates a window using the specified creation parameters.
 * `info` must remain valid for the duration of this function. PAL does not
 * copy its contents.
 *
 * Very large or small `info->width` and `info->height` will be overridden
 * by the video system and will be ignored if `info->monitor` is not `nullptr`.
 * Setting `info->fbConfigIndex` does not create a Vulkan surface or OpenGL context
 * for the specified window. This creates the window with the specified
 * framebuffer configuration or pixel format index.
 *
 * To create a borderless fullscreen window, use @ref PAL_WINDOW_STYLE_BORDERLESS style
 * and select a monitor. The window will be created in windowed mode if the selected
 * monitor is not valid. To create an exclusive fullscreen window, use @ref palSetMonitorMode()
 * to set the display mode of the monitor. The monitor display mode will not be switched if
 * it is invalid.
 *
 * The video system must be initialized before this call.
 *
 * @param[in] info Window creation parameters.
 * @param[out] window Output handle to receive the created window.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must be called from the main thread.
 * 
 * @Result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @Result-code @ref PAL_RESULT_CODE_OUT_OF_MEMORY
 * @Result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
 *
 * @note On `Wayland`:
 *
 * - Creating non-resizable windows is not supported. It will be ignored.
 *
 * - Creating windows on a specific monitor is not supported.
 *
 * - Creating hidden windows is not supported. It will be ignored.
 *
 * On `Win32`:
 *
 * - Creating non-resizable windows also removes the maximize box.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palDestroyWindow
 */
PAL_API PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** window);

/**
 * @brief Destroys a window.
 *
 * This function destroys the specified window and frees its resources if it was
 * created by the video system. Using this function with a foreign window does nothing.
 *
 * @param[in] window The window to destroy.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palCreateWindow
 */
PAL_API void PAL_CALL palDestroyWindow(PalWindow* window);

/**
 * @brief Minimizes a window.
 *
 * This function minimizes a window if it is not already minimized.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_STATE feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to minimize.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palMaximizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMinimizeWindow(PalWindow* window);

/**
 * @brief Maximizes a window.
 *
 * This function maximizes a window if it is not already maximized.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_STATE feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to maximize.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palMinimizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMaximizeWindow(PalWindow* window);

/**
 * @brief Restores a window.
 *
 * This function restores the specified window to its previous state if not restored already.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_STATE feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to restore.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @note Wayland does not support restoring a minimized window.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palMinimizeWindow
 * @sa palMaximizeWindow
 */
PAL_API void PAL_CALL palRestoreWindow(PalWindow* window);

/**
 * @brief Shows the window.
 *
 * This function shows the specified window if it is not shown.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to show.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palHideWindow
 */
PAL_API void PAL_CALL palShowWindow(PalWindow* window);

/**
 * @brief Hides the window.
 *
 * This function hides the specified window if it is not hidden.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to hide.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palShowWindow
 */
PAL_API void PAL_CALL palHideWindow(PalWindow* window);

/**
 * @brief Flashes a window.
 *
 * This function requests the platform to visually flash the specified window.
 * The features required depend on the flags in `info`, see @ref PalFlashInfo.
 *
 * @param[in] window Window to flash.
 * @param[in] info Window flash parameters.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

/**
 * @brief Attaches a window.
 *
 * This function attaches a foreign or native window to the video system.
 * @ref PAL_VIDEO_FEATURE_FOREIGN_WINDOWS feature must be supported.
 *
 * This function registers `windowHandle` with the video system to allow the window
 * to use the video system API. The video system does not own the attached window; users
 * are required to destroy the window after it has been detached with @ref palDetachWindow().
 *
 * `windowHandle` must be created with the same instance the video system uses. Use
 * @ref palGetInstance() to get the instance the video system uses. See @ref palInitVideo()
 * to set a preferred instance.
 *
 * @param[in] windowHandle Foreign or native window.
 * @param[out] window Output handle to receive the attached window.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must be called from the main thread.
 * 
 * @Result-code @ref PAL_RESULT_CODE_INVALID_ARGUMENT
 * @Result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palInitVideo
 * @sa palGetInstance
 * @sa palDetachWindow
 */
PAL_API PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** window);

/**
 * @brief Detaches a window.
 *
 * This function detaches a foreign or native window from the video system.
 * `window` must not be owned by the video system and detaching the window will not
 * destroy it; users are responsible for destroying the window after it has been detached.
 * @ref PAL_VIDEO_FEATURE_FOREIGN_WINDOWS feature must be supported.
 *
 * @param[in] window Window to detach.
 * @param[out] windowHandle Output handle to receive the foreign window, or `nullptr` to ignore it.
 * @return `PAL_RESULT_SUCCESS` on success or result value on failure.
 *         Call @ref palFormatResult() for more information.
 *
 * @Thread-safety Must be called from the main thread.
 * 
 * @Result-code @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED
 * @Result-code @ref PAL_RESULT_CODE_INVALID_HANDLE
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palAttachWindow
 */
PAL_API PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** windowHandle);

/**
 * @brief Gets the style of a window.
 *
 * This function gets the current style of the specified window.
 * @ref PAL_VIDEO_FEATURE_WINDOW_GET_STYLE feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to get its style.
 * @param[out] style Output to receive the window style.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palSetWindowStyle
 */
PAL_API void PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* style);

/**
 * @brief Gets the monitor the window is currently on.
 *
 * @ref PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR feature must be supported or this function
 * results in undefined behavior.
 *
 * @param[in] window Window to get its monitor.
 * @param[out] monitor Output handle to receive the monitor.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** monitor);

/**
 * @brief Gets the title of the window.
 *
 * This is a two-call function, set `buffer` to `nullptr` and `bufferSize` to `0` to obtain
 * the size of the window title in bytes in `size`. Allocate the array and call this function
 * again with `buffer` set to the allocated array and `bufferSize` set to the
 * capacity of the array. @ref PAL_VIDEO_FEATURE_WINDOW_GET_TITLE feature must be
 * supported or this function results in undefined behavior.
 *
 * If the specified size is less than the size of the window title, only the
 * characters that fit in the array will be written.
 *
 * @param[in] window Window to get its title.
 * @param[in] bufferSize Size of the buffer in bytes.
 * @param[out] size Output to receive the size of the window title in bytes, or `nullptr` to ignore it.
 * @param[out] buffer Output buffer to receive the window title, or `nullptr` to get its size.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palSetWindowTitle
 */
PAL_API void PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer);

/**
 * @brief Gets the position of the content area of a window.
 *
 * This function gets the position of the content area of the specified window
 * in screen coordinates. Coordinates are relative to the upper-left corner of the window.
 * X-coordinate increases to the right and Y-coordinate increases down.
 * @ref PAL_VIDEO_FEATURE_WINDOW_GET_POS feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to get its content area position.
 * @param[out] x Output to receive the window x position, or `nullptr` to ignore it.
 * @param[out] y Output to receive the window y position, or `nullptr` to ignore it.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palSetWindowPos
 */
PAL_API void PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Gets the size of the content area of a window.
 *
 * This function gets the size of the content area of the specified window in
 * screen coordinates. @ref PAL_VIDEO_FEATURE_WINDOW_GET_SIZE feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to get its content area size.
 * @param[out] width Output to receive the window width, or `nullptr` to ignore it.
 * @param[out] height Output to receive the window height, or `nullptr` to ignore it.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palSetWindowSize
 */
PAL_API void PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height);

/**
 * @brief Gets the state of a window.
 *
 * @ref PAL_VIDEO_FEATURE_WINDOW_GET_STATE feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to get its state.
 * @param[out] state Output to receive the window state.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* state);

/**
 * @brief Checks whether a window is visible.
 *
 * @ref PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to check its visibility.
 * @return `PAL_TRUE` if the window is visible, otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API PalBool PAL_CALL palIsWindowVisible(PalWindow* window);

/**
 * @brief Gets the input-focused window.
 *
 * This function gets the window that currently has keyboard or mouse input focus within
 * the application.
 * @ref PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS feature must be
 * supported or this function results in undefined behavior.
 *
 * @return Input-focused window on success, or `nullptr` on failure.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API PalWindow* PAL_CALL palGetFocusWindow(void);

/**
 * @brief Gets the native handle of the window.
 *
 * @param[in] window Window to get its native handles.
 * @param[out] info Output struct to receive the window handle information.
 *
 * @Thread-safety Thread safe.
 *
 * @note On `Wayland`:
 *
 * - info->nativeHandle1 is `xdg_surface`.
 *
 * - info->nativeHandle2 is `xdg_toplevel`.
 *
 * - info->nativeHandle3 is `wl_egl_window`.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetWindowHandleInfo(
    PalWindow* window,
    PalWindowHandleInfo* info);

/**
 * @brief Sets the opacity of a window.
 *
 * This function sets the opacity of the specified window. The window must have
 * @ref PAL_WINDOW_STYLE_TRANSPARENT style.
 * @ref PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to set its opacity.
 * @param[in] opacity Window opacity in the range `0.0` to `1.0`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity);

/**
 * @brief Sets the style of a window.
 *
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_STYLE feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to set its style.
 * @param[in] style Window style to set.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palGetWindowStyle
 */
PAL_API void PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style);

/**
 * @brief Sets the title of a window.
 *
 * This function sets the title of the specified window. `title` must be a NULL-terminated
 * UTF-8 encoded string. @ref PAL_VIDEO_FEATURE_WINDOW_SET_TITLE feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to set its title.
 * @param[in] title Window title (NULL-terminated, UTF-8 encoded).
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palGetWindowTitle
 */
PAL_API void PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title);

/**
 * @brief Sets the position of the content area of a window.
 *
 * This function sets the position of the content area of the specified window in
 * screen coordinates. Coordinates are relative to the upper-left corner of the window.
 * X-coordinate increases to the right and Y-coordinate increases down.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_POS feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to set its content area position.
 * @param[in] x X coordinate of the window.
 * @param[in] y Y coordinate of the window.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palGetWindowPos
 */
PAL_API void PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Sets the size of the content area of a window.
 *
 * This function sets the size of the content area of the specified window in
 * screen coordinates. Very large or small `width` and `height` will be overridden
 * by the video system. @ref PAL_VIDEO_FEATURE_WINDOW_SET_SIZE feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to set its content area size.
 * @param[in] width Width of the window.
 * @param[in] height Height of the window.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palGetWindowSize
 */
PAL_API void PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height);

/**
 * @brief Requests input focus for a window.
 *
 * This function requests keyboard and mouse input focus for the specified window within
 * the application. The window must be visible.
 * @ref PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS feature must be
 * supported or this function results in undefined behavior.
 *
 * @param[in] window Window to request input-focus for.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 *
 * @sa palGetFocusWindow
 */
PAL_API void PAL_CALL palSetFocusWindow(PalWindow* window);

#endif // PAL_VIDEO_WINDOW_H