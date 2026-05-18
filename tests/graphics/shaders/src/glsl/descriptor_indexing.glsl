
#version 450

// texs[]; if runtime descriptor array is supported but for this example we wont use it
layout(set = 0, binding = 0) uniform texture2D texs[4];
layout(set = 0, binding = 1) uniform sampler samp;

layout(location = 0) in vec2 aTexCoord;

layout(location = 0) out vec4 color;

void main()
{
    int index = int(gl_FragCoord.x) / 160;
    index = index & 3;
    color = texture(sampler2D(texs[index], samp), aTexCoord);
}