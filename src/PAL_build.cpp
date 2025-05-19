
#include "PAL/PAL_defines.h"

#include "PAL_error.cpp"
#include "PAL_log.cpp"
#include "PAL_input.cpp"
#include "PAL_device.cpp"

#ifdef PAL_PLATFORM_WINDOWS
#include "win32/PAL_win32_platform.cpp"
#include "win32/PAL_win32_window.cpp"
#include "win32/PAL_wgl_context.cpp"
#include "opengl/PAL_glwin32context.cpp"
#endif // PAL_PLATFORM_WINDOWS

#include "opengl/PAL_glapi.cpp"
#include "opengl/PAL_glfuncs.cpp"