# Changelog

## [1.0.0] - 2025-09-27
- Initial stable release of PAL.

## [1.0.1] - 2025-10-01

**Bugfix release** - improve C/C++ interop and build integration

### Fixed
- Added extern "C" guards to all exported functions so PAL can now be linked from both **C** and **C++** projects.
- Fixed a **condition variable** bug where the wrong thread could acquire the mutex first, causing intermittent locking issues. See **tests/condvar_test.c**
- Updated premake scripts to allow **PAL to be included as a submodule or directly in another workspace** if the build system is **premake**.

### Notes
- No API or ABI changes
- Safe upgrade from **v1.0** - just rebuild your project after updating.

## [1.1.0] - 2025-10-17

### Features
- **Build:** Added Linux platform support across all modules.
- **Core:** Added Linux backend support.
- **Video:** Added X11-based backend support.
- **Thread:** Added Linux backend support.
- **Opengl:** Added Linux backend support.
- **System:** Added Linux backend support.
- **Video:** Added **palCreateCursorFrom()** to create system cursors.
- **Video:** Added **palSetFBConfig()** to select window FBConfig.
- **Video:** Added **PAL_VIDEO_FEATURE_WINDOW_SET_ICON** to `PalVideoFeatures` enum.
- **System:** Added **PAL_PLATFORM_API_COCOA** to `PalPlatformApiType` enum.
- **System:** Added **PAL_PLATFORM_API_ANDRIOD** to `PalPlatformApiType` enum.
- **System:** Added **PAL_PLATFORM_API_UIKIT** to `PalPlatformApiType` enum.
- **System:** Added **PAL_PLATFORM_API_HEADLESS** to `PalPlatformApiType` enum.
- **Core:** Added **PAL_RESULT_INVALID_FBCONFIG_BACKEND** to `PalResult` enum.

### Changed
- **System:** `PalCPUInfo.architecture` is now determined at runtime instead of build time.
- **Opengl:** **palEnumerateGLFBConfigs()** now does not use the `glWindow` paramter. Set to `nullptr`

### Fixed
- Fixed a bug where **enter modal mode and exit modal mode** operations triggered only one event.
- Fixed repeated window state event (**minimized**, **maximized**, **restore**).

### Notes
- No API or ABI changes - existing Windows code remains compatible.
- Linux video support currently targets **X11** only: **Wayland** is planned for future releases.
- Safe upgrade from **v1.0.1** - just rebuild your project after updating.

## [1.2.0] - 2025-10-22

### Features
- **Video:** Added **palGetInstance()** to retrieve the native display or instance handle.
- **Video:** Added **palAttachWindow()** for attaching **foreign windows** to PAL.
- **Video:** Added **palDetachWindow()** for detaching **foreign windows** from PAL.
- **Event:** Added **PAL_EVENT_KEYCHAR** to `PalEventType` enum.
- **Event:** Added documentation for event bits(payload) layout.

### Naming Update
- PAL now stands for **Prime Abstraction Layer**, 
reflecting its role as the primary explicit foundation for OS and graphics abstraction.
- All API remains unchanged — this is an identity update only.

### Tests
- Added multi-threaded OpenGL example: demonstrating **Multi-Threaded OpenGL Rendering**. see **multi_thread_opengl_test.c**.
- Added attaching and detach foreign windows example. see **attach_window_test.c**
- Added key character example. see **char_event_test.c**

### Notes
- No API or ABI changes - existing code remains compatible.
- Safe upgrade from **v1.1.0** - just rebuild your project after updating.

## [1.3.0] - 2025-11-

### Features
- **Video:** Added **palGetVideoFeaturesEx()** to check supported features.
- **Video:** Added **palGetWindowHandleInfoEx()** to get extended window handles.

### Tests

### Notes
- No API or ABI changes - existing code remains compatible.
- Safe upgrade from **v1.2.0** - just rebuild your project after updating.