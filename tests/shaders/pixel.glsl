
#version 330 core

layout(location = 1) in vec2 v_TexCoords;

layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;

void main()
{
    color = texture(u_Texture, v_TexCoords);
}