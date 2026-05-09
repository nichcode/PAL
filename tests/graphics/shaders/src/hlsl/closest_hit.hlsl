
struct RayPayload
{
    float3 color;
};

struct HitAttributes
{
    float2 unused;
};

[shader("closesthit")]
void main(
    inout RayPayload payload, 
    in HitAttributes attr)
{
    payload.color = float3(0.0, 1.0, 0.0);
}