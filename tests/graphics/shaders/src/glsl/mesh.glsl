
#version 460
#extension GL_EXT_mesh_shader : require

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(max_vertices = 4, max_primitives = 2) out;
layout(triangles) out;

layout(location = 0) out vec4 vColors[];

void main()
{
    SetMeshOutputsEXT(4, 2);
    gl_MeshVerticesEXT[0].gl_Position = vec4(-0.5, 0.5, 0.0, 1.0);
    gl_MeshVerticesEXT[1].gl_Position = vec4( 0.5, 0.5, 0.0, 1.0);
    gl_MeshVerticesEXT[2].gl_Position = vec4( 0.5,-0.5, 0.0, 1.0);
    gl_MeshVerticesEXT[3].gl_Position = vec4(-0.5,-0.5, 0.0, 1.0);

    // colors
    vColors[0] = vec4(1.0, 0.0, 0.0, 1.0);
    vColors[1] = vec4(0.0, 1.0, 0.0, 1.0);
    vColors[2] = vec4(0.0, 0.0, 1.0, 1.0);
    vColors[3] = vec4(1.0, 0.0, 0.0, 1.0);

    gl_PrimitiveTriangleIndicesEXT[0] = uvec3(0, 1, 2);
    gl_PrimitiveTriangleIndicesEXT[1] = uvec3(0, 2, 3);
}