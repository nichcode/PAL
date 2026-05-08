
struct PushConstants
{
    uint width;
    uint height;
    float4 color;
};

PushConstants pc;

// We used raw Buffer but structured buffer can be used as well. 
// PalDescriptorBufferInfo::stride must be set to 16
RWByteAddressBuffer outBuffer : register(u0, space0); // set 0

[numthreads(16, 16, 1)]
void computeMain(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= pc.width || id.y >= pc.height) {
        return;
    }

    uint index = id.y * pc.width + id.x;
    uint offset = index * 16; // sizeof(float4)
    outBuffer.Store4(offset, asuint(pc.color));
}

// GLSL equivalent

// #version 450

// layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// layout(set = 0, binding = 0) buffer OutputBuffer
// {
//     vec4 pixels[];
// } outBuffer;

// layout(push_constant) uniform PushConstants
// {
//     uint width;
//     uint height;
//     vec4 color;
// } pc;

// void main()
// {
//     uvec2 id = gl_GlobalInvocationID.xy;
//     if (id.x >= pc.width || id.y >= pc.height) {
//         return;
//     }

//     uint index = id.y * pc.width + id.x;
//     outBuffer.pixels[index] = pc.color;
// }
