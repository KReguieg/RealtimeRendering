
// vertex shader that passes along untransformed normal vector
// and that deforms local x and z according to wave func

#version 150

uniform mat4 modelViewProjectionMatrix;
uniform float time;

in vec3 position_MC;
in vec3 normal_MC;

out vec3 fragNormal;

// wave function
const float pi2 = asin(1.0);
float wave(float x, float t, float A,
           float f, float c, float p0)
{
    return A * sin(pi2 * f * (t - x/c) + p0);
}


void main(void) {
    
    vec3  direction = vec3(0, 1.0, 0.0);
    float projected = dot(direction, position_MC);
    vec3  dispdir   = position_MC - direction * projected;
    float disp      = wave(projected, time, 0.2, 4.0, 0.05, 0.0);
    vec3  rippled   = position_MC + dispdir * disp;

    gl_Position = modelViewProjectionMatrix * vec4(rippled,1.0);
    fragNormal  = normal_MC;
}

