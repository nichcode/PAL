
# PAL (Prime Abstraction Layer)

[![Build](https://github.com/nichcode/PAL/actions/workflows/build.yml/badge.svg)](https://github.com/nichcode/PAL/actions/workflows/build.yml)
![License: Zlib](https://img.shields.io/badge/License-Zlib-blue.svg)
![Language: C99](https://img.shields.io/badge/language-C99-green.svg)

## Overview
PAL is a lightweight, low-level, explicit cross-platform abstraction layer in C over
platform and graphics APIs with support for modular builds and custom backends. 
PAL is stateless and transparent. Queries return current state, 
reflecting changes made through native API calls.

The goal of PAL is very simple, write your engine, framework or application on top
of PAL once with full flexibility. If a feature is required and PAL does not support it, the
native handles (eg. `ID3D12Device` or `VkDevice`) can be retrieved and used directly. The natve
handles can be used directly with the API of PAL.

PAL is a capability-first system. This means it does not make arbitrary decisions for the
application. PAL exposes the capability and information of the underlying APIs, and you
decide on how to use them. Example, if an adapter has multiple queues which some can share
resources (which removes the need for transfer ownersip), PAL does not decide which queues
are "best" for your application. That decision belongs to you.

PAL supports Windows, Linux, Vulkan and D3D12. Both Wayland and X11 are supported on Linux.
PAL supports both callback and poll event dispatch mode, custom allocators, custom loggers and custom graphics backends. 

PAL is released under the [Zlib License](https://opensource.org/licenses/Zlib).

## Building PAL
PAL is written in C99 and uses Premake as its build system. PAL supports Windows Vista and later. PAL can be built with GCC, Clang and MSVC. Build options are configure with [pal_config.lua](./pal_config.lua). `true` to enable or `false` to disable a build option. It is recommended to not disable `PAL_BUILD_ABI_DUMP` build option.

X11 needs XRandR (1.2+) and libXcursor. 

See below on how to generate project files for each compiler and toolset. PAL generates **.vscode** folder when generating GNU Make projects.

### Windows
GNU Make (GCC):
```bash
premake\premake5.exe gmake
```

GNU Make (Clang):
```bash
premake\premake5.exe gmake --compiler=clang
```

Visual Studio 2022 (MSVC)
```bash
premake\premake5.exe vs2022
```

Visual Studio 2022 (Clang)
```bash
premake\premake5.exe vs2022 --compiler=clang
```

Visual Studio 2026 (MSVC)
```bash
premake\premake5.exe vs2026
```

Visual Studio 2026 (Clang)
```bash
premake\premake5.exe vs2026 --compiler=clang
```

### Linux
GNU Make (GCC):
```bash
./premake/premake5 gmake
```

GNU Make (Clang):
```bash
./premake/premake5 gmake --compiler=clang
```

## Verify PAL ABI
If the ABI dump tool was enabled when geenrating the projects, build the project and run the command below to verify that your C99 compiler conforms to the PAL ABI. The command below use the release build. Replace **Release** with **Debug** if using debug build.

### Windows
```bash
cd bin
cd Release
abi-dump.exe --quick
```

### Linux
```bash
cd bin
cd Release
./abi-dump --quick
```

To view additional commands, run the abi dump tool with `--help`.

## Using PAL
See the [HTML Documentation](./docs/html/index.html) for the complete list of available functions.
See [Pal Samples](https://github.com/nichcode/PAL-Samples) repository for practical examples of the API. This is the best way to learn PAL.

## Documentation
PAL uses [Doxygen](https://www.doxygen.nl/) for generating API documentation.

```bash
cd docs
doxygen doxyfile
```

The generated HTML docs will be available in `docs/html/`.

## Contributing
Contributions are welcome! Please open an issue or pull request.  
See [CONTRIBUTING.md](./.github/CONTRIBUTING.md) for how and what to contribute.  
Thanks for contributing to PAL.
