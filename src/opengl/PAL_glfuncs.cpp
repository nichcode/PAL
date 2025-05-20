
#include "PAL_pch.h"
#include "PAL_glfuncs.h"

void* Load(const char* func_name);

#ifdef PAL_PLATFORM_WINDOWS
#include "win32/PAL_wgl_context.h"

void* Load(const char* func_name)
{
    return PAL_LoadWGL(func_name);
}

#endif // PAL_PLATFORM_WINDOWS

void LoadGL()
{
    glGetstring = (PFNGLGETSTRINGPROC)Load("glGetString");
    const char* version = (const char*) glGetstring(GL_VERSION);
    if (!version) {
        PAL_ERROR(PAL_PLATFORM_ERROR, "Failed to Get Opengl version");
        return;
    }

#ifdef _MSC_VER
    sscanf_s(version, "%d.%d", &major, &minor);
#else
    sscanf(version, "%d.%d", &glVersion.major, &glVersion.minor);
#endif

    glClear = (PFNGLCLEARPROC)Load("glClear");
    glClearColor = (PFNGLCLEARCOLORPROC)Load("glClearColor");

    glGenBuffers = (PFNGLGENBUFFERSPROC)Load("glGenBuffers");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)Load("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)Load("glBindVertexArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)Load("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)Load("glVertexAttribPointer");
    glBindBuffer = (PFNGLBINDBUFFERPROC)Load("glBindBuffer");
    
    glBufferData = (PFNGLBUFFERDATAPROC)Load("glBufferData");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)Load("glDeleteVertexArrays");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)Load("glDeleteBuffers");
    glDrawElements = (PFNGLDRAWELEMENTSPROC)Load("glDrawElements");

    glCreateProgram = (PFNGLCREATEPROGRAMPROC)Load("glCreateProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)Load("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)Load("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)Load("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)Load("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)Load("glGetShaderInfoLog");
    glAttachShader = (PFNGLATTACHSHADERPROC)Load("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)Load("glLinkProgram");
    glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)Load("glValidateProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)Load("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)Load("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)Load("glUseProgram");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)Load("glDeleteProgram");
    glDetachShader = (PFNGLDETACHSHADERPROC)Load("glDetachShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)Load("glDeleteShader");
    
    glBindVertexBuffers = (PFNGLBINDVERTEXBUFFERSPROC)Load("glBindVertexBuffers");
    glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)Load("glBindVertexBuffer");
    glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC)Load("glVertexBindingDivisor");
    glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC)Load("glVertexAttribFormat");
    glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC)Load("glVertexAttribIFormat");
    glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC)Load("glVertexAttribBinding");

    glViewport = (PFNGLVIEWPORTPROC)Load("glViewport");
}