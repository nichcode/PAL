
#version 450

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec4 vColor;

void main()
{
    gl_Position = vec4(aPosition.x, -aPosition.y, 0.0, 1.0);
    vColor = vec4(aColor, 1.0);
}