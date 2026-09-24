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

#ifndef PAL_VIDEO_WINDOW_H
#define PAL_VIDEO_WINDOW_H

#include "monitor.h"

/**
 * @defgroup window_styles Window Styles
 * @brief Window Styles.
 * 
 * @{
 */
#define PAL_WINDOW_STYLE_RESIZABLE (1U << 0)
#define PAL_WINDOW_STYLE_TRANSPARENT (1U << 1)
#define PAL_WINDOW_STYLE_TOPMOST (1U << 2)
#define PAL_WINDOW_STYLE_NO_MINIMIZEBOX (1U << 3)
#define PAL_WINDOW_STYLE_NO_MAXIMIZEBOX (1U << 4)
#define PAL_WINDOW_STYLE_TOOL (1U << 5)
#define PAL_WINDOW_STYLE_BORDERLESS (1U << 6)
/** @} */

/**
 * @defgroup window_states Window States
 * @brief Window States
 * 
 * @{
 */
#define PAL_WINDOW_STATE_NORMAL 0
#define PAL_WINDOW_STATE_MAXIMIZED 1
#define PAL_WINDOW_STATE_MINIMIZED 2
#define PAL_WINDOW_STATE_RESTORED 3
#define PAL_WINDOW_STATE_COUNT 4
/** @} */

/**
 * @defgroup flash_flags Flash Flags
 * @brief Flash Flash
 * 
 * @{
 */
#define PAL_FLASH_FLAG_STOP 0
#define PAL_FLASH_FLAG_CAPTION (1U << 0)
#define PAL_FLASH_FLAG_TRAY (1U << 1)
/** @} */

/**
 * @defgroup fbconfig_backend FBConfig Backend
 * @brief FBConfig Backend
 * 
 * @{
 */
#define PAL_FBCONFIG_BACKEND_PAL_OPENGL 0
#define PAL_FBCONFIG_BACKEND_EGL 1
#define PAL_FBCONFIG_BACKEND_GLX 2
#define PAL_FBCONFIG_BACKEND_WGL 3
#define PAL_FBCONFIG_BACKEND_COUNT 4
/** @} */

/**
 * @typedef PalWindowStyle
 * @brief Window styles. 
 * 
 * Multiple styles can be OR'ed together using bitwise OR operator (`|`).
 * 
 * All values of this type follow the format `PAL_WINDOW_STYLE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalWindowStyle;

/**
 * @typedef PalWindowState
 * @brief Window states.
 * 
 * All values of this type follow the format `PAL_WINDOW_STATE_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalWindowState;

/**
 * @typedef PalFlashFlags
 * @brief Flash flags. 
 * 
 * Multiple flags can be OR'ed together using bitwise OR operator (`|`).
 *
 * `PAL_FLASH_FLAG_STOP` is not a bit and must not be combined with other bits.
 * 
 * All values of this type follow the format `PAL_FLASH_FLAG_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFlashFlags;

/**
 * @typedef PalFBConfigBackend
 * @brief The backend of a FBConfig.
 * 
 * All values of this type follow the format `PAL_FBCONFIG_BACKEND_*` for API
 * consistency and ease of use.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalFBConfigBackend;

/**
 * @struct PalWindow
 * @brief Opaque handle to a window.
 *
 * @since Added in version 2.0
 */
typedef struct PalWindow PalWindow;

/**
 * @struct PalFlashInfo
 * @brief Contains information for flashing a window.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalFlashInfo::flags
 * The flash flags. This is a bitmask.
 * - `PAL_FLASH_FLAG_STOP:` stop a flash operation. @nl
 * - `PAL_FLASH_FLAG_CAPTION:` flash the titlebar of a window.
 *   `PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION` must be supported. @nl
 * - `PAL_FLASH_FLAG_TRAY:` flash the taskbar icon of a window.
 *   `PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY` must be supported. @nl
 * 
 * @var PalFlashInfo::interval
 * The flash interval in milliseconds.
 * If the interval is greater than `0`, @nl
 * `PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL` must be supported.
 * 
 * @var PalFlashInfo::count
 * The number of times to flash. 
 * Set to `0` to flash until focused or cancelled.
 */
typedef struct PalFlashInfo
{
    PalFlashFlags flags;
    uint32_t interval;
    uint32_t count;
} PalFlashInfo;

/**
 * @struct PalWindowHandleInfo
 * @brief Contains information about a window handle.
 *
 * @since Added in version 2.0
 * 
 * @var PalWindowHandleInfo::nativeInstance
 * The platform (OS) display or instance of the window. Will always be valid.
 * 
 * @var PalWindowHandleInfo::nativeWindow
 * The platform (OS) handle of the window. Will always be valid.
 * 
 * @var PalWindowHandleInfo::nativeHandle1
 * First extra handle of the window. Will be `nullptr` if not available.
 * (eg. `xdgSurface` on Wayland).
 * 
 * @var PalWindowHandleInfo::nativeHandle2
 * Second extra handle of the window. Will be `nullptr` if not available.
 * (eg. `xdgToplevel` on Wayland).
 * 
 * @var PalWindowHandleInfo::nativeHandle3
 * Third extra handle of the window. Will be `nullptr` if not available.
 * (eg. `wl_egl_window` on Wayland).
 */
typedef struct PalWindowHandleInfo
{
    void* nativeInstance;
    void* nativeWindow;
    void* nativeHandle1;
    void* nativeHandle2;
    void* nativeHandle3;
} PalWindowHandleInfo;

/**
 * @struct PalWindowCreateInfo
 * @brief Contains creation parameters of a window.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * 
 * @var PalWindowCreateInfo::title
 * The title of the window in `UTF-8` encoding.
 * 
 * @var PalWindowCreateInfo::monitor
 * The monitor the window should be created on.
 * Set to `nullptr` to use the primary or first monitor available.
 * 
 * @var PalWindowCreateInfo::appName
 * The window app name in `UTF-8` encoding. If `nullptr`, "PAL" will be used.
 * 
 * @var PalWindowCreateInfo::instanceName
 * The window instance name in `UTF-8` encoding. 
 * If `nullptr`, `title` will be used.
 * 
 * @var PalWindowCreateInfo::fbConfigBackend
 * The FBConfig backend of the index at `fbConfigIndex`.
 * Will be ignored if the index is `0`.
 * 
 * @var PalWindowCreateInfo::fbConfigIndex
 * The FBConfig index. This is the loop index of the fbconfig in the
 * drivers fbconfigs. `fbConfigBackend` must match the source to get the @nl
 * exact fbconfig. Set to `0` to create the window without any FBConfig.
 * 
 * @var PalWindowCreateInfo::width
 * The width of the window in pixels.
 * 
 * @var PalWindowCreateInfo::height
 * The height of the window in pixels.
 * 
 * @var PalWindowCreateInfo::show
 * `PAL_TRUE` to show the window after its created.
 * 
 * @var PalWindowCreateInfo::style
 * The window style. This is a bitmask.
 * - `PAL_WINDOW_STYLE_RESIZABLE:` window is resizable. @nl
 * - `PAL_WINDOW_STYLE_TOPMOST:` window is the topmost.
 *   `PAL_VIDEO_FEATURE_TOPMOST_WINDOW` must be supported. @nl
 * - `PAL_WINDOW_STYLE_TRANSPARENT:` window is transparent.
 *   `PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW` must be supported. @nl
 * - `PAL_WINDOW_STYLE_NO_MINIMIZEBOX:` window has no minimize box.
 *   `PAL_VIDEO_FEATURE_NO_MINIMIZEBOX` must be supported. @nl
 * - `PAL_WINDOW_STYLE_NO_MAXIMIZEBOX:` window has no maximize box.
 *   `PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX` must be supported. @nl
 * - `PAL_WINDOW_STYLE_TOOL:` window has no taskbar icon.
 *   `PAL_VIDEO_FEATURE_TOOL_WINDOW` must be supported. @nl
 * - `PAL_WINDOW_STYLE_BORDERLESS:` window has no decorations.
 *   `PAL_VIDEO_FEATURE_BORDERLESS_WINDOW` must be supported. @nl
 * 
 * @var PalWindowCreateInfo::state
 * The initial window state. Set to `PAL_WINDOW_STATE_NORMAL` to create
 * the window in windowed mode.
 * 
 * @var PalWindowCreateInfo::center
 * `PAL_TRUE` to center the window after creation. This only works for 
 * windowed mode.
 */
typedef struct PalWindowCreateInfo
{
    const char* title;
    PalMonitor* monitor;
    const char* appName;
    const char* instanceName;
    PalFBConfigBackend fbConfigBackend;
    int32_t fbConfigIndex;
    uint32_t width;
    uint32_t height;
    PalBool show;
    PalWindowStyle style;
    PalWindowState state;
    PalBool center;
} PalWindowCreateInfo;

/**
 * @brief Creates a window.
 * 
 * The window width and height is restricted by the platform. Very small 
 * or large width and height might be overridden by the system or
 * the window will fail to create.
 * 
 * This function creates the window and optionally sets the pixel format
 * or fbconfig if `PalWindowCreateInfo::fbConfigIndex` is not `0`.
 * This does not create context for opengl, vulkan etc. 
 *
 * @param[in] info Information about how to create the window.
 * @param[out] window The output handle to recieve the created window.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @note On `Wayland`:
 *
 * - Creating non resizable windows is not supported. It will be ignored.
 *
 * - Creating windows on a specific monitor is not supported.
 *
 * - Creating hidden windows is not supported. It will be ignored.
 *
 * @since Added in version 2.0
 */
PAL_API PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** window);

/**
 * @brief Destroys the window.
 *
 * This only destroys windows created by PAL.
 *
 * @param[in] window The window.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palCreateWindow
 */
PAL_API void PAL_CALL palDestroyWindow(PalWindow* window);

/**
 * @brief Minimizes a maximized or restored window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported 
 * otherwise undefined behavior.
 * 
 * If the window is already minimized, this functions does nothing.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palMaximizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMinimizeWindow(PalWindow* window);

/**
 * @brief Maximizes a minimized or restored window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported 
 * otherwise undefined behavior.
 * 
 * If the window is already maximized, this functions does nothing.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palMinimizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMaximizeWindow(PalWindow* window);

/**
 * @brief Restores a window to it previous state.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STATE` must be supported
 * otherwise undefined behavior.
 * 
 * If the window is already restored, this functions does nothing.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @note Wayland does not support restoring a minimized window.
 *
 * @since Added in version 2.0
 * @sa palMinimizeWindow
 * @sa palMaximizeWindow
 */
PAL_API void PAL_CALL palRestoreWindow(PalWindow* window);

/**
 * @brief Shows the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY` must be supported
 * otherwise undefined behavior.
 * 
 * All windows are created hidden if not explicitly shown.
 * This does nothing if the window is already shown.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palHideWindow
 */
PAL_API void PAL_CALL palShowWindow(PalWindow* window);

/**
 * @brief Hides the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY` must be supported
 * otherwise undefined behavior.
 * 
 * This does nothing if the window is already hidden.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palShowWindow
 */
PAL_API void PAL_CALL palHideWindow(PalWindow* window);

/**
 * @brief Requests the platform (OS) to visually flash the window.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] info Information about how to flash the window.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

/**
 * @brief Attachs a foreign or native window to PAL video system.
 *
 * `PAL_VIDEO_FEATURE_FOREIGN_WINDOWS` must be supported 
 * otherwise undefined behavior.
 *
 * This function registers the window with PAL video system so it
 * can manage events and use its functionality/API.
 *
 * PAL does not own the window, it just sends events to that window.
 * Users are responsible for destroying the window when no longer needed.
 * `palDestroyWindow()` does not destroy the foreign or native window.
 *
 * Use Case:
 *
 * PAL takes your native foreign or native window and gives you a PalWindow
 * which can be used with all of PAL API. The native window must be valid
 * till the PalWindow has been detached with `palDetachWindow()`.
 *
 * The window must be created with the same instance or display
 * that PAL uses. see `palGetInstance()`.
 *
 * @param[in] windowHandle The foreign or native window. `Must not be nullptr`.
 * @param[out] window The output handle to recieve the attached window.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetInstance
 * @sa palDestroyWindow
 * @sa palDetachWindow
 */
PAL_API PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** window);

/**
 * @brief Detachs a foreign or native window from PAL video system.
 *
 * `PAL_VIDEO_FEATURE_FOREIGN_WINDOWS` must be supported
 * otherwise undefined behavior.
 *
 * This function unregisters the window from PAL video system.
 * The window must not be owned by PAL otherwise the function fails
 * and sets the result code to `PAL_RESULT_CODE_INVALID_HANDLE`.
 *
 * Detaching the window does not destroy the window,
 * therefore destroying the window is the users responsibility.
 *
 * Use Case:
 *
 * Give back the PalWindow returned at `palAttachWindow()`
 * and optionally get back your native window.
 *
 * @param[in] window The window. `Must not be nullptr`.
 * @param[out] windowHandle The output handle to recieve the foreign
 * or native window. Can be `nullptr`.
 *
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 * failure. Call `palFormatResult()` to get the string representation of
 * the result value.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palAttachWindow
 */
PAL_API PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** windowHandle);

/**
 * @brief Gets the style of the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_STYLE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] style The output to recieve the window style.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetWindowStyle
 */
PAL_API void PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* style);

/**
 * @brief Gets the monitor the window is currently on.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] monitor The output handle to recieve the monitor.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** monitor);

/**
 * @brief Gets the title of the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_TITLE` must be supported
 * otherwise undefined behavior.
 *
 * Set the buffer to `nullptr` to get the size of the window name in bytes.
 * If the size of the provided buffer is less than the actual size of window
 * title, PAL will write upto that limit.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] bufferSize The size of `buffer` in bytes.
 * @param[out] size The actual size of the window title in bytes.
 * @param[out] buffer The output buffer to write to.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetWindowTitle
 */
PAL_API void PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* size,
    char* buffer);

/**
 * @brief Gets the position of the window in pixels.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_POS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] x The output to recieve the window x position in pixels.
 * @param[out] y The output to recieve the window y position in pixels.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetWindowPos
 */
PAL_API void PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/**
 * @brief Gets the size of the window in pixels.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_SIZE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] width The output to recieve the window width in pixels.
 * @param[out] height The output to recieve the window height in pixels.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palSetWindowSize
 */
PAL_API void PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height);

/**
 * @brief Gets the state of the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_STATE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] state The output to recieve the window state.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* state);

/**
 * @brief Checks if the window is visible.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @return `PAL_TRUE` if the window is visible otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 */
PAL_API PalBool PAL_CALL palIsWindowVisible(PalWindow* window);

/**
 * @brief Gets the current input-focused window per application.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS` must be supported
 * otherwise undefined behavior.
 *
 * @return The current input-focused window on success or `nullptr` on
 * failure.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API PalWindow* PAL_CALL palGetFocusWindow();

/**
 * @brief Gets the native handle of the window.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[out] info The output struct to recieve the window handle info.
 *
 * @Thread-safety Thread-safe.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palGetWindowHandleInfo(
    PalWindow* window,
    PalWindowHandleInfo* info);

/**
 * @brief Sets the opacity of the window.
 *
 * `PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW` must be supported
 * otherwise undefined behavior.
 * 
 * The window must have `PAL_WINDOW_STYLE_TRANSPARENT` style.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] opacity Must be in the range 0.0 - 1.0.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 */
PAL_API void PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity);

/**
 * @brief Sets the style of the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_STYLE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] style The style.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetWindowStyle
 */
PAL_API void PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style);

/**
 * @brief Sets the title of the provided window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_TITLE` must be supported 
 * otherwise undefined behavior.
 * 
 * The title must be a `UTF-8` encoding null terminated string.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] title The title.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetWindowTitle
 */
PAL_API void PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title);

/**
 * @brief Sets the position of the window in pixels.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_POS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] x The new x coordinate in pixels.
 * @param[in] y The new y coordinate in pixels.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetWindowPos
 */
PAL_API void PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y);

/**
 * @brief Sets the size of the window in pixels.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_SIZE` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 * @param[in] width The new width of the window in pixels. 
 * Must be greater than `0`.
 * @param[in] height The new height of the window in pixels. 
 * Must be greater than `0`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetWindowSize
 */
PAL_API void PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height);

/**
 * @brief Requests input focus for the window.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be `nullptr`.
 *
 * @Thread-safety Must be called from the main thread.
 *
 * @since Added in version 2.0
 * @sa palGetFocusWindow
 */
PAL_API void PAL_CALL palSetFocusWindow(PalWindow* window);

#endif // PAL_VIDEO_WINDOW_H