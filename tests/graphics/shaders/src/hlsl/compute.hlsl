
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
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= pc.width || id.y >= pc.height) {
        return;
    }

    uint index = id.y * pc.width + id.x;
    uint offset = index * 16; // sizeof(float4)
    outBuffer.Store4(offset, asuint(pc.color));
}
