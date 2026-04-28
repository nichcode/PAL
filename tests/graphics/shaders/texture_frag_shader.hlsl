
Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PSInput input) : SV_Target
{
    return tex.Sample(samp, input.uv);
}