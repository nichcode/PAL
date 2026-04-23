
struct PushConstants
{
    uint width;
    uint height;
    float4 color;
};

RWByteAddressBuffer outBuffer : register(u0);
ConstantBuffer<PushConstants> pc : register(b0);

[numThreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= pc.width || id.y >= pc.height) {
        return;
    }

    uint index = id.y * pc.width + id.x;
    uint offset = index * 16; // sizeof(float4)
    outBuffer.Store4(offset, asuint(pc.color));
}