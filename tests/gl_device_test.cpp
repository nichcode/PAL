
#include "PAL/PAL.h"

int main(int argc, char** argv)
{
    PAL_Init();
    u32 flags = PAL_APP_WINDOW | PAL_WINDOW_CENTER;
    PAL_Window* window = PAL_CreateWindow("PAL Window", 640, 480, flags);

    PAL_DeviceDesc device_desc;
    device_desc.debug = true;
    device_desc.type = PAL_OPENGL_DEVICE;
    PAL_Device* device = PAL_CreateDevice(window, &device_desc);

    PAL_SetClearColor(device, .2f, .2f, .2f, 1.0f);

    // buffers
    f32 vertices[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.0f,  0.5f
	};

    u32 indices[] = { 0, 1, 2 };

    PAL_BufferDesc buffer_desc;
    buffer_desc.data = vertices;
    buffer_desc.size = sizeof(vertices);
    buffer_desc.type = PAL_VERTEX_BUFFER;
    buffer_desc.usage = PAL_USAGE_STATIC;
    PAL_Buffer* vertex_buffer = PAL_CreateBuffer(device, &buffer_desc);

    buffer_desc.data = indices;
    buffer_desc.type = PAL_INDEX_BUFFER;
    buffer_desc.indexCount = 3;
    buffer_desc.indexFormat = PAL_INDEX_U32;
    PAL_Buffer* index_buffer = PAL_CreateBuffer(device, &buffer_desc);

    PAL_Element element;
    element.format = PAL_FORMAT_FLOAT2;
    element.index = 0;
    element.instanceStepRate = 0;
    element.name = "a_Position";
    element.offset = 0; // will be set internally
    element.type = PAL_PER_VERTEX;

    PAL_Layout* layout = PAL_CreateLayout(device, &element, 1, 0);

    // set pipeline
    PAL_PipeLine* pipeline = PAL_GetPipeLine(device);
    pipeline->layout = layout;
    pipeline->vertexBuffer = vertex_buffer;
    pipeline->indexBuffer = index_buffer;

    pipeline->vertexBufferSlot = 0;
    pipeline->vertexBufferOffset = 0;
    pipeline->vertexBufferStride = PAL_GetLayoutStride(layout);
    pipeline->indexCount = 3;

    while (!PAL_WindowShouldClose(window)) {
        PAL_PullEvents();

        PAL_Clear(device);
        PAL_Flush(device);
        PAL_Present(device);
    }

    PAL_DestroyLayout(layout);
    PAL_DestroyBuffer(vertex_buffer);
    PAL_DestroyBuffer(index_buffer);
    
    PAL_DestroyDevice(device);
    PAL_DestroyWindow(window);
    PAL_Terminate();
    return 0;
}