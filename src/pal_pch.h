
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <stdlib.h>

#ifdef _WIN32
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif // WIN32_LEAN_AND_MEAN

 #ifndef NOMINMAX
  #define NOMINMAX
 #endif // NOMINMAX

 #include <windows.h>
 #include <windowsx.h>
 #include <winnt.h>
#endif // _WIN32