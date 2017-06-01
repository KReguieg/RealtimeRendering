/*
 *  vectors: geom shader examples by Hartmut Schirmacher
 */

#version 150

uniform mat4 modelViewMatrix;

uniform int vectorToShow;

in vec3 position_MC;
in vec3 normal_MC;
in vec3 tangent_MC;
in vec3 bitangent_MC;

out VertexData {
    vec3 vector;
} VertexOut;


void main()
{
    // which vector to show?
    vec3 vector = vec3(0,0,0);
    switch(vectorToShow) {
    case 1:
        vector = normal_MC;
        break;
    case 2:
        vector = tangent_MC;
        break;
    case 3:
        vector = bitangent_MC;
        break;
    };

    // transform position and vector to be shown
    VertexOut.vector = (modelViewMatrix * vec4(vector,0)).xyz;
    gl_Position      = modelViewMatrix * vec4(position_MC,1);
}

