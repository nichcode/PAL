
cbuffer PushConstants : register(b0)
{
    uint width;
    uint height;
    float4 set1Color;
    float4 set2Color;
    float4 set3Color;
};

// we use structured buffer for this example.
RWStructuredBuffer<float4> set1OutBuffer : register(u0, space0);
RWStructuredBuffer<float4> set2OutBuffer : register(u0, space1);
RWStructuredBuffer<float4> set3OutBuffer : register(u0, space2);

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= width || id.y >= height) {
        return;
    }

    uint index = id.y * width + id.x;
    set1OutBuffer[index] = set1Color;
    set2OutBuffer[index] = set2Color;
    set3OutBuffer[index] = set3Color;
}
