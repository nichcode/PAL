
# PAL (Platform Abstraction Layer)

## Overview
PAL is a lightweight, low-level, modular, explicit, cross-platform abstraction layer for system functionality (video, input, threading, etc.).
It is designed to be **explicit**, and as close to the OS as possible — similar in philosophy to Vulkan. PAL is such that, querying state with PAL on a window should be the same as querying the state with the direct OS call. PAL is like a helper not an abstraction hidding the OS. PAL and the user stores and gets the state directly from the OS.

PAL currently supports only Windows. But Linux, Andriod and MacOS support will be added down the line.

- Minimal overhead (close to raw OS calls)  
- Explicit API (no hidden behavior or magic defaults)  
- Written in C for easy integration  
- Extendable: users can plug in their own backends (e.g. event queue, allocator)
- Stateless: Objects like windows, threads and monitors are represented by opaque handles. You choose when to query or cache their state. PAL does not do internal caching, all calls goes to the underlying Os.
- No lowest common denominator: PAL does not “dumb down” APIs to the weakest platform. Instead, it exposes capabilities explicitly, and you decide what to use. PAL exposes functionality and capabilities as features that must be used to ensure correctness across platforms.
- Modular: PAL opengl system can be used with a window from another library. (eg. SDL, GLFW).
- Explicit Build: PAL supports modular builds. You can build only the video system and PAL will build and work.

## Building PAL
PAL written in C99. PAL does not need additional dependencies aside the ones the OS provides. You only need to build with atleast C99. PAL supports from Windows Vista and only Mingw64 is currently supported.

**Prerequisites for Windows:**
- Make for Windows

Get all the prerequisites for your plaform (Windows Only). If using git, clone the repository with `git clone https://github.com/nichcode/PAL or download the zip file from github.

PAL supports modular builds and uses premake as its build system. check `pal_config.lua` at the root directory to choose which systems to build.
To build PAL tests, set `PAL_BUILD_TESTS = true` from the build config file.

After building, if you decide to use use a system you did not build, you need to rebuild. check `include/pal/pal_config.h` at the root directory to see the reflection of the systems choosen.

Run `premake\premake5.exe gmake2` from the root direcory to generate the projects. This uses the config file (above) to choose the systems to build.

There are two build types available, Debug and Release. Debug includes debug symbols and is optimal for development and exploration, while Release is ideal for performance.

To build PAL in debug, run `make all config=debug` at the root directory of PAL.
To build PAL in release, run `make all config=release` at the root directory of PAL.

## Getting Started
**A simple window creation API with the video system. See window test from the tests directory to get the full code.**

```c
#include "pal/pal_video.h"

int main (int argc, char** argv) {

    palInitVideo();
    palCreateWindow();

    while (running) {
        palUpdateVideo();
    }

    palDestroyWindow();
    palShutdownVideo();

    return 0;
}
```

## Supported systems:
- Core system (allocators, loggers etc.)
- Video system (windows, monitors, icons,   cursors, keyboard and mouse input)
- Event system (event queue, event callback, user events)
- Thread system (threads, mutex, condition varribale, TLS)
- Opengl system (Framebuffer configs, context)

## Planned systems:
- Graphics system (Vulkan, D3D12, Metal, Custom backend)
- Networking
- Audio System
- Filesystem
- HID system