# PAL (Prime Abstraction Layer)

![License: Zlib](https://img.shields.io/badge/License-Zlib-blue.svg)
![Language: C99](https://img.shields.io/badge/language-C99-green.svg)

## Overview
PAL is a lightweight, low-level, explicit cross-platform abstraction layer in C over
platform and graphics APIs with support for modular builds and custom backends. PAL is stateless and transparent. Queries return current state, reflecting changes made through native API calls.

PAL supports Windows and Linux. Both Wayland and X11 are supported on Linux.

PAL is released under the [Zlib License](https://opensource.org/licenses/Zlib).

## Building PAL
PAL is written in C99 and uses Premake as its build system. PAL supports Windows Vista and later. PAL can be built with GCC, Clang and MSVC. Configure build options with [pal_config.lua](./pal_config.lua). **true/false** to turn on and off a build option. [pal_config.h](./include/pal/pal_config.h) is the reflection of the systems that will be built. 

X11 needs XRandR (1.2+) and libXcursor. 

### Windows
```bash
premake\premake5.exe gmake # generate Makefiles for GCC
premake\premake5.exe gmake --compiler=clang # generate Makefiles for Clang

premake\premake5.exe vs2022        # generate Visual Studio 2022 project For MSVC
premake\premake5.exe vs2022 --compiler=clang # generate Visual Studio 2022 project For Clang

premake\premake5.exe vs2026        # generate Visual Studio 2026 project for MSVC
premake\premake5.exe vs2026 --compiler=clang # generate Visual Studio 2026 project For Clang
```

### Linux
```bash
./premake/premake5 gmake        # generate Makefiles for GCC
./premake/premake5 gmake --compiler=clang # generate Makefiles for Clang
```

## Documentation
PAL uses [Doxygen](https://www.doxygen.nl/) for generating API documentation.

```bash
cd docs
doxygen doxyfile
```

The generated HTML docs will be available in `docs/html/`.

## Contributing
Contributions are welcome! Please open an issue or pull request.  
See  [CONTRIBUTING.md](./.github/CONTRIBUTING.md) for how and what to contribute.  
Thanks for contributing to PAL.
