
Texture2D tex : register(t0, space0); // set 0
SamplerState samp : register(s0, space0); // set 0

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 fragMain(PSInput input) : SV_Target
{
    return tex.Sample(samp, input.uv);
}

// GLSL equivalent

// #version 450

// layout(set = 0, binding = 0) uniform texture2D tex;
// layout(set = 0, binding = 1) uniform sampler samp;

// layout(location = 0) in vec2 aTexCoord;

// layout(location = 0) out vec4 color;

// void main()
// {
//     color = texture(sampler2D(tex, samp), aTexCoord);
// }