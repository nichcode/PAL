
#include "PAL/PAL_defines.h"

#include "PAL_error.cpp"
#include "PAL_log.cpp"
#include "PAL.cpp"

#ifdef PAL_PLATFORM_WINDOWS
#include "win32/PAL_win32_platform.cpp"
#include "win32/PAL_win32_window.cpp"
#endif // PAL_PLATFORM_WINDOWS