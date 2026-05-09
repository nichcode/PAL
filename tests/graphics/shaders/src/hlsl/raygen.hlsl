
struct RayPayload
{
    float3 color;
};

RWByteAddressBuffer outBuffer : register(u0, space0); // set 0
RaytracingAccelerationStructure tlas : register(t0, space0); // set 0

[shader("raygeneration")]
void main()
{
    uint2 pixel = DispatchRaysIndex().xy;
    uint2 size = DispatchRaysDimensions().xy;
    RayPayload payload;
    payload.color = float3(0.0, 0.0, 0.0);

    float2 uv = (float2(pixel) + 0.5) / float2(size);
    float2 ndcUv = uv * 2.0 - 1.0;

    RayDesc desc;
    desc.Origin = float3(0.0, 0.0, -3.0);;
    desc.Direction = normalize(float3(ndcUv.x, ndcUv.y, 1.0));;
    desc.TMin = 0.001;
    desc.TMax = 1000.0;
    TraceRay(tlas, RAY_FLAG_NONE, 0xFF, 0, 0, 0, desc, payload);

    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }

    uint index = pixel.y * size.x + pixel.x;
    uint offset = index * 16; // sizeof(float4)
    float4 color = float4(payload.color, 1.0);
    outBuffer.Store4(offset, asuint(color));
}