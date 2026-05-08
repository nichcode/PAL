
#version 450

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 0, binding = 0) buffer OutputBuffer
{
    vec4 pixels[];
} outBuffer;

layout(push_constant) uniform PushConstants
{
    uint width;
    uint height;
    vec4 color;
} pc;

void main()
{
    uvec2 id = gl_GlobalInvocationID.xy;
    if (id.x >= pc.width || id.y >= pc.height) {
        return;
    }

    uint index = id.y * pc.width + id.x;
    outBuffer.pixels[index] = pc.color;
}