
struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 fragMain(PSInput input) : SV_Target
{
    return input.color;
}

// GLSL equivalent

// #version 450

// layout(location = 0) in vec4 aColor;

// layout(location = 0) out vec4 color;

// void main()
// {
//     color = aColor;
// }