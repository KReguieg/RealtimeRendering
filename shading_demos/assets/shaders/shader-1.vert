
// minimal vertex shader

#version 150

// transformation matrices
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

// in: position in model coordinates (_MC)
in vec3 position_MC;

void main(void) {
    gl_Position = projectionMatrix *
                  viewMatrix *
                  modelMatrix *
                  vec4(position_MC, 1);
}

