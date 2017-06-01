/*
 *  vectors: geom shader examples by Hartmut Schirmacher
 */

#version 150

uniform mat4 projectionMatrix;

layout (triangles) in;
layout (line_strip, max_vertices=2) out;

in VertexData {
    vec3 vector;
} VertexIn[3];

uniform float scale;

void main()
{
    vec3 n = (normalize(VertexIn[0].vector) +
              normalize(VertexIn[1].vector) +
              normalize(VertexIn[2].vector)   ) / 3.0;
    
    vec4 p  = (gl_in[0].gl_Position +
               gl_in[1].gl_Position +
               gl_in[2].gl_Position  ) / 3.0;
    
    vec4 q = p + vec4(n,0)*scale;
    
    gl_Position = projectionMatrix * p; EmitVertex();
    gl_Position = projectionMatrix * q; EmitVertex();
    EndPrimitive();
}
