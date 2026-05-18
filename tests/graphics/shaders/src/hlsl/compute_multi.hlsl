
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
// some use index 0 and so use index 1.
RWStructuredBuffer<float4> outBuffers[2] : register(u0, space0); // set 0
RWStructuredBuffer<float4> outBuffer : register(u0, space1); // set 1

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= width || id.y >= height) {
        return;
    }

    uint index = id.y * width + id.x;
    outBuffers[0][index] = bufferColor1;
    outBuffers[1][index] = bufferColor2;
    outBuffer[index] = bufferColor3;
}
