
#include "PAL_pch.h"
#include "PAL/PAL_shader.h"

PAL_Shader* PAL_CreateShader(PAL_ShaderDesc desc)
{
    CHECK_CONTEXT(return nullptr)
    PAL_Shader* shader = new PAL_Shader();
    CHECK_ERR(shader, "failed to create shader", return nullptr)

    shader->handle = s_ActiveContext->api.createShader(desc);
    CHECK_ERR(shader, "failed to create shader handle", delete shader; return nullptr)

    shader->layout = desc.layout;
    shader->layoutSent = false;

    s_ActiveContext->data.shaders.push_back(shader);
    return shader;
}

void PAL_DestroyShader(PAL_Shader* shader)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(shader, "shader is null", return)
    PAL_Context* context = s_ActiveContext;

    auto it = std::find(context->data.shaders.begin(), context->data.shaders.end(), shader);
    if (it != context->data.shaders.end()) {
        context->data.shaders.erase(it); 
    }

    if (context->state.activeShader == shader) {
        context->state.activeShader = nullptr;
    }
    context->api.destroyShader(shader->handle);
    delete shader;
    shader = nullptr;
}

void PAL_BindShader(PAL_Shader* shader)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(shader, "shader is null", return)
    PAL_Context* context = s_ActiveContext;

    if (context->state.activeShader != shader) {
        context->state.activeShader = shader;
        context->api.bindShader(shader->handle);
        if (!shader->layoutSent) {
            context->api.setLayout(shader->handle, &shader->layout);
            shader->layoutSent = true;
        }
    }
}

void PAL_SetInt(const char* name, i32 data)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(s_ActiveContext->state.activeShader, "no active shader")
    s_ActiveContext->api.setInt(s_ActiveContext->state.activeShader->handle, name, data);
}

void PAL_SetIntArray(const char* name, i32* data, u32 count) 
{
    CHECK_CONTEXT(return)
    CHECK_ERR(s_ActiveContext->state.activeShader, "no active shader")
    s_ActiveContext->api.setIntArray(s_ActiveContext->state.activeShader->handle, name, data, count);
}

void PAL_SetFloat(const char* name, f32 data)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(s_ActiveContext->state.activeShader, "no active shader")
    s_ActiveContext->api.setFloat(s_ActiveContext->state.activeShader->handle, name, data);
}

void PAL_SetFloat2(const char* name, f32 data, f32 data2)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(s_ActiveContext->state.activeShader, "no active shader")
    s_ActiveContext->api.setFloat2(s_ActiveContext->state.activeShader->handle, name, data, data2);
}

void PAL_SetFloat3(const char* name, f32 data, f32 data2, f32 data3)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(s_ActiveContext->state.activeShader, "no active shader")
    s_ActiveContext->api.setFloat3(s_ActiveContext->state.activeShader->handle, name, data, data2, data3);
}

void PAL_SetFloat4(const char* name, f32 data, f32 data2, f32 data3, f32 data4)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(s_ActiveContext->state.activeShader, "no active shader")
    s_ActiveContext->api.setFloat4(s_ActiveContext->state.activeShader->handle, name, data, data2, data3, data4);
}

void PAL_SetMat4(const char* name, f32* data)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(s_ActiveContext->state.activeShader, "no active shader")
    s_ActiveContext->api.setMat4(s_ActiveContext->state.activeShader->handle, name, data);
}