
#include "pal/pal_opengl.h"
#include "pal/pal_video.h"

#include "glad/glad.h"

bool initOpengl(
    PalEventDriver** outEventDriver,
    PalWindow** outWindow);

void shutdownOpengl(
    PalEventDriver* eventDriver,
    PalWindow* window,
    PalGLContext* context);

bool createContext(
    PalWindow* window,
    PalGLPixelFormat* format,
    PalGLContext** outContext);

void createTriangle(
    Uint32* vao, 
    Uint32* vbo, 
    Uint32* ibo, 
    Uint32* program);

void drawTriangle(
    Uint32 vao, 
    Uint32 program);

void destroyTriangle(
    Uint32* vao, 
    Uint32* vbo, 
    Uint32* ibo, 
    Uint32* program);