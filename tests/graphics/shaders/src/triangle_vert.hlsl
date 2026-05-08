
struct VSInput 
{
    float2 pos : POSITION;
    float3 color : COLOR;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.pos, 0.0, 1.0);
    output.color = float4(input.color, 1.0);
    return output;
}

// GLSL equivalent

// #version 450

// layout(location = 0) in vec2 aPosition;
// layout(location = 1) in vec3 aColor;

// layout(location = 0) out vec4 vColor;

// void main()
// {
//     gl_Position = vec4(aPosition, 0.0, 1.0);
//     vColor = vec4(aColor, 1.0);
// }
