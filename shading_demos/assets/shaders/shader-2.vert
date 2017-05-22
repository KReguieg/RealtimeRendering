
// minimal vertex shader

#version 150

uniform mat4 modelViewProjectionMatrix;

in vec3 position_MC;

void main(void) {
    gl_Position = modelViewProjectionMatrix * vec4(position_MC, 1);
}
