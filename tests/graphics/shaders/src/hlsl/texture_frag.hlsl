
Texture2D tex : register(t0, space0); // set 0
SamplerState samp : register(s0, space0); // set 0

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PSInput input) : SV_Target
{
    return tex.Sample(samp, input.uv);
}