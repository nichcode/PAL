
struct RayPayload
{
    float3 color;
};

[shader("miss")]
void main(inout RayPayload payload)
{
    payload.color = float3(0.0, 0.0, 0.0);
}