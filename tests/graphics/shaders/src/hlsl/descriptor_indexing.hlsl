
Texture2D texs[] : register(t0, space0); // set 0
SamplerState samp : register(s0, space0); // set 0

cbuffer PushConstants : register(b0)
{
    uint textureIndices[4];
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PSInput input) : SV_Target
{
    int quadrant = 0; // quad is centered
    if (input.pos.x < 320.0 && input.pos.y < 240.0) {
        // red texture
        quadrant = 0;

    } else if (input.pos.x >= 320.0 && input.pos.y < 240.0) {
        // green texture
        quadrant = 1;

    } else if (input.pos.x < 320.0 && input.pos.y >= 240.0) {
        // blue texture
        quadrant = 2;

    } else {
        // yellow texture
        quadrant = 3;
    }

    uint index = textureIndices[quadrant];
    return texs[NonUniformResourceIndex(index)].Sample(samp, input.uv);
}