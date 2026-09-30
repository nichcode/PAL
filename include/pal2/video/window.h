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

/**
 * @struct PalFlashInfo
 * @brief Information for flashing a window.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalFlashInfo {
    /** A bitmask of flash flags. All flags must be supported.*/
    PalFlashFlags flags;

    /** The flash interval in milliseconds. If the interval is greater
     * than `0`, PAL_VIDEO_FEATURE_WINDOW_FLASH_INTERVAL must be supported.
     */
    uint32_t interval;

    /** The number of times to flash. Set to `0` to flash until 
     * focused or cancelled.
     */
    uint32_t count;
} PalFlashInfo;

/**
 * @struct PalWindowHandleInfo
 * @brief Information about a window handle.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
typedef struct PalWindowHandleInfo {
    /** The platform (OS) display or instance of the window. 
     * Will always be valid. 
     */
    void* nativeInstance;

    /** The platform (OS) handle of the window. Will always be valid.*/
    void* nativeWindow;

    /** Additional handle of the window. Will be nullptr if there is no
     * additional window handle.
     */
    void* nativeHandle1;

    /** Additional handle of the window. Will be nullptr if there is no
     * additional window handle.
     */
    void* nativeHandle2;

    /** Additional handle of the window. Will be nullptr if there is no
     * additional window handle.
     */
    void* nativeHandle3;
} PalWindowHandleInfo;

/**
 * @struct PalWindowCreateInfo
 * @brief Creation parameters of a window.
 *
 * Uninitialized fields may result in undefined behavior.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palCreateWindow
 */
typedef struct PalWindowCreateInfo {
    /**The title of the window in UTF-8 encoding.*/
    const char* title;

    /** The monitor the window should be created on. Set to nullptr 
     * for windowed mode.
     */
    PalMonitor* monitor;

    /** The window app name in UTF-8 encoding. If nullptr, 
     * "PAL" will be used.
     */
    const char* appName;

    /** The window instance name in UTF-8 encoding. If nullptr,
     * ::title will be used.
     */
    const char* instanceName;

    /** The FBConfig backend of the index at ::fbConfigIndex.
     * This will be ignored if ::fbConfigIndex is `0`.
     */
    PalFBConfigBackend fbConfigBackend;

    /** This is the loop index in the backends framebuffer configurations.
     * ::fbConfigBackend tells PAL which backend produced the framebuffer
     * configuration loop index. Set to `0` to create the window without
     * framebuffer configuration or pixel format.
     */
    int32_t fbConfigIndex;

    /** The width of the window in screen coordinates.*/
    uint32_t width;

    /** The height of the window in screen coordinates.*/
    uint32_t height;

    /** `PAL_TRUE` to show the window after its created.*/
    PalBool show;

    /** A bitmask of window styles. All the flags must be supported.*/
    PalWindowStyle style;

    /** The initial window state. Set to PAL_WINDOW_STATE_NORMAL to create
     * the window in a windowed state.
     */
    PalWindowState state;

    /** `PAL_TRUE` to center the window after creation. This only works if
     * ::state is PAL_WINDOW_STATE_NORMAL.
     */
    PalBool center;
} PalWindowCreateInfo;

/**
 * @brief Creates a window.
 * 
 * The window width and height is restricted by the platform. Very small 
 * or large width and height might be overridden by the system or
 * the window will fail to create. PalWindowCreateInfo::width and
 * PalWindowCreateInfo::height will be ignored if PalWindowCreateInfo::monitor
 * is not nullptr.
 * 
 * PalWindowCreateInfo::state will be ignored if its set to 
 * @ref PAL_WINDOW_STATE_NORMAL and PalWindowCreateInfo::monitor 
 * is not nullptr.
 * 
 * This function creates the window and optionally sets the framebuffer 
 * configuration or pixel format if PalWindowCreateInfo::fbConfigIndex is
 * not set to `0`. This does not create a vulkan surface or an OpenGL
 * or OpenGL ES context.
 * 
 * To create a borderless fullscreen window, create the window with
 * @ref PAL_WINDOW_STYLE_BORDERLESS style and choose a monitor. Without
 * a monitor selected, the window will not be created in fullscreen.
 * 
 * To create an exclusive fullscreen window, create the window with the styles
 * you want and call @ref palSetMonitorMode on the monitor the window was
 * created on with your selected @ref PalMonitorMode.
 * Without a monitor selected, the window will not be created in fullscreen.
 * 
 * If any of the window styles set in PalWindowCreateInfo::styles is
 * not supported, this function fails and sets the result code of the
 * returned result value to @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
 * 
 * The created window must be destroyed with @ref palDestroyWindow when
 * no longer needed.
 *
 * @param[in] info Information about how to create the window.
 * @param[out] window The output handle to recieve the created window.
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 *         failure. Call @ref palFormatResult to get the string representation
 *         of the result value.
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
 * On `Win32`:
 * 
 * - Creating non resizable windows also removes the maximize box.
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
 * This only destroys windows created by PAL. If this function is used with
 * a foreign window, it will return silently.
 *
 * @param[in] window The window to destroy.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * PAL_VIDEO_FEATURE_WINDOW_SET_STATE must be supported 
 * otherwise undefined behavior. If the window is already minimized,
 * this functions does nothing.
 *
 * @param[in] window The window. Must not be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * PAL_VIDEO_FEATURE_WINDOW_SET_STATE must be supported 
 * otherwise undefined behavior. If the window is already maximized,
 * this functions does nothing.
 *
 * @param[in] window The window. Must not be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palMinimizeWindow
 * @sa palRestoreWindow
 */
PAL_API void PAL_CALL palMaximizeWindow(PalWindow* window);

/**
 * @brief Restores a window to it previous state.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_STATE must be supported
 * otherwise undefined behavior. If the window is already in the 
 * restored state this functions does nothing.
 *
 * @param[in] window The window. Must not be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY must be supported
 * otherwise undefined behavior.All windows are created hidden 
 * if not explicitly shown. This does nothing if the window 
 * is already shown.
 *
 * @param[in] window The window. Must not be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * PAL_VIDEO_FEATURE_WINDOW_SET_VISIBILITY must be supported
 * otherwise undefined behavior. This does nothing if the window 
 * is already hidden.
 *
 * @param[in] window The window. Must not be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 * 
 * @sa palShowWindow
 */
PAL_API void PAL_CALL palHideWindow(PalWindow* window);

/**
 * @brief Requests the platform (OS) to visually flash the window.
 * 
 * If any of the flash flags set in PalFlashInfo::flags is
 * not supported, behavior is undefined.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] info Information about how to flash the window.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

/**
 * @brief Attachs a foreign or native window to PAL video system.
 *
 * PAL_VIDEO_FEATURE_FOREIGN_WINDOWS must be supported 
 * otherwise this function fails and sets the result code
 * of the returned result value to @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
 *
 * This function registers the window with PAL video system so it
 * can manage events and use its functionality/API. PAL does not
 * own the window, users are responsible for destroying the window
 * when no longer needed. @ref palDestroyWindow does not destroy the
 * foreign or native window.
 * 
 * Use Case:
 *
 * PAL takes your native foreign or native window and gives you a 
 * @ref PalWindow which can be used with all of PAL API. The native 
 * window must be valid till the PalWindow has been detached with 
 * @ref palDetachWindow. PAL does not copied the handle.
 *
 * The window must be created with the same instance or display
 * that PAL uses. Call @ref palGetInstance to get the instance or
 * display PAL is using or set your instance or display with
 * @ref palInitVideo when initializing the video system.
 *
 * @param[in] windowHandle The foreign or native window. Must not be nullptr.
 * @param[out] window The output handle to recieve the attached window.
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 *         failure. Call @ref palFormatResult to get the string representation
 *         of the result value.
 *
 * @Thread-safety Must be called from the main thread.
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
 * @brief Detachs a foreign or native window from PAL video system.
 *
 * PAL_VIDEO_FEATURE_FOREIGN_WINDOWS must be supported 
 * otherwise this function fails and sets the result code
 * of the returned result value to @ref PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED.
 *
 * This function detachs the window from PAL video system.
 * The window must not be owned by PAL otherwise the function fails
 * and sets the result code of the returned result value to
 * @ref PAL_RESULT_CODE_INVALID_HANDLE.
 *
 * Detaching the window does not destroy it, therefore the user is responsible
 * for destroying the window.
 *
 * Use Case:
 *
 * Give back the PalWindow returned at @ref palAttachWindow and get
 * back your native window.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] windowHandle The output handle to recieve the foreign
 *                          or native window. Can be nullptr.
 * @return `PAL_RESULT_SUCCESS` on success or an appropriate result value on
 *         failure. Call @ref palFormatResult to get the string representation
 *         of the result value.
 *
 * @Thread-safety Must be called from the main thread.
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
 * @brief Gets the current style of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_STYLE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] style The output to recieve the window style.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * PAL_VIDEO_FEATURE_WINDOW_GET_MONITOR must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] monitor The output handle to recieve the monitor.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * PAL_VIDEO_FEATURE_WINDOW_GET_TITLE must be supported
 * otherwise undefined behavior.
 *
 * Set the buffer to nullptr to get the size of the window name in bytes.
 * If the size of the provided buffer is less than the actual size of window
 * title, PAL will write upto that limit.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] bufferSize The size of the buffer in bytes.
 * @param[out] size The actual size of the window title in bytes.
 * @param[out] buffer The output buffer to write to.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * @brief Gets the position of the window in screen coordinates.
 *
 * `PAL_VIDEO_FEATURE_WINDOW_GET_POS` must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] x The output to recieve the window x position in
 *               screen coordinates. Can be nullptr.
 * @param[out] y The output to recieve the window y position in
 *               screen coordinates. Can be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * @brief Gets the size of the window in screen coordinates.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_SIZE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] width The output to recieve the window width in
 * screen coordinates. Can be nullptr.
 * @param[out] height The output to recieve the window height in
 * screen coordinates. Can be nullptr.
 *
 * @Thread-safety Must only be called from the main thread.
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
 * @brief Gets the state of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_STATE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] state The output to recieve the window state.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* state);

/**
 * @brief Checks if the window is visible.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_VISIBILITY must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @return `PAL_TRUE` if the window is visible otherwise `PAL_FALSE`.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API PalBool PAL_CALL palIsWindowVisible(PalWindow* window);

/**
 * @brief Gets the current input-focused window per application.
 *
 * PAL_VIDEO_FEATURE_WINDOW_GET_INPUT_FOCUS must be supported
 * otherwise undefined behavior. This function returns the keyboard
 * or mouse input-focused window.
 *
 * @return The current input-focused window on success or nullptr on
 * failure.
 *
 * @Thread-safety Must only be called from the main thread.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API PalWindow* PAL_CALL palGetFocusWindow();

/**
 * @brief Gets the native handle of the window.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[out] info The output struct to recieve the window handle info.
 *
 * @Thread-safety Thread-safe.
 * 
 * @note On `Wayland`:
 * 
 * - PalWindowHandleInfo::nativeHandle1 is set to the `xdg_surface`.
 * 
 * - PalWindowHandleInfo::nativeHandle1 is set to the `xdg_toplevel`.
 * 
 * - PalWindowHandleInfo::nativeHandle1 is set to the `wl_egl_window`.
 *
 * @since Added in version 2.0
 * @ingroup pal_video
 */
PAL_API void PAL_CALL palGetWindowHandleInfo(
    PalWindow* window,
    PalWindowHandleInfo* info);

/**
 * @brief Sets the opacity of the window.
 *
 * PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW must be supported
 * otherwise undefined behavior. The window must have be
 * have been created with @ref PAL_WINDOW_STYLE_TRANSPARENT style.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] opacity Must be in the range `0.0 - 1.0`.
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
 * @brief Sets the style of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_STYLE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] style The window style.
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
 * @brief Sets the title of the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_TITLE must be supported 
 * otherwise undefined behavior. The title must be a UTF-8 encoding
 * null terminated string.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] title The window title.
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
 * @brief Sets the position of the window in screen coordinates.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_POS must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] x The new x coordinate in screen coordinates.
 * @param[in] y The new y coordinate in screen coordinates.
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
 * @brief Sets the size of the window in screen coordinates.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_SIZE must be supported
 * otherwise undefined behavior.
 *
 * @param[in] window The window. Must not be nullptr.
 * @param[in] width The new width of the window in screen coordinates. 
 *                  Must be greater than `0`.
 * @param[in] height The new height of the window in screen coordinates. 
 *                   Must be greater than `0`.
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
 * @brief Requests input focus for the window.
 *
 * PAL_VIDEO_FEATURE_WINDOW_SET_INPUT_FOCUS must be supported
 * otherwise undefined behavior. The window must be visible.
 *
 * @param[in] window The window. Must not be nullptr.
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