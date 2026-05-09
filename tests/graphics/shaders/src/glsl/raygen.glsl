
#version 460
#extension GL_EXT_ray_tracing : require

layout(set = 0, binding = 0) buffer OutputBuffer
{
    vec4 pixels[];
} outBuffer;

layout(set = 0, binding = 1) uniform accelerationStructureEXT tlas;
layout(location = 0) rayPayloadEXT vec3 payloadColor;

void main()
{
    uvec2 pixel = gl_LaunchIDEXT.xy;
    uvec2 size = gl_LaunchSizeEXT.xy;
    payloadColor = vec3(0.0);

    vec2 uv = (vec2(pixel) + 0.5) / vec2(size);
    vec2 ndcUv = uv * 2.0 - 1.0;
    vec3 origin = vec3(0.0, 0.0, -3.0);
    vec3 direction = normalize(vec3(ndcUv.x, ndcUv.y, 1.0));

    traceRayEXT(
        tlas,
        gl_RayFlagsOpaqueEXT,
        0xFF,
        0,
        0,
        0,
        origin,
        0.001,
        direction,
        1000.0,
        0
    );

    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }

    uint index = pixel.y * size.x + pixel.x;
    outBuffer.pixels[index] = vec4(payloadColor, 1.0);
}
