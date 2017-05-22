
// vertex shader that passes along untransformed normal vector
// and that queezes in local z direction

#version 150

uniform mat4 modelViewProjectionMatrix;

in vec3 position_MC;
in vec3 normal_MC;

out vec3 fragNormal;

void main(void) {
    vec4 pos = vec4(position_MC,1) * vec4(1,1,0.2,1);
    gl_Position = modelViewProjectionMatrix * pos;
    fragNormal  = normal_MC;
}

