
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

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.pos, 0.0, 1.0);
    output.color = float4(input.color, 1.0);
    return output;
}