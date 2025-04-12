
#include "prime/prime.h"

void onWindowResize(prWindow* window, u32 width, u32 height)
{
    prContext* context = (prContext*)prGetUserData();
    prViewport view;
    view.width = width;
    view.height = height;
    prSetView(view);
}

int main(int argc, char** argv)
{
    prInit();

    u32 flags = prWindowFlags_Center | prWindowFlags_Show;
    prWindow* window = prCreateWindow("window", 640, 480, flags);

    prContextDesc desc;
    desc.type = prContextTypes_OpenGL;
    desc.major = 3;
    desc.minor = 3;
    prContext* context = prCreateContext(window, desc);
    prMakeActive(context, false);
    prSetVsync(true);
    prSetClearColor(.2f, .2f, .2f, 1.0f);

    f32 vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

    u32 indices[] = { 0, 1, 2, 2, 3, 0 };

    // layout
    prShaderLayout layout;
    prShaderAttrib position_attrib;
    position_attrib.type = prShaderDataTypes_Float3;
    layout.attribs[0] = position_attrib;
    layout.count = 1;

    // vertex buffer
    prBufferDesc buffer_desc;
    buffer_desc.type = prBufferTypes_Vertex;
    buffer_desc.data = vertices;
    buffer_desc.size = sizeof(vertices);
    buffer_desc.usage = prBufferUsages_Static;
    prBuffer* vertex_buffer = prCreateBuffer(context, buffer_desc);

    // index buffer
    buffer_desc.type = prBufferTypes_Index;
    buffer_desc.data = indices;
    buffer_desc.size = sizeof(indices);
    buffer_desc.usage = prBufferUsages_Static;
    prBuffer* index_buffer = prCreateBuffer(context, buffer_desc);
    
    // shader
    prShaderDesc shader_desc;
    shader_desc.load = true;
    shader_desc.type = prShaderSourceTypes_GLSL;
    shader_desc.vertex_src = "shaders/context_vertex.glsl";
    shader_desc.pixel_src = "shaders/context_pixel.glsl";
    shader_desc.layout = layout;
    prShader* shader = prCreateShader(context, shader_desc);

    prBindBuffer(vertex_buffer);
    prBindBuffer(index_buffer);
    prBindShader(shader);

    prSetWindowResizedCallback(onWindowResize);
    prSetUserData(context);

    while (!prWindowShouldClose(window)) {
        prPullEvents();

        prClear();
        prDrawElements(prDrawModes_Triangles, 6);
        prSwapBuffers();
    }

    prSetWindowResizedCallback(nullptr);
    prDestroyContext(context);
    prDestroyWindow(window);
    prShutdown();
    return 0;
}