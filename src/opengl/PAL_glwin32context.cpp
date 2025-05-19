
#include "PAL_pch.h"
#include "PAL_glwin32context.h"
#include "win32/PAL_wgl_context.h"

PAL_GLWin32Context::PAL_GLWin32Context(void* window_handle, b8 debug, u32 major, u32 minor)
{
    m_Window = (HWND)window_handle;
    m_Context = PAL_WGLCreateContext(
        m_Window,
        &m_DeviceContext,
        debug,
        major,
        minor
    );
}

void PAL_GLWin32Context::Destroy()
{
    s_wglDeleteContext(m_Context);
    ReleaseDC(m_Window, m_DeviceContext);
}

void PAL_GLWin32Context::Present()
{
    s_SwapBuffers(m_DeviceContext);
}