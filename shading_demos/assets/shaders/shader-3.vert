
// vertex shader that passes along untransformed normal vector

#version 150

uniform mat4 modelViewProjectionMatrix;

in vec3 position_MC;
in vec3 normal_MC;

out vec3 fragNormal;

void main(void) {
    gl_Position = modelViewProjectionMatrix * vec4(position_MC,1);
    fragNormal  = normal_MC;
}

