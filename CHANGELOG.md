
<!-- ===========================================================-->
<!-- PAL V2.0.0 -->
<!-- =========================================================== -->

## 2.0.0

### Features

- Added a graphics system API (`pal_graphics.h`).
- Added `palGetResultCode()` to get the result code from a result value.
- Added `palGetSupportedGLAPIs()` to check supported opengl api types.
- Added new `PalResult` values:
  - `PAL_RESULT_INVALID_HANDLE`
  - `PAL_RESULT_FEATURE_NOT_SUPPORTED`
  - `PAL_RESULT_NOT_INITIALIZED`
  - `PAL_RESULT_OUT_OF_DATE`
- Added type `PalGLBackend` with values:
  - `PAL_GL_BACKEND_EGL`
  - `PAL_GL_BACKEND_GLX`
  - `PAL_GL_BACKEND_WGL`
- Added type `PalGLAPI` with values:
  - `PAL_GL_API_OPENGL`
  - `PAL_GL_API_OPENGL_ES`
- Added `_COUNT` constants to all type groups (eg. `PAL_EVENT_TYPE_COUNT`).
- Added `PAL_GL_GRAPHICS_CARD_NAME_SIZE`,`PAL_GL_VENDOR_NAME_SIZE` and `PAL_GL_VERSION_NAME_SIZE` constants.

### Changes

- Converted all enum types to fixed-width integer types and their values to standalone constants (eg. `PalResult` to `uint64_t`).
- Removed all previous `PalResult` values except:
  - `PAL_RESULT_SUCCESS`
  - `PAL_RESULT_INVALID_ARGUMENT`
  - `PAL_RESULT_PLATFORM_FAILURE`
  - `PAL_RESULT_OUT_OF_MEMORY`
  - `PAL_RESULT_TIMEOUT`
  - `PAL_RESULT_INVALID_OPERATION`.
  - `PAL_RESULT_DEVICE_LOST`.
- Removed `UintXX` and `IntXX` types in favor of standard `uintXX_t` and `intXX_t`.
- Removed `_MAX` constants from all type groups (eg. `PAL_EVENT_MAX`).
- Replaced standard `bool` type and `true`/`false` constants with `PalBool` type and `PAL_TRUE`/`PAL_FALSE`.
- `palGetVersion()` now returns `void` and takes a pointer to the struct.
- `palFormatResult()` now takes two additional parameters.
- Renamed `PalGLRelease` to `PalGLReleaseBehavior`.
- Renamed `palGLGetProcAddress()` to `palGetGLProcAddress()`.
- Renamed event type constants from `PAL_EVENT_**` to `PAL_EVENT_TYPE_**`.
- Renamed dispatch mode constants from `PAL_DISPATCH_**` to `PAL_DISPATCH_MODE_**`.
- `palInitGL()` now takes two additional parameters.
- `palEnumerateGLFBConfigs()` no longer takes the `glWindow` parameter.
- `palInitVideo()` now takes an additional parameter.
- `palGetMouseDelta()` now takes `dx` and `dy` paramters as `float`.
- `palGetMouseWheelDelta()` now takes `dx` and `dy` paramters as `float`.
- `palJoinThread()` now takes `retval` paramters as `void**`.
- Removed `palGLSetInstance()` function.
- Removed `palGLGetBackend()` function.
- Removed `palGetVideoFeaturesEx()` function and `PalVideoFeatures64` enum.
- Removed `palGetWindowHandleInfoEx()` function and `PalWindowHandleInfoEX` struct.
- Removed `palGetRawMouseWheelDelta()` function.
- Removed `palSetPreferredInstance()` function.
- Removed `palSetFBConfig()` function.
- Removed `PAL_CONFIG_BACKEND_GLES`.
- `palGetWindowHandleInfo()` now returns `PalResult` and takes a pointer to the struct.
- `PalGLInfo` now has `backend` and `api` fields.
- Renamed `nativeDisplay` to `nativeInstance` in `PalWindowHandleInfo`.
- Renamed `display` to `instance` in `PalGLWindow`.
- `PalWindowHandleInfo` now has `nativeHandle1`, `nativeHandle2` and `nativeHandle3` fields.
- `PalWindowCreateInfo` now has `state`, `appName`, `instanceName`, `fbConfigBackend` and `fbConfigIndex` fields.
- Removed `maximized` and `minimized` in `PalWindowCreateInfo`.

<!-- =========================================================== -->
<!-- PAL V1.3.0 -->
<!-- =========================================================== -->

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