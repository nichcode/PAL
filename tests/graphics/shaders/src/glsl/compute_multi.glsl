
#version 450

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(set = 0, binding = 0) buffer Set1OutputBuffer
{
    vec4 pixels[];
} set1OutBuffer;

layout(set = 1, binding = 0) buffer Set2OutputBuffer
{
    vec4 pixels[];
} set2OutBuffer;

layout(set = 2, binding = 0) buffer Set3OutputBuffer
{
    vec4 pixels[];
} set3OutBuffer;

layout(push_constant) uniform PushConstants
{
    uint width;
    uint height;
    vec4 set1Color;
    vec4 set2Color;
    vec4 set3Color;
} pc;

void main()
{
    uvec2 id = gl_GlobalInvocationID.xy;
    if (id.x >= pc.width || id.y >= pc.height) {
        return;
    }

    uint index = id.y * pc.width + id.x;
    set1OutBuffer.pixels[index] = pc.set1Color;
    set2OutBuffer.pixels[index] = pc.set2Color;
    set3OutBuffer.pixels[index] = pc.set3Color;
}