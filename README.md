# PAL (Platform Abstraction Layer)

![License: Zlib](https://img.shields.io/badge/License-Zlib-blue.svg)
![Language: C99](https://img.shields.io/badge/language-C99-green.svg)

## Overview

PAL is a lightweight, low-level, cross-platform abstraction layer in **C**, designed to be **explicit** and as close to the **OS** as possible — similar in philosophy to Vulkan. It gives you precise control without hidden behavior, making it ideal for developers who want performance and predictability.

PAL is transparent. All queries — window size, position, monitor info, and more — reflect the current platform state. Using PAL is like working directly with the OS: it applies no hidden logic, makes no assumptions, and leaves behavior fully in your control.

This approach gives you total control: you handle events, manage resources, and cache state explicitly. PAL provides the building blocks; how you use them — whether for simple applications or advanced frameworks — is entirely up to you.

Example – Get Window Size
```c
// Direct query from the platform — not cached by PAL
palGetWindowSize(window, &w, &h);
```
> Note: palGetWindowSize queries the OS directly. If your application needs continuous updates (e.g., window moves or resizes frequently), it is more efficient to listen to PAL events rather than repeatedly querying the OS. This ensures your app stays performant.

---

## Why PAL?

Other libraries like SDL or GLFW provide high-level abstractions but at the cost of overhead, implicit behavior, and limited control. **PAL is different:**

- ✅ **Explicit**: You decide how memory, events, and handles are managed.
- ✅ **Low Overhead**: PAL is close to raw OS calls, ensuring performance.
- ✅ **Modular**: Pick only the subsystems you need (video, event, threading, OpenGL, etc.).
- ✅ **Extendable**: Plug in your own backends (event queue, allocator, etc.).
- ✅ **Transparent**: Exposes raw OS handles when you need them.

---

## Quick Start

Here’s the smallest program that opens a PAL window:

```c
#include "pal/pal_video.h"

int main() {
    PalEventDriver* driver = nullptr;
    PalEventDriverCreateInfo info = {0};
    palCreateEventDriver(&info, &driver);

    palInitVideo(nullptr, driver);

    PalWindow* window = nullptr;
    PalWindowCreateInfo w = {0};
    w.width = 640; 
    w.height = 480; 
    w.title = "Hello PAL";
    w.show = true;
    palCreateWindow(&w, &window);

    while (1) {
        palUpdateVideo();
        PalEvent e;
        while (palPollEvent(driver, &e)) {
            if (e.type == PAL_EVENT_WINDOW_CLOSE) return 0;
        }
    }
}
```

➡️ Build and run this, and you’ll get a cross-platform window managed entirely by PAL.

For more detailed examples, see the [tests folder](./tests) tests folder, which contains full usage scenarios and validation cases.

---

## Philosophy
- PAL is a thin layer over the OS, not a framework or library.
- Queries return the current platform state, reflecting any changes made through direct OS calls.
- Developers are responsible for state tracking, caching, and event handling.
- PAL enables cross-platform consistency while preserving full OS behavior and control.
- Advanced users can build libraries or frameworks on top of PAL.
- Minimal overhead (close to raw OS calls)  
- Explicit API (no hidden behavior or defaults)  
- Event system supporting both polling and callbacks  
- Written in C for easy integration  
- Stateless: Opaque handles, no internal caching  
- No lowest common denominator: exposes platform capabilities directly  
- Modular builds: include only the subsystems you need

---

## Supported Platforms
- Windows (Vista+)

## Planned Platforms
- Linux (X11/Wayland)
- macOS (Cocoa)
- Android
- iOS

## Dependencies
- Standard C library
- Platform SDKs (Win32, X11, Cocoa, etc.)
- [Make for Windows](https://www.gnu.org/software/make/) (if not using Visual Studio)

## Compilers
- GCC
- Clang
- MSVC

---

## Build

PAL is written in **C99** and uses **Premake** as its build system. Configure modules via [pal_config.lua](./pal_config.lua).  
See [pal_config.h](./include/pal/pal_config.h) to see the reflection of modules that will be built.

**Windows**
```bash
premake\premake5.exe gmake2        # generate Makefiles (default: GCC)
premake\premake5.exe gmake2 --compiler=clang

premake\premake5.exe vs2022        # generate Visual Studio project (default: MSVC)
premake\premake5.exe vs2022 --compiler=clang
```

**Linux**
```bash
premake\premake5.exe gmake2        # generate Makefiles (default: GCC)
premake\premake5.exe gmake2 --compiler=clang
```

Enable tests in `pal_config.lua` by setting `PAL_BUILD_TESTS = true`.

---

## Modules

- `pal_core` - memory, log, time, version
- `pal_video` - windows, monitors, mouse, keyboard
- `pal_event` - event queue, event callback
- `pal_thread` - threads, synchronization
- `pal_opengl` - framebuffer configs, context

### Planned Modules
- `pal_graphics` - Vulkan, D3D12, Metal, Custom
- `pal_network`
- `pal_audio`
- `pal_hid`
- `pal_filesystem`

---

## Documentation

PAL uses [Doxygen](https://www.doxygen.nl/) for generating API documentation.

```bash
cd docs
doxygen doxyfile
```

The generated HTML docs will be available in `docs/html/`.

---

## Contributing

Contributions are welcome! Please open an issue or pull request.

---

## License

PAL is released under the [Zlib License](https://opensource.org/licenses/Zlib).
