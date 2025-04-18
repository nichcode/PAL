
#pragma once

#include "PAL/PAL_defines.h"
#include "PAL/PAL_input.h"
#include "PAL/PAL_buffer.h"
#include "PAL/PAL_shader.h"
#include "PAL/PAL_viewport.h"
#include "PAL/PAL_texture.h"
#include "PAL/PAL_font.h"
#include "PAL/PAL_renderer.h"
#include "PAL_maths.h"

#include <vector>
#include <map>

#include "ft2build.h"
#include FT_FREETYPE_H

struct PAL_Context;

struct PAL_Window
{
    void* handle;
    PAL_Context* context;
    u32 width, height;
    i32 x, y;
    const char* title;
    b8 shouldClose, focused;

    u32 keycodes[512];
    u32 scancodes[PAL_Keys_Max + 1];
    u32 keys[PAL_Keys_Max + 1];
    u32 buttons[PAL_Buttons_Max + 1];
    i32 mousePos[2];
};

struct PAL_DynAPI
{
    // context
    void*(*createContext)(void* window_handle, i32 major, i32 minor) = nullptr;
    void(*destroyContext)(void* context_handle) = nullptr;
    void(*swapBuffers)(void* context_handle) = nullptr;
    void(*setVsync)(void* context_handle, b8 vsync) = nullptr;
    void(*makeActive)(void* context_handle) = nullptr;
    void(*clear)(void* context_handle, const PAL_Color color) = nullptr;
    void(*drawArrays)(void* context_handle, u32 mode, u32 count) = nullptr;
    void(*drawElements)(void* context_handle, u32 mode, u32 count) = nullptr;
    void(*drawArraysInstanced)(void* context_handle, u32 mode, u32 count, u32 instance_count) = nullptr;
    void(*drawElementsInstanced)(void* context_handle, u32 mode, u32 count, u32 instance_count) = nullptr;
    void(*setView)(void* context_handle, PAL_Viewport view) = nullptr;
    void(*setBlend)(void* context_handle, u32 blend_mode) = nullptr;

    // buffer
    void*(*createBuffer)(PAL_BufferDesc desc) = nullptr;
    void(*destroyBuffer)(void* buffer_handle) = nullptr;
    void(*bindBuffer)(void* buffer_handle) = nullptr;
    void(*setBufferData)(void* buffer_handle, void* data, u32 size) = nullptr;

    // shader
    void*(*createShader)(PAL_ShaderDesc desc) = nullptr;
    void(*destroyShader)(void* shader_handle) = nullptr;
    void(*bindShader)(void* shader_handle) = nullptr;
    void(*setInt)(void* shader_handle, const char* name, i32 data) = nullptr;
    void(*setIntArray)(void* shader_handle, const char* name, i32* data, u32 count) = nullptr;
    void(*setFloat)(void* shader_handle, const char* name, f32 data) = nullptr;
    void(*setFloat2)(void* shader_handle, const char* name, f32 data, f32 data2) = nullptr;
    void(*setFloat3)(void* shader_handle, const char* name, f32 data, f32 data2, f32 data3) = nullptr;
    void(*setFloat4)(void* shader_handle, const char* name, f32 data, f32 data2, f32 data3, f32 data4) = nullptr;
    void(*setMat4)(void* shader_handle, const char* name, f32* data) = nullptr;
    void(*setLayout)(void* shader_handle, PAL_ShaderLayout* layout) = nullptr;

    // texture 
    void*(*createTexture)(PAL_TextureDesc desc) = nullptr;
    void*(*loadTexture)(const char* filepath, u32* width, u32* height) = nullptr;
    void(*destroyTexture)(void* texture_handle) = nullptr;
    void(*bindTexture)(void* texture_handle, u32 slot) = nullptr;
    void(*bindTarget)(void* texture_handle, u32 width, u32 height) = nullptr;
    void(*unbindTarget)(void* texture_handle) = nullptr;
};

struct PAL_PipeLineState
{
    PAL_Buffer* activeVBuffer = nullptr;
    PAL_Buffer* activeIBuffer = nullptr;
    PAL_Buffer* activeSBuffer = nullptr;
    PAL_Buffer* activeUBuffer = nullptr;
    PAL_Shader* activeShader = nullptr;
    PAL_Texture* activeTexture = nullptr;
    PAL_Texture* activeTarget = nullptr;
};

struct PAL_Destructor
{
    std::vector<PAL_Buffer*> buffers;
    std::vector<PAL_Shader*> shaders;
    std::vector<PAL_Texture*> textures;
    std::vector<PAL_Font*> fonts;
};

struct PAL_Context
{
    PAL_DynAPI api;
    PAL_Destructor data;
    PAL_PipeLineState state;
    PAL_Viewport view;
    void* handle;
};

struct PAL_Buffer
{
    void* handle = nullptr;
    u32 type = 0;
    b8 dataSent = false;
};

struct PAL_Shader
{
    void* handle = nullptr;
    PAL_ShaderLayout layout;
    b8 layoutSent = false;
};

struct PAL_Texture
{
    void* handle = nullptr;
    u32 width = 0;
    u32 height = 0;
    u32 flag = 0;
    const char* path = nullptr;
};

struct PAL_Font
{
    PAL_Texture* texture = nullptr;
    std::map<u8, PAL_Glyph> glyphs;
    const char* path = nullptr;
    u32 size = 0;
    f32 baseline = 0.0f;
};

struct PAL_Vertex
{
    PAL_Vec3 position;
    PAL_Color color;
    PAL_Vec4 texture;
};

struct PAL_Renderer
{
    u32 indexCount = 0;
    u32 texIndex = 1;
    
    PAL_Buffer* vbo = nullptr;
    PAL_Buffer* ibo = nullptr;
    PAL_Shader* shader = nullptr;

    PAL_Texture* whiteTexture;

    PAL_Vec4 vertices[4];
    PAL_Vec2 texCoords[4];
    std::vector<PAL_Vertex> sprites;
    std::vector<PAL_Texture*> textures;
};

static PAL_Context* s_ActiveContext = nullptr;
static void* s_UserData = nullptr;
static FT_Library s_Library;

void _InitInput();
void _ConsoleWrite(u32 level, const char* message);
void _SetError(const char* msg, ...);

#define CHECK_CONTEXT(...) if (s_ActiveContext) {} else {_SetError("no active context"); __VA_ARGS__;}                                                       
#define CHECK_ERR(expr, msg, ...) if (expr) {} else {_SetError(msg); __VA_ARGS__;}