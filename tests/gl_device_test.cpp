
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
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 1.0f
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

    PAL_Element elements[2];
    elements[0].format = PAL_FORMAT_FLOAT2;
    elements[0].index = 0;
    elements[0].instanceStepRate = 0;
    elements[0].name = "a_Position";
    elements[0].offset = 0; // will be set internally
    elements[0].type = PAL_PER_VERTEX;

    elements[1].format = PAL_FORMAT_FLOAT3;
    elements[1].index = 1;
    elements[1].instanceStepRate = 0;
    elements[1].name = "a_Color";
    elements[1].offset = 0; // will be set internally
    elements[1].type = PAL_PER_VERTEX;

    PAL_Layout* layout = PAL_CreateLayout(device, elements, 2, 0);

    PAL_VertexShader* vertex_shader = PAL_CreateVertexShader(
        device,
        layout,
        "shaders/device_vertex.glsl",
        true
    );

    PAL_PixelShader* pixel_shader = PAL_CreatePixelShader(
        device,
        "shaders/device_pixel.glsl",
        true
    );

    PAL_Viewport viewport;
    viewport.width = 640;
    viewport.height = 480;

    // set pipeline
    PAL_PipeLine* pipeline = PAL_GetPipeLine(device);
    pipeline->layout = layout;
    pipeline->vertexBuffer = vertex_buffer;
    pipeline->indexBuffer = index_buffer;
    pipeline->vertexShader = vertex_shader;
    pipeline->pixelShader = pixel_shader;
    pipeline->viewport = &viewport;

    pipeline->vertexBufferSlot = 0;
    pipeline->vertexBufferOffset = 0;
    pipeline->vertexBufferStride = PAL_GetLayoutStride(layout);
    pipeline->count = 3;
    pipeline->drawType = PAL_INDEX_DRAW;
    pipeline->primitive = PAL_TRIANGLES;

    while (!PAL_WindowShouldClose(window)) {
        PAL_PullEvents();

        PAL_Clear(device);
        PAL_Flush(device);
        PAL_Present(device);
    }

    PAL_DestroyVertexShader(vertex_shader);
    PAL_DestroyPixelShader(pixel_shader);

    PAL_DestroyLayout(layout);
    PAL_DestroyBuffer(vertex_buffer);
    PAL_DestroyBuffer(index_buffer);
    
    PAL_DestroyDevice(device);
    PAL_DestroyWindow(window);
    PAL_Terminate();
    return 0;
}