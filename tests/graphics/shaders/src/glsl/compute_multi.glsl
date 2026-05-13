
#version 450

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 0, binding = 0) buffer OutputBuffers
{
    vec4 pixels[];
} outBuffers[2];

layout(set = 1, binding = 0) buffer OutputBuffer
{
    vec4 pixels[];
} outBuffer;

layout(push_constant) uniform PushConstants
{
    uint width;
    uint height;
    vec4 bufferColor1;
    vec4 bufferColor2;
    vec4 bufferColor3;
} pc;

void main()
{
    uvec2 id = gl_GlobalInvocationID.xy;
    if (id.x >= pc.width || id.y >= pc.height) {
        return;
    }

    uint index = id.y * pc.width + id.x;
    outBuffers[0].pixels[index] = pc.bufferColor1;
    outBuffers[1].pixels[index] = pc.bufferColor2;
    outBuffer.pixels[index] = pc.bufferColor3;
}