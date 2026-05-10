
#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 payloadColor;

layout(shaderRecordEXT) buffer HitRecord
{
    vec3 color;
} hitRecord;

void main()
{
    payloadColor = hitRecord.color;
}