
struct VSInput 
{
    float2 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.pos, 0.0, 1.0);
    output.uv = input.uv;
    return output;
}

// GLSL equivalent

// #version 450

// layout(location = 0) in vec2 aPosition;
// layout(location = 1) in vec2 aTexCoord;

// layout(location = 0) out vec2 vTexCoord;

// void main()
// {
//     gl_Position = vec4(aPosition, 0.0, 1.0);
//     vTexCoord = aTexCoord;
// }