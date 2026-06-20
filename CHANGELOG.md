# CHANGELOG

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

## [1.3.0] - 2025-11-21

### Features
- **Video:** Added Wayland backend support
- **Video:** Added **palGetVideoFeaturesEx()** to check old and extended supported features.
- **Video:** Added **palGetWindowHandleInfoEx()** to get extended window handles.
- **Video:** Added **palGetRawMouseWheelDelta()** to get raw mouse wheel delta.
- **Video:** Added **PAL_CONFIG_BACKEND_GLES** to `PalFBConfigBackend` enum.
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

## [2.0.0] - 2026-01-00

### Core
Added
- `PalBool` type.
- `palGetResultCode()` to get the result code from a result value.
- `PAL_RESULT_SUCCESS` define.
- `PAL_RESULT_INVALID_ARGUMENT` define.
- `PAL_RESULT_OUT_OF_MEMORY` define.
- `PAL_RESULT_PLATFORM_FAILURE` define.
- `PAL_RESULT_TIMEOUT` define.
- `PAL_RESULT_INVALID_HANDLE` define.
- `PAL_RESULT_FEATURE_NOT_SUPPORTED` define.
- `PAL_RESULT_NOT_INITIALIZED` define.
- `PAL_RESULT_INVALID_OPERATION` define.
- `PAL_RESULT_DEVICE_LOST` define.
- `PAL_RESULT_OUT_OF_DATE` define.
- `PAL_RESULT_MAX` define.
- `PAL_TRUE` define.
- `PAL_FALSE` define.

Changed
- `PalResult` is now `uint64_t`.
- `Int8` is now `int8_t`.
- `Int16` is now `int16_t`.
- `Int32` is now `int32_t`.
- `Int64` is now `int64_t`.
- `IntPtr` is now `intptr_t`.
- `Uint8` is now `uint8_t`.
- `Uint16` is now `uint16_t`.
- `Uint32` is now `uint32_t`.
- `Uint64` is now `uint64_t`.
- `UintPtr` is now `uintptr_t`.
- `palFormatResult()` now takes two more additional parameters.
- `palGetVersion()` now returns `void` and takes an output parameter.

Removed
- `PalResult` enum.
- `Int8` type.
- `Int16` type.
- `Int32` type.
- `Int64` type.
- `IntPtr` type.
- `Uint8` type.
- `Uint16` type.
- `Uint32` type.
- `Uint64` type.
- `UintPtr` type.
- `PAL_RESULT_SUCCESS` enum value.
- `PAL_RESULT_INVALID_ARGUMENT` enum value.
- `PAL_RESULT_OUT_OF_MEMORY` enum value.
- `PAL_RESULT_PLATFORM_FAILURE` enum value.
- `PAL_RESULT_TIMEOUT` enum value.
- `PAL_RESULT_INVALID_OPERATION` enum value.
- `PAL_RESULT_NULL_POINTER` enum value.
- `PAL_RESULT_INVALID_ALLOCATOR` enum value.
- `PAL_RESULT_ACCESS_DENIED` enum value.
- `PAL_RESULT_INSUFFICIENT_BUFFER` enum value.
- `PAL_RESULT_INVALID_THREAD` enum value.
- `PAL_RESULT_THREAD_FEATURE_NOT_SUPPORTED` enum value.
- `PAL_RESULT_VIDEO_NOT_INITIALIZED` enum value.
- `PAL_RESULT_INVALID_MONITOR` enum value.
- `PAL_RESULT_INVALID_MONITOR_MODE` enum value.
- `PAL_RESULT_VIDEO_FEATURE_NOT_SUPPORTED` enum value.
- `PAL_RESULT_INVALID_KEYCODE` enum value.
- `PAL_RESULT_INVALID_SCANCODE` enum value.
- `PAL_RESULT_INVALID_MOUSE_BUTTON` enum value.
- `PAL_RESULT_GL_NOT_INITIALIZED` enum value.
- `PAL_RESULT_INVALID_GL_WINDOW` enum value.
- `PAL_RESULT_GL_EXTENSION_NOT_SUPPORTED` enum value.
- `PAL_RESULT_INVALID_GL_FBCONFIG` enum value.
- `PAL_RESULT_INVALID_GL_VERSION` enum value.
- `PAL_RESULT_INVALID_GL_PROFILE` enum value.
- `PAL_RESULT_INVALID_GL_CONTEXT` enum value.
- `PAL_RESULT_INVALID_FBCONFIG_BACKEND` enum value.

### Event
Added
- Added `PAL_EVENT_WINDOW_CLOSE` define.
- Added `PAL_EVENT_WINDOW_SIZE` define.
- Added `PAL_EVENT_WINDOW_MOVE` define.
- Added `PAL_EVENT_WINDOW_STATE` define.
- Added `PAL_EVENT_WINDOW_FOCUS` define.
- Added `PAL_EVENT_WINDOW_VISIBILITY` define.
- Added `PAL_EVENT_WINDOW_MODAL_BEGIN` define.
- Added `PAL_EVENT_WINDOW_MODAL_END` define.
- Added `PAL_EVENT_MONITOR_DPI_CHANGED` define.
- Added `PAL_EVENT_MONITOR_LIST_CHANGED` define.
- Added `PAL_EVENT_KEYDOWN` define.
- Added `PAL_EVENT_KEYREPEAT` define.
- Added `PAL_EVENT_KEYUP` define.
- Added `PAL_EVENT_MOUSE_BUTTONDOWN` define.
- Added `PAL_EVENT_MOUSE_BUTTONUP` define.
- Added `PAL_EVENT_MOUSE_MOVE` define.
- Added `PAL_EVENT_MOUSE_DELTA` define.
- Added `PAL_EVENT_MOUSE_WHEEL` define.
- Added `PAL_EVENT_USER` define.
- Added `PAL_EVENT_KEYCHAR` define.
- Added `PAL_EVENT_WINDOW_DECORATION_MODE` define.
- Added `PAL_EVENT_MAX` define.
- Added `PAL_DISPATCH_NONE` define.
- Added `PAL_DISPATCH_CALLBACK` define.
- Added `PAL_DISPATCH_POLL` define.
- Added `PAL_DISPATCH_MAX` define.
- Added `PAL_DECORATION_MODE_CLIENT_SIDE` define.
- Added `PAL_DECORATION_MODE_CLIENT_SIDE` define.

Changed
- `PalEventType` is now a `uint64_t`.

Removed
- `PalEventType` enum.
- `PAL_EVENT_WINDOW_CLOSE` enum value.
- `PAL_EVENT_WINDOW_SIZE` enum value.
- `PAL_EVENT_WINDOW_MOVE` enum value.
- `PAL_EVENT_WINDOW_STATE` enum value.
- `PAL_EVENT_WINDOW_FOCUS` enum value.
- `PAL_EVENT_WINDOW_VISIBILITY` enum value.
- `PAL_EVENT_WINDOW_MODAL_BEGIN` enum value.
- `PAL_EVENT_WINDOW_MODAL_END` enum value.
- `PAL_EVENT_MONITOR_DPI_CHANGED` enum value.
- `PAL_EVENT_MONITOR_LIST_CHANGED` enum value.
- `PAL_EVENT_KEYDOWN` enum value.
- `PAL_EVENT_KEYREPEAT` enum value.
- `PAL_EVENT_KEYUP` enum value.
- `PAL_EVENT_MOUSE_BUTTONDOWN` enum value.
- `PAL_EVENT_MOUSE_BUTTONUP` enum value.
- `PAL_EVENT_MOUSE_MOVE` enum value.
- `PAL_EVENT_MOUSE_DELTA` enum value.
- `PAL_EVENT_MOUSE_WHEEL` enum value.
- `PAL_EVENT_USER` enum value.
- `PAL_EVENT_KEYCHAR` enum value.
- `PAL_EVENT_WINDOW_DECORATION_MODE` enum value.
- `PAL_EVENT_MAX` enum value.
- `PAL_DISPATCH_NONE` enum value.
- `PAL_DISPATCH_CALLBACK` enum value.
- `PAL_DISPATCH_POLL` enum value.
- `PAL_DISPATCH_MAX` enum value.
- `PAL_DECORATION_MODE_CLIENT_SIDE` enum value.
- `PAL_DECORATION_MODE_CLIENT_SIDE` enum value.

### Opengl
Added

Changed

Removed

### Thread
Added

Changed

Removed

### Features
- **Opengl:** Added **palGetSupportedGLAPIs()**.

### Changed

- **Thread:** **palJoinThread()** now takes a void** for retval parameter.

- **Opengl:** **palEnumerateGLFBConfigs()** now does not take glWindow parameter anymore.
- **Opengl:** **palInitGL()** now takes an api and instance parameter.
- **Opengl:** rename **PalGLRelease** to **PalGLReleaseBehavior**.
- **Opengl:** rename **palGLGetProcAddress()** to **palGetGLProcAddress()**.

- **Video:** **palGetWindowHandleInfo()** now takes an info parameter.
- **Video:** **palGetMouseDelta()** now takes floats instead of uint32_t.
- **Video:** **palGetMouseWheelDelta()** now takes floats instead of uint32_t.
- **Video:** **palInitVideo()** now takes a preferredInstance parameter.
- **Video:** **PalWindowCreateInfo** now has `appName`, `instanceName`, `fbConfigBackend` and 
`fbConfigIndex` fields.

### Removed


- **Opengl:** Removed **palGLSetInstance** function.
- **Opengl:** Removed **palGLGetBackend** function.

- **Video:** Removed **palGetVideoFeaturesEx** function.
- **Video:** Removed **palGetWindowHandleInfoEx** function.
- **Video:** Removed **palGetRawMouseWheelDelta** function.
- **Video:** Removed **palSetPreferredInstance** function.
- **Video:** Removed **palSetFBConfig** function.

### Tests

- Added grapics example: see **graphics_test.c**
- Added clear color example: see **clear_color_test.c**
- Added vertex shader/buffer triangle example: see **triangle_test.c**
- Added mesh example: see **mesh_test.c**
- Added compute example: see **compute_test.c**
- Added ray tracing example: see **ray_tracing_test.c**
- Added texture rendering example: see **texture_test.c**

### Notes
- API or ABI changes
