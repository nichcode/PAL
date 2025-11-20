# Contributing to PAL

Thank you for your interest in contributing to **PAL**.

Everyone is welcome to contribute to PAL by submitting bug reports, 
bug fixes, improving documentation, adding tests examples, telling others about PAL,
giving PAL a star, adding a new backend, etc.

### Reporting Bugs

Make sure the bug is not an API usage issue and you have the lastest version of PAL.
If the above is not the case, report the bug on our [GitHub Issue Tracker](https://github.com/nichcode/PAL/issues) using the bug report template. Please make sure you write a good bug report.

## Requesting Features

Requesting a feature which does not align with the goals of PAL (explicit, low-level) will likey
not be merge into PAL. This approach keeps project focused and consistent.

Request features on our [GitHub Issue Tracker](https://github.com/nichcode/PAL/issues) 
using the feature report template. Please explain into detail why your feature will work and if 
possibly usages of it in use.

## Coding Convention

- **C99** for C source.

- Naming convention:
  - `lowerCamelCase` for functions and function parameters. (e.g. `palCreateWindow`, `windowHandle`).
  - `PascalCase` for public types (e.g. `PalResult`).
  - static internal variables uses 's_' prefix. (eg. `s_InternalData`).
  - `snakeCase` for public and internal macros. (e.g. `PAL_DEFAULT_ALIGNMENT`).


## Contributing code

**PAL is released under the Zlib License and every 
code contributed to PAL must agree to Zlib licensing terms.**

Pull request checklist:

- Ensure it compiles on **all** supported platforms.
- Ensure the PR is focused and changes are relevant to the feature or bug fix.
- Ensure your code is formatted with clang-format using the `clang-format` file in the repo.