
struct PSInput
{
    float4 pos : SV_POSITION;
};

struct GSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

[maxvertexcount(9)]
void main(
    triangle PSInput inputs[3], 
    inout TriangleStream<GSOutput> stream)
{
    float2 offsets[3] = {
        float2(-0.6, 0.0 ),
        float2( 0.0, 0.0 ),
        float2( 0.6, 0.0 )
    };

    float4 colors[3] = {
        float4( 1.0, 0.0, 0.0, 1.0 ),
        float4( 0.0, 1.0, 0.0, 1.0 ),
        float4( 0.0, 0.0, 1.0, 1.0 )
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            GSOutput output;
            output.pos = inputs[j].pos;
            output.pos.xy += offsets[i];
            output.color = colors[i];

            stream.Append(output);
        }
        stream.RestartStrip();
    }
}