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

// position and normal vectors
in vec4 position_MC;

void main(void) {
    // position to clip coordinates
    gl_Position = modelViewProjectionMatrix * position_MC;
    texcoord_frag = texcoord;   
}
