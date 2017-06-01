/*
 *  vectors: geom shader examples by Hartmut Schirmacher
 */

#version 150

uniform mat4 projectionMatrix;

layout (triangles) in;
layout (line_strip, max_vertices=6) out;

in VertexData {
    vec3 vector;
} VertexIn[3];

uniform float scale;

void main()
{
    // for each vertex, generate a line showing the vector
    for(int i=0; i<3; i++) {
        vec3 v = normalize(VertexIn[i].vector) * scale;
        vec4 p  = gl_in[i].gl_Position;
        vec4 q = p + vec4(v,0);
        // move point positions 1% towards the camera
        gl_Position = projectionMatrix * (p * vec4(1,1,0.99,1)); EmitVertex();
        gl_Position = projectionMatrix * (q * vec4(1,1,0.99,1)); EmitVertex();
        EndPrimitive();
    }
}
