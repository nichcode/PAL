
#include "PAL/PAL_defines.h"

#include "PAL.cpp"
#include "PAL_allocator.cpp"
#include "PAL_error.cpp"
#include "PAL_platform.cpp"
#include "PAL_log.cpp"

// windows specific files
#if PAL_PLATFORM_WINDOWS
#include "win32/PAL_win32platform.cpp"
#endif // PAL_PLATFORM_WINDOWS