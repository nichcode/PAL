
<!-- ===========================================================-->
<!-- PAL V2.2.0 -->
<!-- =========================================================== -->

### Features

// TODO: add API to get native handles

- Added `PAL_DEFAULT_QUEUE_EVENT_COUNT`.
- Improved API documentation.
- Fixed Thread safety documentation.
- Improved Vulkan backend debugging.
- Improved D3D12 backend debugging.
- [Wayland] Fixed a bug where invalid monitor handles were returned sometimes. (#8)
- [X11] Fixed a bug where invalid monitor handles were returned sometimes. (#8)
- [Win32] Fixed a bug where invalid monitor handles were returned sometimes. (#8)

<!-- ===========================================================-->
<!-- PAL V2.1.0 -->
<!-- =========================================================== -->

## 2.1.0

### Features

- Added `palCmdImageOwnershipTransfer()` to transfer image ownership between two queues. (#7)
- Added `palCmdBufferOwnershipTransfer()` to transfer buffer ownership between two queues. (#7)
- Added `palCanQueueShareOwnership()` to check if two queues can share resource ownership. (#7)
- Added `palCanQueueUseUsageState()` to check if a queue can use a usage state. (#7)
- Added `palCanQueueUsePipelineStages()` to check if a queue can use a pipeline stage(s). (#7)
- Added `PAL_GRAPHICS_BACKEND_VTABLE_VERSION_2` constant to extend the graphics backend vtable. (#7)
- Added struct `PalGraphicsBackendVtable2`. (#7)
- Fixed D3D12 backend queue bug where queues destroyed were not added back to the pool. (#7)
- Fixed `palFormatResult()` bug where results without native code were not formatting properly. (#7)
- Improved D3D12 backend debugging. (#7)

<!-- ===========================================================-->
<!-- PAL V2.0.1 -->
<!-- =========================================================== -->

## 2.0.1
- Fixed a bug where an internal backend failing to initialize also caused the graphics system to fail. (#5)

<!-- ===========================================================-->
<!-- PAL V2.0.0 -->
<!-- =========================================================== -->

## 2.0.0

### Features

- Added a graphics system API (`pal2/pal_graphics.h`). (#4)
- Added `palGetResultCode()` to get the result code from a result value. (#4)
- Added `palGetResultSource()` to get the result source from a result value. (#4)
- Added `palGetResultNativeCode()` to get the result native code from a result value. (#4)
- Added `palGetSupportedGLAPIs()` to check supported opengl api types. (#4)
- Added type `PalResultCode` with values: (#4)
  - `PAL_RESULT_CODE_INVALID_ARGUMENT`
  - `PAL_RESULT_CODE_OUT_OF_MEMORY`
  - `PAL_RESULT_CODE_PLATFORM_FAILURE`
  - `PAL_RESULT_CODE_TIMEOUT`
  - `PAL_RESULT_CODE_INVALID_HANDLE`
  - `PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED`
  - `PAL_RESULT_CODE_INVALID_OPERATION`
  - `PAL_RESULT_CODE_DEVICE_LOST`
  - `PAL_RESULT_CODE_OUT_OF_DATE`
- Added type `PalResultSource` with values: (#4)
  - `PAL_RESULT_SOURCE_NONE`
  - `PAL_RESULT_SOURCE_WIN32`
  - `PAL_RESULT_SOURCE_POSIX`
  - `PAL_RESULT_SOURCE_EGL`
  - `PAL_RESULT_SOURCE_VULKAN`
  - `PAL_RESULT_SOURCE_D3D12`
  - `PAL_RESULT_SOURCE_METAL`
- Added type `PalGLBackend` with values: (#4)
  - `PAL_GL_BACKEND_EGL`
  - `PAL_GL_BACKEND_GLX`
  - `PAL_GL_BACKEND_WGL`
- Added type `PalGLAPI` with values: (#4)
  - `PAL_GL_API_OPENGL`
  - `PAL_GL_API_OPENGL_ES`
- Added `_COUNT` constants to all type groups (eg. `PAL_EVENT_TYPE_COUNT`). (#4)
- Added `PAL_GL_GRAPHICS_CARD_NAME_SIZE`,`PAL_GL_VENDOR_NAME_SIZE` and `PAL_GL_VERSION_NAME_SIZE` constants. (#4)

### Changes

- Moved all public header files to `pal2` directory (eg. `pal2/pal_video.h`). (#4)
- `palGetVersion()` now returns `void` and takes a pointer to the struct. (#4)
- `palFormatResult()` now takes two additional parameters (#4)
- Converted all enum types to fixed-width integer types and their values to standalone constants (eg. `PalResult` to `uint64_t`). (#4)
- Removed all previous `PalResult` values except: `PAL_RESULT_SUCCESS` (#4)
- Removed `palGLSetInstance()` function. (#4)
- Removed `palGLGetBackend()` function. (#4)
- Removed `palGetVideoFeaturesEx()` function and `PalVideoFeatures64` enum. (#4)
- Removed `palGetWindowHandleInfoEx()` function and `PalWindowHandleInfoEX` struct. (#4)
- Removed `palGetRawMouseWheelDelta()` function. (#4)
- Removed `palSetPreferredInstance()` function. (#4)
- Removed `palSetFBConfig()` function. (#4)
- Removed `PAL_FBCONFIG_BACKEND_GLES`. (#4)
- `palInitGL()` now takes two additional parameters. (#4)
- `palEnumerateGLFBConfigs()` no longer takes the `glWindow` and `count` now as `uint32_t` (#4)
- `palInitVideo()` now takes an additional parameter. (#4)
- `palGetWindowHandleInfo()` now returns `PalResult` and takes a pointer to the struct. (#4)
- `PalGLInfo` now has `backend` and `api` fields. (#4)
- Renamed `nativeDisplay` to `nativeInstance` in `PalWindowHandleInfo`. (#4)
- Renamed `display` to `instance` in `PalGLWindow`. (#4)
- `PalWindowHandleInfo` now has `nativeHandle1`, `nativeHandle2` and `nativeHandle3` fields. (#4)
- `PalWindowCreateInfo` now has `state`, `appName`, `instanceName`, `fbConfigBackend` and `fbConfigIndex` fields. (#4)
- Removed `maximized` and `minimized` in `PalWindowCreateInfo`. (#4)
- Removed `UintXX` and `IntXX` types in favor of standard `uintXX_t` and `intXX_t`. (#4)
- Removed `_MAX` constants from all type groups (eg. `PAL_EVENT_MAX`). (#4)
- Replaced standard `bool` type and `true`/`false` constants with `PalBool` type and `PAL_TRUE`/`PAL_FALSE`. (#4)
- Renamed `PalGLRelease` to `PalGLReleaseBehavior`. (#4)
- Renamed `palGLGetProcAddress()` to `palGetGLProcAddress()`. (#4)
- Renamed event type constants from `PAL_EVENT_**` to `PAL_EVENT_TYPE_**`. (#4)
- Renamed dispatch mode constants from `PAL_DISPATCH_**` to `PAL_DISPATCH_MODE_**`. (#4)
- Renamed platform type constants from `PAL_PLATFORM_**` to `PAL_PLATFORM_TYPE_**`. (#4)
- Renamed platform api type constants from `PAL_PLATFORM_API_**` to `PAL_PLATFORM_API_TYPE_**`. (#4)
- Renamed cursor type constants from `PAL_CURSOR_**` to `PAL_CURSOR_TYPE_**`. (#4)
- Renamed flash flag constants from `PAL_FLASH_**` to `PAL_FLASH_FLAG_**`. (#4)
- Renamed fbConfig backend type constants from `PAL_FBCONFIG_BACKEND_**` to `PAL_FBCONFIG_BACKEND_**`. (#4)
- Renamed `PalFlashFlag` to `PalFlashFlags`. (#4)
- `palGetMouseDelta()` now takes `dx` and `dy` paramters as `float`. (#4)
- `palEnumerateMonitors()` now takes `count` paramter as `uint32_t`. (#4)
- `palEnumerateMonitorModes()` now takes `count` paramter as `uint32_t`. (#4)
- `palGetClosestGLFBConfig()` now takes `count` paramter as `uint32_t`. (#4)
- `palGetMouseWheelDelta()` now takes `dx` and `dy` paramters as `float`. (#4)
- `palJoinThread()` now takes `retval` paramters as `void**`. (#4)
- `palUnpackUint32()` now takes `data` paramters as `uint64_t`. (#4)
- `palUnpackInt32()` now takes `data` paramters as `uint64_t`. (#4)
- `palUnpackPointer()` now takes `data` paramters as `uint64_t`. (#4)
- `palUnpackFloat()` now takes `data` paramters as `uint64_t`. (#4)
- `palPackUint32()` now returns `uint64_t` instead of `int64_t`. (#4)
- `palPackInt32()` now returns `uint64_t` instead of `int64_t`. (#4)
- `palPackPointer()` now returns `uint64_t` instead of `int64_t`. (#4)
- `palPackFloat()` now returns `uint64_t` instead of `int64_t`. (#4)
- These function now returns `void` instead of `PalResult` and does not do runtime validation anymore: (#4)
  - `palGetPlatformInfo()`
  - `palGetCPUInfo()`
  - `palGetThreadName()`
  - `palGetPrimaryMonitor()`
  - `palGetMonitorInfo()`
  - `palEnumerateMonitorModes()`
  - `palGetCurrentMonitorMode()`
  - `palMinimizeWindow()`
  - `palMaximizeWindow()`
  - `palRestoreWindow()`
  - `palShowWindow()`
  - `palHideWindow()`
  - `palGetWindowStyle()`
  - `palGetWindowMonitor()`
  - `palGetWindowTitle()`
  - `palGetWindowPos()`
  - `palGetWindowSize()`
  - `palGetWindowState()`
  - `palGetWindowHandleInfo()`
  - `palSetWindowStyle()`
  - `palSetWindowTitle()`
  - `palSetWindowPos()`
  - `palSetWindowSize()`
  - `palSetFocusWindow()`
  - `palSetWindowIcon()`
  - `palClipCursor()`
  - `palGetCursorPos()`
  - `palSetCursorPos()`
  - `palSetWindowCursor()`
  - `palSetWindowOpacity()`
  - `palFlashWindow()`
  - `palSetSwapInterval()`

<!-- =========================================================== -->
<!-- PAL V1.3.0 -->
<!-- =========================================================== -->

## [1.3.0] - 2025-11-21

### Features
- **Video:** Added Wayland backend support
- **Video:** Added **palGetVideoFeaturesEx()** to check old and extended supported features.
- **Video:** Added **palGetWindowHandleInfoEx()** to get extended window handles.
- **Video:** Added **palGetRawMouseWheelDelta()** to get raw mouse wheel delta.
- **Video:** Added **PAL_FBCONFIG_BACKEND_GLES** to `PalFBConfigBackend` enum.
- **Video:** Added **palSetPreferredInstance()** to set the native instance or display PAL video should use rather than creating a new one.

- **Core:** Added **palPackFloat()** to combine two floats into a single int64_t integer.
- **Core:** Added **palUnpackFloat()** to retreive two floats from a single int64_t integer.

- **OpenGL:** Added **palGLSetInstance()** to set the native instance or display PAL opengl should use. This must be set before calling **palInitGL()**.
- **OpenGL:** Added **palGLGetBackend()** to get the opengl backend.

- **Event:** Added **PAL_EVENT_WINDOW_DECORATION_MODE** to `PalEventType` enum.
- **Event:** Added **PalDecorationMode** enum.

### Tests
- Added native integration example: demonstrating **Native API Integration with PAL API**. see 
**native_integration_test.c**.

- Added native instance example: demonstrating **Native Instance or Display Integration with PAL API**.
see **native_instance_test.c**.

- Added custom decoration example: demonstrating **Custom Window Decoration**.
see **custom_decoration_test.c**.

### Notes
- **No ABI changes** - existing code remains compatible.

- **OpenGL tests may fail** - The opengl system now needs to call **palGLSetInstance()**
to set the instance or display before initializing. Failure to do this fails.
This is a runtime behavior change. The tests are updated in the repo.

- **Pal mouse button event** - The `event.data` now packs both the button and
wayland seat serial (If on wayland). Existing code that reads as a single value
without unpacking will see different values. 
This is a runtime behavior change. The **input_window_test.c** has been updated in the repo.

- **palJoinThread()** - ABI remains unchanged but now takes the address 
of a pointer variable for the return value of the thread.
PAL internally reinterpreted into a pointer-to-pointer. This is for ABI stability.

Example – Join thread and get the return value
```c
void* retval;
palJoinThread(thread, &retval);
```

<!-- =========================================================== -->
<!-- PAL V1.2.0 -->
<!-- =========================================================== -->

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

<!-- =========================================================== -->
<!-- PAL V1.1.0 -->
<!-- =========================================================== -->

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

<!-- =========================================================== -->
<!-- PAL V1.0.1 -->
<!-- =========================================================== -->

## [1.0.1] - 2025-10-01

**Bugfix release** - improve C/C++ interop and build integration

### Fixed
- Added extern "C" guards to all exported functions so PAL can now be linked from both **C** and **C++** projects.
- Fixed a **condition variable** bug where the wrong thread could acquire the mutex first, causing intermittent locking issues. See **tests/condvar_test.c**
- Updated premake scripts to allow **PAL to be included as a submodule or directly in another workspace** if the build system is **premake**.

### Notes
- No API or ABI changes
- Safe upgrade from **v1.0** - just rebuild your project after updating.

<!-- =========================================================== -->
<!-- PAL V1.0.0 -->
<!-- =========================================================== -->

## [1.0.0] - 2025-09-27
- Initial stable release of PAL.