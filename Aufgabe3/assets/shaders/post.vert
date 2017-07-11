/*
 * Simple Vertex Shader for Post Processing
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 */

#version 150

// transformation matrices as provided by RTR app
uniform mat4 modelViewProjectionMatrix;

// tex coords
in  vec2 texcoord;
out vec2 texcoord_frag;
out float z;

// position and normal vectors
in vec4 position_MC;

void main(void) {

    // position to clip coordinates
    gl_Position = modelViewProjectionMatrix * position_MC;
    z = gl_Position.z;
    texcoord_frag = texcoord;
}
