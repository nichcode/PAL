
#include "PAL_pch.h"
#include "PAL_glapi.h"
#include "PAL_glfuncs.h"

#ifdef PAL_PLATFORM_WINDOWS
#include "PAL_glwin32context.h"
#endif // PAL_PLATFORM_WINDOWS

inline u32 PAL_GetBufferUsageToGL(u32 usage)
{
    switch (usage) {
        case PAL_USAGE_STATIC: {
            return GL_STATIC_DRAW;
            break;
        }
        case PAL_USAGE_DYNAMIC: {
            return GL_DYNAMIC_DRAW;  
            break;
        }
    }

    PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid buffer usage");
    return 0;
}

inline GLenum PAL_GetGFormat(u32 format)
{
    switch (format) {
        case PAL_FORMAT_FLOAT:
        case PAL_FORMAT_FLOAT2:
        case PAL_FORMAT_FLOAT3:
        case PAL_FORMAT_FLOAT4: {
            return GL_FLOAT;
        }

        case PAL_FORMAT_INT:
        case PAL_FORMAT_INT2:
        case PAL_FORMAT_INT3:
        case PAL_FORMAT_INT4: {
            return GL_INT;
        }
    }

    PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid element format");
    return 0;
}

inline u32 PAL_GetGLFormatCount(u32 format)
{
    switch (format)
    {
        case PAL_FORMAT_INT:
        case PAL_FORMAT_FLOAT: {
            return 1;
        }

        case PAL_FORMAT_INT2:
        case PAL_FORMAT_FLOAT2: {
            return 2;
        }

        case PAL_FORMAT_INT3:
        case PAL_FORMAT_FLOAT3: {
            return 3;
        }

        case PAL_FORMAT_INT4:
        case PAL_FORMAT_FLOAT4: {
            return 4;
        }
    }

    PAL_ERROR(PAL_INVALID_PARAMETER, "Invalid element format");
    return 0;
}

void* PAL_GLDynAPI::CreateDevice(void* window_handle, PAL_DeviceDesc* desc)
{
    PAL_GLDevice* device = new PAL_GLDevice();
    if (!device) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for opengl device");
    }

    i32 major, minor;
    if (!desc->glVersionMajor) { major = glVersion.major; }
    if (!desc->glVersionMinor) { minor = glVersion.minor; }

    device->context = CreateGLContext(
        window_handle,
        desc->debug,
        major,
        minor
    );

    if (!device->context) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to create WGL context");
    }

    return device;
}

void PAL_GLDynAPI::DestroyDevice(void* handle)
{
    PAL_GLDevice* device = (PAL_GLDevice*)handle;
    device->context->Destroy();
    delete device;    
}

void PAL_GLDynAPI::SetClearColor(void* handle, f32 r, f32 g, f32 b, f32 a)
{
    glClearColor(r, g, b, a);
}

void PAL_GLDynAPI::Clear(void* handle)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PAL_GLDynAPI::Present(void* dhandle)
{
    PAL_GLDevice* device = (PAL_GLDevice*)dhandle;
    device->context->Present();
}

void PAL_GLDynAPI::Draw(void* handle, u32 count)
{
    
}

void PAL_GLDynAPI::DrawInstanced(void* handle, u32 count, u32 instance_count)
{
    
}

void PAL_GLDynAPI::DrawIndexed(void* handle, u32 count)
{
    // TODO: change type
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void PAL_GLDynAPI::DrawIndexedInstance(void* handle, u32 count, u32 instance_count)
{
    
}


//************************buffer*******************************
void* PAL_GLDynAPI::CreateBuffer(void* device_handle, PAL_BufferDesc* desc)
{
    PAL_GLBuffer* buffer = new PAL_GLBuffer();
    if (!buffer) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for opengl buffer");
        return nullptr;
    }

    if (desc->type == PAL_VERTEX_BUFFER) {
        buffer->size = desc->size;
    }
    else if (desc->type == PAL_INDEX_BUFFER) {
        u32 index_size =  0;
        if (desc->indexFormat == PAL_INDEX_U16) {
            index_size = sizeof(u16);
        }
        else if (desc->indexFormat == PAL_INDEX_U32) {
            index_size = sizeof(u32);
        }

        buffer->size = desc->indexCount * index_size;
    }

    buffer->data = desc->data;
    buffer->usage = PAL_GetBufferUsageToGL(desc->usage);
    glGenBuffers(1, &buffer->id);

    return buffer;
}

void PAL_GLDynAPI::DestroyBuffer(void* handle)
{
    PAL_GLBuffer* buffer = (PAL_GLBuffer*)handle;
    glDeleteBuffers(1, &buffer->id);
    delete buffer;
}

void PAL_GLDynAPI::SetVertexBuffer(void* handle, u32 binding_slot, u32 stride, u32 offset)
{
    PAL_GLBuffer* buffer = (PAL_GLBuffer*)handle;

    if (!buffer->dataSent) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
        glBufferData(GL_ARRAY_BUFFER, buffer->size, buffer->data, buffer->usage);
        buffer->dataSent = true;
    }

    glBindVertexBuffer(binding_slot, buffer->id, offset, stride);
}

void PAL_GLDynAPI::SetIndexBuffer(void* handle)
{
    PAL_GLBuffer* buffer = (PAL_GLBuffer*)handle;
    if (!buffer->dataSent) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer->size, buffer->data, buffer->usage);
        buffer->dataSent = true;
        return;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}

//************************layout*******************************
void* PAL_GLDynAPI::CreateLayout(void* device_handle, PAL_Element* elements, u32 count, u32 binding_index)
{
    PAL_GLLayout* layout = new PAL_GLLayout();
    if (!layout) {
        PAL_ERROR(PAL_OUT_OF_MEMORY, "Failed to allocate memory for opengl layout");
        return nullptr;
    }

    layout->elements = elements;
    layout->count = count;
    layout->bindingIndex = binding_index;
    
    glGenVertexArrays(1, &layout->id); 
    return layout;
}

void PAL_GLDynAPI::DestroyLayout(void* handle)
{
    PAL_GLLayout* layout = (PAL_GLLayout*)handle;
    glDeleteVertexArrays(1, &layout->id);
    delete layout;
}

void PAL_GLDynAPI::SetLayout(void* handle)
{
    PAL_GLLayout* layout = (PAL_GLLayout*)handle;
    glBindVertexArray(layout->id);

    for (u32 i = 0; i < layout->count; i++) {
        PAL_Element& element = layout->elements[i];
        u32 count = PAL_GetGLFormatCount(element.format);
        u32 format = PAL_GetGFormat(element.format);
        u32 binding_index = layout->bindingIndex;
        u32 index = element.index;

        u32 divisor = 0;
        if (element.type == PAL_PER_INSTANCE) {
            divisor = element.instanceStepRate;
        }

        switch (format) {
            case GL_FLOAT: {
                glEnableVertexAttribArray(index);
                glVertexAttribFormat(index, count, GL_FLOAT, GL_FALSE, element.offset);
                glVertexAttribBinding(index, binding_index);
                glVertexBindingDivisor(binding_index, divisor);
                break;
            }

            case GL_INT: {
                glEnableVertexAttribArray(index);
                glVertexAttribIFormat(index, count, GL_INT, element.offset);
                glVertexAttribBinding(index, binding_index);
                glVertexBindingDivisor(binding_index, divisor);
                break;
            }
        }
    }
}

PAL_GLContext* CreateGLContext(void* window_handle, b8 debug, u32 major, u32 minor)
{
#ifdef PAL_PLATFORM_WINDOWS
    return new PAL_GLWin32Context(window_handle, debug, major, minor);
#endif // PAL_PLATFORM_WINDOWS
}
