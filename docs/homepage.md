
# Prime Abstraction Layer

PAL is a lightweight, low-level, explicit cross-platform abstraction layer in C over
platform and graphics APIs with support for modular builds and custom backends. 
PAL is stateless and transparent. Queries return current state, 
reflecting changes made through native API calls.

PAL supports Windows, Linux, Vulkan and D3D12. Both Wayland and X11 are supported on Linux.

PAL is released under the [Zlib License](https://opensource.org/licenses/Zlib).