
#pragma once

#include "PAL_glapi.h"
#include "win32/PAL_win32.h"

class PAL_GLWin32Context : public PAL_GLContext
{
private:
    HWND m_Window;
    HDC m_DeviceContext;
    HGLRC m_Context;

public:
    PAL_GLWin32Context(void* window_handle, b8 debug, u32 major, u32 minor);
    void Destroy() override;
    void Present() override;
};