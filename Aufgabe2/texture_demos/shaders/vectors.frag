/* 
 *  vectors: geom shader examples by Hartmut Schirmacher
 */

#version 150

uniform vec3 vectorColor;

in VertexData {
    vec3 normal;
} VertexIn;

out vec4 outColor;

void main(void)
{
    outColor = vec4(vectorColor, 1);
}
