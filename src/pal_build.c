
#include "pal_core.c"
#include "pal_events.c"
#include "pal_shared.c"
#include "pal_video.c"

#ifdef _WIN32
 #include "win32/pal_win32core.c"
 #include "win32/pal_win32events.c"
 #include "win32/pal_win32video.c"
#endif // _WIN32