
struct VSOutput
{
    float4 pos : SV_POSITION;
};

VSOutput main(uint vertexId : SV_VertexID)
{
    float2 positions[3] = {
        float2( 0.0,  0.2),
        float2( 0.2, -0.2 ),
        float2(-0.2, -0.2 )
    };

    VSOutput output;
    output.pos = float4(positions[vertexId], 0.0, 1.0);
    return output;
}
