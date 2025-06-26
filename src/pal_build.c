
#include "pal_internal.c"
#include "pal_log.c"
#include "pal_memory.c"
#include "pal_system.c"
#include "pal_video.c"
#include "pal_events.c"

#ifdef _WIN32
 #include "win32/pal_win32platform.c"
 #include "win32/pal_win32window.c"
 #include "win32/pal_win32events.c"
#endif // _WIN32