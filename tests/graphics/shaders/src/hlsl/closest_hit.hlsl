
struct RayPayload
{
    float3 color;
};

struct HitAttributes
{
    float2 unused;
};

cbuffer HitRecord : register(b0)
{
    float3 hitColor;
};

[shader("closesthit")]
void main(
    inout RayPayload payload, 
    in HitAttributes attr)
{
    payload.color = hitColor;
}