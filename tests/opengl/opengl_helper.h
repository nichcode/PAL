
#ifndef _OPENGL_HELPER_H
#define _OPENGL_HELPER_H

#include "tests.h"
#include "pal2/pal_opengl.h"

// clang-format off
static const char* g_BoolsToString[2] = {
    "False", 
    "True"};
// clang-format on

// opengl typedefs
typedef void(PAL_GL_APIENTRY* PFNGLCLEARCOLORPROC)(
    float red,
    float green,
    float blue,
    float alpha);

typedef void(PAL_GL_APIENTRY* PFNGLCLEARPROC)(uint32_t mask);

PalWindow* glHelperCreateWindow(
    const char* title,
    PalEventDriver* eventDriver,
    PalGLFBConfig* outFbConfig);

void glHelperDestroyWindow(
    PalWindow* window, 
    PalEventDriver* eventDriver);

PalGLWindow glHelperGetGLWindow(PalWindow* window);

PalGLContext* glHelperCreateContext(
    PalGLWindow* glWindow, 
    PalGLFBConfig* fbConfig);
    
void glHelperDestroyContext(PalGLContext* context);

#endif // _OPENGL_HELPER_H