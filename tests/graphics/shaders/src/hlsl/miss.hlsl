
struct RayPayload
{
    float3 color;
};

cbuffer MissRecord : register(b0)
{
    float3 missColor;
};

[shader("miss")]
void main(inout RayPayload payload)
{
    payload.color = missColor;
}