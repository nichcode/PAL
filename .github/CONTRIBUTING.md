# Contributing to PAL

Everyone is welcome to contribute to PAL by submitting bug reports, bug fixes,
improving documentation, adding tests examples, telling others about PAL, 
giving PAL a star, adding a new backend, etc. 

### Reporting Bugs

Make sure the bug is not an API usage issue and you have the lastest version 
of PAL. If the above is not the case, report the bug on our 
[GitHub Issue Tracker](https://github.com/nichcode/PAL/issues) using the bug 
report template. Please make sure you write a good bug report.

## Requesting Features

Requesting a feature which does not align with the goals of PAL 
(explicit, low-level) will likey not be merge into PAL. This approach keeps
project focused and consistent.

[GitHub Issue Tracker](https://github.com/nichcode/PAL/issues) to request
features using the feature report template. Please explain into detail why your
feature will work and if possibly usages of it in use.

## Coding Convention
Most of the coding convention are enforced by .clang-format formatting. Some 
have to be manualy enforced since clang-format cannot be used to achieve them.
Another limitaton which must be enforced manually is the column width. 
Since its not enforced by `clang-format`, Its better if its `80` width.

1. Function calls that take a single argument or function declaration that 
takes a single paramter should be on a single line. If its a call, 
its better to do any error checking just below the function call.

```C
PalResult palCreateDevice(void* a);

result = palCreateBuffer(int a);
if (result != PAL_RESULT_SUCCESS) {

}
```

2. Function calls that take a multiple argument or function declaration that
takes multiple paramters should be one argument/paramter per line. 
If its a call, its better to do any error checking after a blank line.

```C
PalResult palCreateImage(
    void device,
    PalImage* outImage);

result = palCreateImageView(
    image,
    &outImage);

if (result != PAL_RESULT_SUCCESS) {

}
```

3. Logical code sections must be seperated by blank lines. There is no right
or wrong way, you decide. The code should be clean and make reading easy.

4. Initializers must be one per line which must end with a trailing comma. 
It influences how formatting is done.

```C
const char* backends[] = {
    "d3d12"
    "vulkan"
    "opengl"
    "metal",
};
```

5. `goto` must be used when several failure paths need common setup rather
than duplicating cleanup code all over. Please use descriptive labels such as
`cleanup`:, `cleanup_buffer` etc. If there is no need for several failure 
paths, a single check is better with a simple return.
Put a blank line before `goto` cleanup labels.

PAL uses these naming convention:
  - `lowerCamelCase` for functions and function parameters.
     (e.g. `palCreateWindow`, `windowHandle`).
  - `PascalCase` for public types (e.g. `PalResult`).
  - static internal variables uses 's_' prefix. (eg. `s_InternalData`).
  - `snakeCase` for public and internal macros. (e.g. `PAL_DEFAULT_ALIGNMENT`).
  - Single line function call


## Contributing code

PAL is released under the Zlib License and every 
code contributed to PAL must agree to Zlib licensing terms.

Pull request checklist:

- Ensure it compiles on **all** supported platforms.
- Ensure the PR is focused and changes are relevant to the feature or bug fix.
- Ensure your code is formatted with clang-format using the 
`clang-format` file in the repo. The minimum version is 14.
