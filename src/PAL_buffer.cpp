
#include "PAL_pch.h"
#include "PAL/PAL_buffer.h"

PAL_Buffer* PAL_CreateBuffer(PAL_BufferDesc desc)
{
    CHECK_CONTEXT(return nullptr) 
    PAL_Buffer* buffer = new PAL_Buffer();
    CHECK_ERR(buffer, "failed to create buffer", return nullptr)

    buffer->dataSent = false;
    buffer->type = desc.type;
    buffer->handle = s_ActiveContext->api.createBuffer(desc);
    CHECK_ERR(buffer, "failed to create buffer handle", delete buffer; return nullptr)

    // rebind the previous active buffer
    if (buffer->type == PAL_BufferTypes_Vertex) {
        if (s_ActiveContext->state.activeVBuffer) {
            s_ActiveContext->api.bindBuffer(s_ActiveContext->state.activeVBuffer->handle);
        }
    }

    else if (buffer->type == PAL_BufferTypes_Index) {
        if (s_ActiveContext->state.activeIBuffer) {
            s_ActiveContext->api.bindBuffer(s_ActiveContext->state.activeIBuffer->handle);
        }
    }

    else if (buffer->type == PAL_BufferTypes_Storage) {
        if (s_ActiveContext->state.activeSBuffer) {
            s_ActiveContext->api.bindBuffer(s_ActiveContext->state.activeSBuffer->handle);
        }
    }

    else if (buffer->type == PAL_BufferTypes_Uniform) {
        if (s_ActiveContext->state.activeUBuffer) {
            s_ActiveContext->api.bindBuffer(s_ActiveContext->state.activeUBuffer->handle);
        }
    }

    s_ActiveContext->data.buffers.push_back(buffer);
    return buffer;
}

void PAL_DestroyBuffer(PAL_Buffer* buffer)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(buffer, "buffer is null", return)
    PAL_Context* context = s_ActiveContext;

    auto it = std::find(context->data.buffers.begin(), context->data.buffers.end(), buffer);
    if (it != context->data.buffers.end()) {
        context->data.buffers.erase(it); 
    }

    if (buffer->type == PAL_BufferTypes_Vertex) {
        if (context->state.activeVBuffer == buffer) {
            context->state.activeVBuffer = nullptr;
        }
    }

    else if (buffer->type == PAL_BufferTypes_Index) {
        if (context->state.activeIBuffer == buffer) {
            context->state.activeIBuffer = nullptr;
        }
    }

    else if (buffer->type == PAL_BufferTypes_Storage) {
        if (context->state.activeSBuffer == buffer) {
            context->state.activeSBuffer = nullptr;
        }
    }

    else if (buffer->type == PAL_BufferTypes_Uniform) {
        if (context->state.activeUBuffer == buffer) {
            context->state.activeUBuffer = nullptr;
        }
    }

    context->api.destroyBuffer(buffer->handle);
    delete buffer;
    buffer = nullptr;
}

void PAL_BindBuffer(PAL_Buffer* buffer)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(buffer, "buffer is null", return)

    PAL_Context* context = s_ActiveContext;
    if (buffer->type == PAL_BufferTypes_Vertex) {
        if (context->state.activeVBuffer != buffer) {
            context->state.activeVBuffer = buffer;
            context->api.bindBuffer(buffer->handle);
        }
    }

    else if (buffer->type == PAL_BufferTypes_Index) {
        if (context->state.activeIBuffer != buffer) {
            context->state.activeIBuffer = buffer;
            context->api.bindBuffer(buffer->handle);
        }
    }

    else if (buffer->type == PAL_BufferTypes_Storage) {
        if (context->state.activeSBuffer != buffer) {
            context->state.activeSBuffer = buffer;
            context->api.bindBuffer(buffer->handle);
        }
    }

    else if (buffer->type == PAL_BufferTypes_Uniform) {
        if (context->state.activeUBuffer != buffer) {
            context->state.activeUBuffer = buffer;
            context->api.bindBuffer(buffer->handle);
        }
    }
}

void PAL_SetBufferData(u32 type, void* data, u32 size)
{
    CHECK_CONTEXT(return)
    PAL_Context* context = s_ActiveContext;
    
    if (type == PAL_BufferTypes_Vertex) {
        CHECK_ERR(context->state.activeVBuffer, "no active vertex buffer")
        context->api.setBufferData(context->state.activeVBuffer->handle, data, size);
    }

    else if (type == PAL_BufferTypes_Index) {
        CHECK_ERR(context->state.activeIBuffer, "no active index buffer")
        context->api.setBufferData(context->state.activeIBuffer->handle, data, size);
    }

    else if (type == PAL_BufferTypes_Storage) {
        CHECK_ERR(context->state.activeSBuffer, "no active storage buffer")
        context->api.setBufferData(context->state.activeSBuffer->handle, data, size);
    }

    else if (type == PAL_BufferTypes_Uniform) {
        CHECK_ERR(context->state.activeUBuffer, "no active uniform buffer")
        context->api.setBufferData(context->state.activeUBuffer->handle, data, size);
    }
}