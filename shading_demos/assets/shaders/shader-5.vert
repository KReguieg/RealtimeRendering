
// vertex shader that passes along untransformed normal vector
// and that deforms the model in camera space

#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec3 position_MC;
in vec3 normal_MC;

out vec3 fragNormal;

void main(void) {
    vec4 position_EC = viewMatrix * modelMatrix * vec4(position_MC,1);
    position_EC *= vec4(0.3,1,1,1);
    gl_Position = projectionMatrix * position_EC;
    fragNormal  = normal_MC;
}

