

struct PushData
{
    uint width;
    uint height;
    float4 set1Color;
    float4 set2Color;
    float4 set3Color;
};

[[vk::push_constant]]
ConstantBuffer<PushData> pc : register(b0);

// we use structured buffer for this example.

// binding 0 set 0
[[vk::binding(0, 0)]]
RWStructuredBuffer<float4> set1OutBuffer : register(u0, space0);

// binding 0 set 1
[[vk::binding(0, 1)]]
RWStructuredBuffer<float4> set2OutBuffer : register(u0, space1);

// binding 0 set 2
[[vk::binding(0, 2)]]
RWStructuredBuffer<float4> set3OutBuffer : register(u0, space2);

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= pc.width || id.y >= pc.height) {
        return;
    }

    uint index = id.y * pc.width + id.x;
    set1OutBuffer[index] = pc.set1Color;
    set2OutBuffer[index] = pc.set2Color;
    set3OutBuffer[index] = pc.set3Color;
}
