
cbuffer PushConstants : register(b0)
{
    uint width;
    uint height;
    float4 bufferColor1;
    float4 bufferColor2;
    float4 bufferColor3;
};

// we use structured buffer for this example.
// Some drivers do not map the registers well if we declare as an array
// some use index 0 and so use index 1. For portability, we make the registers explicit
RWStructuredBuffer<float4> outBuffer1 : register(u0, space0); // set 0
RWStructuredBuffer<float4> outBuffer2 : register(u1, space0); // set 0

RWStructuredBuffer<float4> outBuffer3 : register(u0, space1); // set 1

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= width || id.y >= height) {
        return;
    }

    uint index = id.y * width + id.x;
    outBuffer1[index] = bufferColor1;
    outBuffer2[index] = bufferColor2;
    outBuffer3[index] = bufferColor3;
}
