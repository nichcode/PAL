
#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0) uniform texture2D texs[];
layout(set = 0, binding = 1) uniform sampler samp;

layout(push_constant) uniform PushConstants
{
    uint textureIndices[4];
} pc;

layout(location = 0) in vec2 aTexCoord;

layout(location = 0) out vec4 color;

void main()
{
    int quadrant = 0; // quad is centered
    if (gl_FragCoord.x < 320.0 && gl_FragCoord.y < 240.0) {
        // red texture
        quadrant = 0;

    } else if (gl_FragCoord.x >= 320.0 && gl_FragCoord.y < 240.0) {
        // green texture
        quadrant = 1;

    } else if (gl_FragCoord.x < 320.0 && gl_FragCoord.y >= 240.0) {
        // blue texture
        quadrant = 2;

    } else {
        // yellow texture
        quadrant = 3;
    }

    uint index = pc.textureIndices[quadrant];
    color = texture(sampler2D(texs[nonuniformEXT(index)], samp), aTexCoord);
}