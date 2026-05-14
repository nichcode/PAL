
#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

layout(location = 0) out vec4 outColor;

void main()
{
    vec2 offsets[3] = {
        vec2(-0.6, 0.0 ),
        vec2( 0.0, 0.0 ),
        vec2( 0.6, 0.0 )
    };

    vec4 colors[3] = {
        vec4( 1.0, 0.0, 0.0, 1.0 ),
        vec4( 0.0, 1.0, 0.0, 1.0 ),
        vec4( 0.0, 0.0, 1.0, 1.0 )
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            gl_Position = gl_in[j].gl_Position;
            gl_Position.xy += offsets[i];
            outColor = colors[i];

            EmitVertex();
        }
        EndPrimitive();
    }
}