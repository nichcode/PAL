
## Introduction

PAL is a lightweight, low-level, explicit cross-platform abstraction layer 
in C over platform and graphics APIs. PAL is stateless and transparent. 
Queries return current state, reflecting changes made through native API calls.

The goal of PAL is very simple, write your engine, framework or application 
on top of PAL once with full flexibility. If a feature is required and PAL does
not support it, the native handles (eg. `ID3D12Device` or `VkDevice`) can be 
accessed and used directly. The natve handles can be used directly with
the API of PAL.

PAL is a capability-first system. This means it does not make arbitrary 
decisions for the application. PAL exposes the capability and information of 
the underlying APIs, and you decide on how to use them. Example, if an adapter 
has multiple queues which some can share resources 
(which removes the need for transfer ownersip), PAL does not decide which 
queues are "best" for your application. That decision belongs to the user.

PAL supports Windows, Linux, Vulkan and D3D12. Both Wayland and X11 are 
supported on Linux. PAL supports both callback and poll event dispatch mode, 
custom allocators, custom loggers and custom graphics backends.

PAL is released under the [Zlib License](https://opensource.org/licenses/Zlib).