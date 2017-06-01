/*
 *  geom shader examples by Hartmut Schirmacher
 */

#version 150

// transformation matrices as provided by Cinder
uniform mat4 modelViewProjectionMatrix;

// position and normal vectors
in vec4 position_MC;

void main()
{
    vec4 pos     = modelViewProjectionMatrix * position_MC;
    pos.z       -= 0.001;
    gl_Position  = pos;
}
