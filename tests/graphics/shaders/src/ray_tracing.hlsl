
struct RayPayload
{
    float3 color;
};

struct HitAttributes
{
    float2 unused;
};

RWByteAddressBuffer outBuffer : register(u0, space0); // set 0
RaytracingAccelerationStructure tlas : register(t0, space0); // set 0

[shader("raygeneration")]
void raygenMain()
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

[shader("closesthit")]
void closestHitMain(
    inout RayPayload payload, 
    in HitAttributes attr)
{
    payload.color = float3(0.0, 1.0, 0.0);
}

[shader("miss")]
void missMain(inout RayPayload payload)
{
    payload.color = float3(0.0, 0.0, 0.0);
}

// GLSL equivalent

// // Raygen
// #version 460
// #extension GL_EXT_ray_tracing : require

// layout(set = 0, binding = 0) buffer OutputBuffer
// {
//     vec4 pixels[];
// } outBuffer;

// layout(set = 0, binding = 1) uniform accelerationStructureEXT tlas;
// layout(location = 0) rayPayloadEXT vec3 payloadColor;

// void main()
// {
//     uvec2 pixel = gl_LaunchIDEXT.xy;
//     uvec2 size = gl_LaunchSizeEXT.xy;
//     payloadColor = vec3(0.0);

//     vec2 uv = (vec2(pixel) + 0.5) / vec2(size);
//     vec2 ndcUv = uv * 2.0 - 1.0;
//     vec3 origin = vec3(0.0, 0.0, -3.0);
//     vec3 direction = normalize(vec3(ndcUv.x, ndcUv.y, 1.0));

//     traceRayEXT(
//         tlas,
//         gl_RayFlagsOpaqueEXT,
//         0xFF,
//         0,
//         0,
//         0,
//         origin,
//         0.001,
//         direction,
//         1000.0,
//         0
//     );

//     if (pixel.x >= size.x || pixel.y >= size.y) {
//         return;
//     }

//     uint index = pixel.y * size.x + pixel.x;
//     outBuffer.pixels[index] = vec4(payloadColor, 1.0);
// }

// // Closest Hit
// #version 460
// #extension GL_EXT_ray_tracing : require

// layout(location = 0) rayPayloadInEXT vec3 payloadColor;

// void main()
// {
//     payloadColor = vec3(0.0, 1.0, 0.0);
// }

// // Miss
// #version 460
// #extension GL_EXT_ray_tracing : require

// layout(location = 0) rayPayloadInEXT vec3 payloadColor;

// void main()
// {
//     payloadColor = vec3(0.0);
// }