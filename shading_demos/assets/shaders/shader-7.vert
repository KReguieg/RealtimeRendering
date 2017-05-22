
// vertex shader that passes along untransformed normal vector
// as well as untransformed texture coordinates

#version 150

uniform mat4 modelViewProjectionMatrix;

in vec3 position_MC;
in vec2 texcoord;

out vec2 fragTexCoord;

void main(void) {
    gl_Position   = modelViewProjectionMatrix * vec4(position_MC,1);
    fragTexCoord  = texcoord;
}

