
#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 payloadColor;

void main()
{
    payloadColor = vec3(0.0);
}