
struct MSOutput 
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    out vertices MSOutput meshVertices[4], 
    out indices uint3 triangleIndices[2])
{
    SetMeshOutputCounts(4, 2);
    meshVertices[0].position = float4(-0.5, 0.5, 0.0, 1.0);
    meshVertices[1].position = float4( 0.5, 0.5, 0.0, 1.0);
    meshVertices[2].position = float4( 0.5,-0.5, 0.0, 1.0);
    meshVertices[3].position = float4(-0.5,-0.5, 0.0, 1.0);

    meshVertices[0].color = float4(1.0, 0.0, 0.0, 1.0);
    meshVertices[1].color = float4(0.0, 1.0, 0.0, 1.0);
    meshVertices[2].color = float4(0.0, 0.0, 1.0, 1.0);
    meshVertices[3].color = float4(1.0, 0.0, 0.0, 1.0);

    triangleIndices[0] = uint3(0, 1, 2);
    triangleIndices[1] = uint3(0, 2, 3);
}