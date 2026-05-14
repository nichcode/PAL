
#version 450

void main()
{
    vec2 positions[3] = {
        vec2( 0.0,  0.2),
        vec2( 0.2, -0.2 ),
        vec2(-0.2, -0.2 )
    };

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    gl_Position.y *= -1.0;
}
